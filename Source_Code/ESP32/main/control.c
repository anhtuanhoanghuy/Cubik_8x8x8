#include "control.h"
#include "Defines.h"
#include "freertos/queue.h"
#include "Utils.h"
#include "String.h"
#include "Wifi.h"
#include "Monitoring.h"
#include "esp_log.h"
extern System_Variable system_Variable;

void send_ACK(const ack_packet_t *packet)
{
    if (packet == NULL)
    {
        return;
    }
    uint8_t data_monitoring[255] = {0};

    uint16_t index = 0;

    data_monitoring[index++] = 0xAA;

    data_monitoring[index++] = CMD_ACK_RESPONSE_ID;

    data_monitoring[index++] = 2;
    data_monitoring[index++] =  packet->seq_id;
    data_monitoring[index++] =  packet->status;
        // ===== Checksum (XOR) =====
    uint8_t checksum = 0;
    for (int i = 0; i < index; i++)
    {
        checksum ^= data_monitoring[i];
    }

    data_monitoring[index++] = checksum;
    send_mqtt(MONITORING, (char*)data_monitoring,index);
}

void send_command(const command_packet_t *cmd)
{
    if (cmd == NULL)
    {
        return;
    }

    uart_tx_packet_t packet_tx = {0};
    uint8_t index = 0;
    uint8_t checksum = 0;

    packet_tx.data[index++] = 0xAA;
    packet_tx.data[index++] = cmd->commandID;
    packet_tx.data[index++] = cmd->length;

    memcpy(&packet_tx.data[index], cmd->commandData, cmd->length);
    index += cmd->length;

    for (uint8_t i = 1; i < index; i++)
    {
        checksum ^= packet_tx.data[i];
    }

    packet_tx.data[index++] = checksum;
    packet_tx.size = index;

    send_uart_packet(&packet_tx);

}

ack_status_t process_command(command_packet_t *cmd) {
    switch (cmd->commandID) {
        case CMD_WIFI_CONNECT_ID: {
            // char ssid[WIFI_SSID_MAX] = {0};
            // char password[WIFI_PASS_MAX] = {0};

            // uint8_t ssid_len = cmd->commandData[0];
            // uint8_t pass_len = cmd->commandData[1];
            // ESP_LOGI("BLE", "ssid_len: %d",ssid_len);
            // ESP_LOGI("BLE", "pass_len %d",pass_len);
            // if (ssid_len > WIFI_SSID_MAX || pass_len > WIFI_PASS_MAX) return;

            // memcpy(ssid, &cmd->commandData[2], ssid_len);
            // memcpy(password, &cmd->commandData[2 + ssid_len], pass_len);        
            ESP_LOGI("BLE", "CMD_BLE_WIFI_CONNECT");
            wifi_connect("ASUS_06", "apc&4a9nkb");
            // ESP_LOGI("BLE", "Connect to %s : %s",ssid, password);
            break;
        }
    
        case CMD_WIFI_DISCONNECT_ID: {
            // ESP_LOGI("BLE", "CMD_BLE_WIFI_DISCONNECT");
            // wifi_disconnect();
            break;
        }

        // case CMD_SYSTEM_NOTIFICATION_ID:
        //     LOCK();
        //     global_system_data.notification = cmd->commandData[0];
        //     UNLOCK();
        //     return ACK_OK;

        // case CMD_SYSTEM_AI_REALTIME_ID:
        //     LOCK();
        //     global_system_data.ai_realtime = cmd->commandData[0];
        //     UNLOCK();
        //     return ACK_OK;

        // case CMD_SYSTEM_VOLUME_ID:
        //     LOCK();
        //     global_system_data.volume = cmd->commandData[0];
        //     UNLOCK();
        //     return ACK_OK;

        // case CMD_SYSTEM_WIFI_ID:
        //     LOCK();
        //     global_system_data.wifi_status = cmd->commandData[0];
        //     UNLOCK();
        //     return ACK_OK;

        case CMD_SYSTEM_MONITORING_ID: {
            send_mqtt(MONITORING, (char*)encode_monitoring(&system_Variable),sizeof(System_Variable) + 3);
            return ACK_OK;
        }

        default:
            send_command(cmd);
            return ACK_OK;
        }  
    return ACK_INVALID_PARAM;
}