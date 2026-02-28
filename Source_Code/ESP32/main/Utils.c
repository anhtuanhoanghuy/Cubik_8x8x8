#include "Utils.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "Defines.h"
#include "Wifi.h"
#include "Bluetooth.h"
#include "Monitoring.h"


static const char *TAG = "DATA_RECEIVED";
extern QueueHandle_t wifi_ble_rx_queue; 

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
        }
    }
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
        mqtt_monitoring("test");
        break;
    
    default:
        ESP_LOGW("BLE", "⚠️ Unknown CMD: 0x%02X", cmd->command_id);
    }
}