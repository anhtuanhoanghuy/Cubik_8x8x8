#include "monitoring.h"
#include "command.h"

SemaphoreHandle_t global_system_mutex;

uint8_t encode_monitoring_data(uart_tx_packet_t *monitoring_data, const global_system_data_t *data) {

    // ===== FRAME HEADER =====
    monitoring_data->data[HEADER] = 0xAA;
    monitoring_data->data[COMMAND_ID] = CMD_SYSTEM_MONITORING_ID;
    monitoring_data->data[PAYLOAD_LENGTH] = MONITORING_FRAME_SIZE - 4;

    // ===== PAYLOAD =====

    // LED
    monitoring_data->data[LED_STATUS_IDX]     = data->LED.status;
    monitoring_data->data[LED_MODE_IDX]       = data->LED.mode;
    monitoring_data->data[LED_SPEED_IDX]      = data->LED.speed;
    monitoring_data->data[LED_BRIGHTNESS_IDX] = data->LED.brightness;

    // DHT22
    monitoring_data->data[TEMPERATURE_IDX]    = data->DHT22.temperature;

    monitoring_data->data[HUMIDITY_IDX]       = data->DHT22.humidity;

    // LCD
    monitoring_data->data[LCD_STATUS_IDX]     = data->LCD.status;

    monitoring_data->data[LCD_AUTO_OFF_IDX]   = data->LCD.disp_auto_off;

    // SYSTEM
    monitoring_data->data[WIFI_STATUS_IDX]    = data->wifi_status;

    monitoring_data->data[NOTIFICATION_IDX]   = data->notification;

    monitoring_data->data[DISP_AUTO_OFF_IDX]  = data->disp_auto_off;

    monitoring_data->data[AUTO_SLEEP_IDX]     = data->auto_sleep;

    monitoring_data->data[AI_REALTIME_IDX]    = data->ai_realtime;

    monitoring_data->data[VOLUME_IDX]         = data->volume;

    

    // ===== CHECKSUM =====

    monitoring_data->data[CHECKSUM_IDX] = calculate_checksum(&monitoring_data->data[COMMAND_ID], monitoring_data->data[PAYLOAD_LENGTH] + 2);

    // ===== TOTAL FRAME SIZE =====

    monitoring_data->size = MONITORING_FRAME_SIZE;

    return monitoring_data->size;
}