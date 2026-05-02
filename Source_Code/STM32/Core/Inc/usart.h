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

/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define UART_BUFFER_SIZE  256 
#define UART_MIN_PACKAGE_LENGTH 4
#define UART_DATA_MAX_PACKET_LENGTH 32

typedef enum {
  UART_RING_BUFFER_OK = 0,
  UART_RING_BUFFER_OVERFLOW,
  UART_RING_BUFFER_EMPTY,
} UART_Ring_Buffer_Status_t;

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
  volatile uint8_t overflow_flag; // Flag to indicate buffer overflow
} ring_buffer_t;

typedef struct {
  uint8_t commandID;
  uint8_t length;
  uint8_t commandData[UART_DATA_MAX_PACKET_LENGTH];
} command_packet_t;

typedef struct {
    command_packet_t command;
    uint8_t data_index;
    parser_state_t state;
} parser_context_t;

extern TaskHandle_t uart_task_t;
extern UART_HandleTypeDef huart1;
extern QueueHandle_t received_commandHandle;
extern uint8_t rx_byte;
extern ring_buffer_t ring_buffer;
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
UART_Ring_Buffer_Status_t uart_rb_pop(ring_buffer_t *, uint8_t *);

bool parse_byte(parser_context_t *, uint8_t);

void parser_reset(parser_context_t *);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

