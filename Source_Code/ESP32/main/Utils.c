#include "Utils.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "Defines.h"
#include "Wifi.h"
#include "Bluetooth.h"
#include "Monitoring.h"
#include "freertos/timers.h"

static const char *TAG = "DATA_RECEIVED";
extern QueueHandle_t wifi_ble_rx_queue; 
extern TimerHandle_t debounce_timer;

bool validateChecksum(const uint8_t *data, uint8_t len, uint8_t checksum) {
    uint8_t calc_checksum = 0;
    for (uint8_t i = 0; i < len; i++) {
        calc_checksum^= data[i];
    }
    return calc_checksum == checksum;
}

uint8_t decode_data(wifi_ble_command_t *cmd, const uint8_t *data_received, uint16_t packet_length) {
    if (packet_length < MIN_PACKAGE_LENGTH) {   
        return INVALID_ATTR_VALUE_LEN;
    } 

    if (data_received[0] == 0xAA) {

        cmd->command_id = data_received[1];
        cmd->len        = data_received[2];

        // Validate len
        if (packet_length != (MIN_PACKAGE_LENGTH + cmd->len)) {
            ESP_LOGI(TAG, "Size of packet is invalid.");   
            return INVALID_ATTR_VALUE_LEN;
        } 

        // Validate Checksum
        if (!validateChecksum(data_received, packet_length - 1, data_received[packet_length - 1])) {         
            return MISTAKE_DATA;
        } 
        memcpy(cmd->data,&data_received[3],cmd->len);
        return DATA_VALID;
    }
    return MISTAKE_DATA;
}

QueueHandle_t queue_init(void) {
    QueueHandle_t queue = xQueueCreate(
        8,
        sizeof(wifi_ble_command_t)
    );
    configASSERT(queue);
    return queue;
}

/* =========================================================
 * COMMAND PROCESS
 * ========================================================= */

 void data_received_process_task(void *param)
{
    wifi_ble_command_t cmd;
    while (1) {
        if (xQueueReceive(wifi_ble_rx_queue, &cmd, portMAX_DELAY)) {
            process_command(&cmd);
            xTimerReset(debounce_timer,0);
        }
    }
}

uint8_t* encode_monitoring(System_Variable *system_Variable)
{
    static uint8_t data_monitoring[255] = {0};
    uint16_t index = 0;
    // Pack flags
    uint8_t flags = 0;

    // Header
    data_monitoring[index++] = 0xAA; //start package

    data_monitoring[index++] = 100;  //datalength

    // ===== Payload =====

    memcpy(&data_monitoring[index], system_Variable->device_name, 30);
    index += 30;

    memcpy(&data_monitoring[index], system_Variable->device_tag, 30);
    index += 30;

    memcpy(&data_monitoring[index], system_Variable->wifiInfo, 32);
    index += 32;

    data_monitoring[index++] = system_Variable->sleepStartHour;
    data_monitoring[index++] = system_Variable->sleepStartMinute;
    data_monitoring[index++] = system_Variable->sleepEndHour;
    data_monitoring[index++] = system_Variable->sleepEndMinute;

    data_monitoring[index++] = system_Variable->ledMode;
    data_monitoring[index++] = system_Variable->brightness;
    data_monitoring[index++] = system_Variable->speed;


    flags |= (system_Variable->isOnline           & 1) << 0;
    flags |= (system_Variable->bleStatus          & 1) << 1;
    flags |= (system_Variable->wifiStatus         & 1) << 2;
    flags |= (system_Variable->isBLEConnected     & 1) << 3;
    flags |= (system_Variable->isWiFiConnected    & 1) << 4;
    flags |= (system_Variable->LEDStatus          & 1) << 5;
    flags |= (system_Variable->voiceMessageStatus & 1) << 6;
    flags |= (system_Variable->autoOff            & 1) << 7;

    data_monitoring[index++] = flags;
    // ===== Checksum (XOR) =====
    uint8_t checksum = 0;
    for (int i = 0; i < index; i++)
    {
        checksum ^= data_monitoring[i];
    }

    data_monitoring[index++] = checksum;
    printf("%d\n",sizeof(data_monitoring));
    for(int i =0;i < 255;i++) {

        printf("%02X ", data_monitoring[i]);
    }
    printf("\n");
    return data_monitoring;
}

void process_command(wifi_ble_command_t *cmd)
{
    ESP_LOGI("BLE", "CMD: 0x%02X, Len: %d", cmd->command_id, cmd->len);
    
    switch (cmd->command_id) {
    
    case CMD_BLE_WIFI_CONNECT: {
        char ssid[WIFI_SSID_MAX] = {0};
        char password[WIFI_PASS_MAX] = {0};

        uint8_t ssid_len = cmd->data[0];
        uint8_t pass_len = cmd->data[1];
        ESP_LOGI("BLE", "ssid_len: %d",ssid_len);
        ESP_LOGI("BLE", "pass_len %d",pass_len);
        if (ssid_len > WIFI_SSID_MAX || pass_len > WIFI_PASS_MAX) return;

        memcpy(ssid, &cmd->data[2], ssid_len);
        memcpy(password, &cmd->data[2 + ssid_len], pass_len);        
        wifi_connect(ssid, password);
        ESP_LOGI("BLE", "CMD_BLE_WIFI_CONNECT");
        ESP_LOGI("BLE", "Connect to %s : %s",ssid, password);
        break;
    }
    
    case CMD_BLE_WIFI_DISCONNECT: {
        ESP_LOGI("BLE", "CMD_BLE_WIFI_DISCONNECT");
        wifi_disconnect();
        break;
    }

    case CMD_BLE_WIFI_ON_OFF:
        ESP_LOGI("BLE", "CMD_BLE_WIFI_ON_OFF");
        uint8_t state = cmd->data[0];
        switch (state)
        {
            case OFF:
                wifi_off();
                break;
            case ON:
                wifi_on();
                break;
            default:
                break;
        }
        break;

    case CMD_BLE_TEST_MODE:
        break;

    case CMD_WIFI_BLE_LED_ON_OFF:
        ESP_LOGI("COMMAND", "CMD_WIFI_BLE_LED_ON_OFF");
        break;
    
    default:
        ESP_LOGW("BLE", "⚠️ Unknown CMD: 0x%02X", cmd->command_id);
    }
}