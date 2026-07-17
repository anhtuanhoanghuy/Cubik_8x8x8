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

checksum_status_t validateChecksum(command_packet_t *cmd, uint8_t checksum) {
    uint8_t calc_checksum = 0;
    calc_checksum ^= cmd->commandID;
    calc_checksum ^= cmd->length;
    for (uint8_t i = 0; i < cmd->length; i++) {
        calc_checksum ^= cmd->commandData[i];
    }
    return calc_checksum == checksum ? CHECKSUM_OK : CHECKSUM_ERROR;
}

uint8_t build_command_packet(command_packet_t *cmd, const uint8_t *data_received, uint16_t packet_length) {
    if (packet_length < MIN_PACKAGE_LENGTH) {   
        return INVALID_ATTR_VALUE_LEN;
    } 

    if (data_received[0] == 0xAA) {

        cmd->commandID = data_received[1];
        cmd->length     = data_received[2];

        // Validate len
        if (packet_length != (MIN_PACKAGE_LENGTH + cmd->length)) {
            ESP_LOGI(TAG, "Size of packet is invalid.");   
            return INVALID_ATTR_VALUE_LEN;
        } 

        uint8_t calc_checksum = 0;
        for (uint8_t i = 1; i < packet_length - 1; i++) {
            calc_checksum^= data_received[i];
        }
        // Validate Checksum
        if (data_received[packet_length - 1] != calc_checksum) {         
            return MISTAKE_DATA;
        } 
        memcpy(cmd->commandData,&data_received[3],cmd->length);
        return DATA_VALID;
    }
    return MISTAKE_DATA;
}

/* =========================================================
 * COMMAND PROCESS
 * ========================================================= */

uint8_t* encode_monitoring(System_Variable *system_Variable)
{
    static uint8_t data_monitoring[255] = {0};
    uint16_t index = 0;

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
    data_monitoring[index++] = system_Variable->voiceMessageEnable;
    data_monitoring[index++] = system_Variable->autoSleepEnable;
    data_monitoring[index++] = system_Variable->dispAutoOffEnable;
    data_monitoring[index++] = system_Variable->aiEnable;
    data_monitoring[index++] = system_Variable->ledMode;
    data_monitoring[index++] = system_Variable->brightness;
    data_monitoring[index++] = system_Variable->speed;
    data_monitoring[index++] = system_Variable->volume;
    data_monitoring[index++] = system_Variable->isWiFiConnected;
    data_monitoring[index++] = system_Variable->isOnline;
    data_monitoring[index++] = system_Variable->LEDStatus;
    data_monitoring[index++] = system_Variable->DispStatus;

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