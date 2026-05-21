#include "monitoring.h"

SemaphoreHandle_t global_system_mutex;

uint8_t encode_monitoring_data(uint8_t* monitoring_data, global_system_data_t* data) {

    monitoring_data[PACKET_HEADER_IDX]  = 0xAA;
    monitoring_data[PACKET_COMMAND_IDX] = 0xFE;

    // LED
    monitoring_data[LED_STATUS_IDX]     = data->LED.status;
    monitoring_data[LED_MODE_IDX]       = data->LED.mode;
    monitoring_data[LED_SPEED_IDX]      = data->LED.speed;
    monitoring_data[LED_BRIGHTNESS_IDX] = data->LED.brightness;

    // DHT22
    monitoring_data[TEMPERATURE_IDX]    = (uint8_t)data->DHT22.temperature;
    monitoring_data[HUMIDITY_IDX]       = data->DHT22.humidity;

    // LCD
    monitoring_data[LCD_STATUS_IDX]     = data->LCD.status;
    monitoring_data[LCD_AUTO_OFF_IDX]   = data->LCD.disp_auto_off;

    // SYSTEM
    monitoring_data[WIFI_STATUS_IDX]    = data->wifi_status;
    monitoring_data[NOTIFICATION_IDX]   = data->notification;
    monitoring_data[DISP_AUTO_OFF_IDX]  = data->disp_auto_off;
    monitoring_data[AUTO_SLEEP_IDX]     = data->auto_sleep;
    monitoring_data[AI_REALTIME_IDX]    = data->ai_realtime;
    monitoring_data[VOLUME_IDX]         = data->volume;

    uint8_t checksum = 0;

    for(int i = 2; i < PACKET_CHECKSUM_IDX; i++)
    {
        checksum ^= monitoring_data[i];
    }

    monitoring_data[PACKET_CHECKSUM_IDX] = checksum;

    return MONITORING_PACKET_SIZE;
}