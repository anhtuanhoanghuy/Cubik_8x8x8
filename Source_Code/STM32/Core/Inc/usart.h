/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define UART_BUFFER_SIZE  256 
#define UART_MIN_PACKAGE_LENGTH 4
#define UART_DATA_MAX_PACKET_LENGTH 32
#define UART_TX_MAX_SIZE 128

#define UART_EVENT_RX             (1 << 0)
#define UART_EVENT_TX_REQUEST     (1 << 1)
#define UART_EVENT_TX_COMPLETE    (1 << 2)
#define UART_EVENT_OVERFLOW       (1 << 3)

typedef enum {
  UART_RING_BUFFER_OK = 0,
  UART_RING_BUFFER_OVERFLOW,
  UART_RING_BUFFER_EMPTY,
} UART_Ring_Buffer_Status_t;

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
  volatile uint16_t head;
  volatile uint16_t tail;
  uint8_t data_buffer[UART_BUFFER_SIZE];
} ring_buffer_t;

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

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
extern TaskHandle_t uart_task_t;
extern UART_HandleTypeDef huart1;
extern QueueHandle_t transmit_Handle;
extern QueueHandle_t received_commandHandle;
extern uint8_t rx_byte;
extern ring_buffer_t ring_buffer;
extern EventGroupHandle_t uart_event_group;
extern volatile bool uart_tx_busy;

UART_Ring_Buffer_Status_t uart_rb_pop(ring_buffer_t *, uint8_t *);

UART_Parser_Status_t parse_byte(parser_context_t *, uint8_t);

void parser_reset(parser_context_t *);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
