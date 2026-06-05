#if !defined (__UTILS_H__)
#define __UTILS_H__

#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "Defines.h"
#include "uart.h"

typedef enum {
    CHECKSUM_OK = 0,
    CHECKSUM_ERROR
} checksum_status_t;    

checksum_status_t validateChecksum(command_packet_t *, uint8_t );
uint8_t decode_data(wifi_ble_command_t *, const uint8_t *, uint16_t);
QueueHandle_t queue_init(void);
void data_received_process_task(void *);
void process_command(wifi_ble_command_t *);
uint8_t* encode_monitoring(System_Variable *);

#endif