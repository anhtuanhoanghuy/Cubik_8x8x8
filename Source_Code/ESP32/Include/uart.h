#ifndef __UART_H__
#define __UART_H__

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "driver/uart.h"
#include "driver/gpio.h"

#define UART_PORT UART_NUM_2
#define UART_TX_REQUEST 0x0A
#define UART_TX_COMPLETE 0x0B
#define UART_DATA_MAX_PACKET_LENGTH 128
#define UART_TX_MAX_SIZE 128
typedef enum {
    UART_PARSER_SUCCESS = 0,
    UART_PARSER_FAILURE,
} UART_Parser_Status_t;
  
typedef enum {
    WAIT_HEADER,
    WAIT_CMD_KEY,
    WAIT_LEN,
    WAIT_CMD_DATA,
    WAIT_CRC,
} parser_state_t;

typedef struct {
    uint8_t commandID;
    uint8_t length;
    uint8_t commandData[UART_DATA_MAX_PACKET_LENGTH];
} command_packet_t;

typedef struct {
    command_packet_t command;
    uint8_t data_index;
    uint8_t state;
} parser_context_t;

typedef struct {
    uint8_t size;
    uint8_t data[UART_TX_MAX_SIZE];
} uart_tx_packet_t;

extern TaskHandle_t uart_task_t;
extern QueueHandle_t received_eventHandle;
extern QueueHandle_t received_commandHandle;
extern QueueHandle_t transmit_Handle;
extern EventGroupHandle_t uart_event_group;
extern volatile bool uart_tx_busy;

void uart_init(void);

UART_Parser_Status_t parse_byte(parser_context_t *, uint8_t);

void parser_reset(parser_context_t *);


#endif
