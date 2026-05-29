#ifndef __MONITORING_H__
#define __MONITORING_H__

#include "FreeRTOS.h"
#include "semphr.h"
#include "usart.h"
#include "data.h"

extern SemaphoreHandle_t global_system_mutex;

#define LOCK()   xSemaphoreTake(global_system_mutex, portMAX_DELAY)
#define UNLOCK() xSemaphoreGive(global_system_mutex)

typedef enum {
    HEADER,
    COMMAND_ID,
    PAYLOAD_LENGTH,
    // LED
    LED_STATUS_IDX,
    LED_MODE_IDX,
    LED_SPEED_IDX,
    LED_BRIGHTNESS_IDX,

    // DHT22
    TEMPERATURE_IDX,
    HUMIDITY_IDX,

    // LCD
    LCD_STATUS_IDX,
    LCD_AUTO_OFF_IDX,

    // SYSTEM
    WIFI_STATUS_IDX,
    NOTIFICATION_IDX,
    DISP_AUTO_OFF_IDX,
    AUTO_SLEEP_IDX,
    AI_REALTIME_IDX,
    VOLUME_IDX,

    /////////////////
    CHECKSUM_IDX,
    MONITORING_FRAME_SIZE

} monitoring_data_protocol_t;

uint8_t encode_monitoring_data(uart_tx_packet_t *, const global_system_data_t *);

#endif