/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "main.h"
#include "defines.h"
#include "Utils.h"

/* USER CODE BEGIN 0 */
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
TaskHandle_t uart_task_t;
QueueHandle_t received_commandHandle = NULL;
static volatile uint16_t rx_size = 0;
uint8_t rx_byte;
ring_buffer_t ring_buffer = {0};


static void uart_rb_push(ring_buffer_t *, uint8_t);

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA1_Channel5;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if(huart->Instance == USART1) {
    uart_rb_push(&ring_buffer, rx_byte);
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (uart_task_t != NULL) {
      vTaskNotifyGiveFromISR(uart_task_t, &xHigherPriorityTaskWoken);
    }
    HAL_UART_Receive_IT(&huart1, &rx_byte, sizeof(uint8_t));
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

static void uart_rb_push(ring_buffer_t *ring_buffer, uint8_t byte) {
  uint16_t next = (ring_buffer->head + 1) % UART_BUFFER_SIZE;
  if(next != ring_buffer->tail)
  {
    ring_buffer->data_buffer[ring_buffer->head] = byte;
    ring_buffer->head = next;
  } else {
    // Buffer is full, drop the new byte and set flag
    ring_buffer->overflow_flag = SET;
  }
}

UART_Ring_Buffer_Status_t uart_rb_pop(ring_buffer_t *ring_buffer, uint8_t *byte) {
  if(ring_buffer->tail == ring_buffer->head) {
    return UART_RING_BUFFER_EMPTY;
  }
  *byte = ring_buffer->data_buffer[ring_buffer->tail];
  ring_buffer->tail = (ring_buffer->tail + 1) % UART_BUFFER_SIZE;
  return UART_RING_BUFFER_OK;
}

UART_Parser_Status_t parse_byte(parser_context_t *parser, uint8_t byte) {
    switch (parser->state) {
        case WAIT_HEADER:
            if (byte == 0xAA)
                parser->state = WAIT_CMD_KEY;
            break;

        case WAIT_CMD_KEY:
            parser->command.commandID = byte;
            parser->state = WAIT_LEN;
            break;

        case WAIT_LEN:
            if (byte <= UART_DATA_MAX_PACKET_LENGTH) {
                parser->command.length = byte;
                parser->data_index = 0;
                parser->state = (parser->command.length == 0) ? WAIT_CRC : WAIT_CMD_DATA;
            } else {
                parser->state = WAIT_HEADER;
            }
            break;

        case WAIT_CMD_DATA:
            if (parser->data_index < UART_DATA_MAX_PACKET_LENGTH) {
                parser->command.commandData[parser->data_index++] = byte;
                if (parser->data_index >= parser->command.length)
                    parser->state = WAIT_CRC;
            } else {
                parser->state = WAIT_HEADER;
            }
            break;

        case WAIT_CRC:
        {
            uint8_t checksum = byte;
            if (validateChecksum(&parser->command, checksum) == CHECKSUM_OK) {
                return UART_PARSER_SUCCESS;
            }
            parser_reset(parser);
            break;
        }
    }
    return UART_PARSER_FAILURE;
}

void parser_reset(parser_context_t *parser) {
    memset(&parser->command, 0, sizeof(parser->command));
    parser->data_index = 0;
    parser->state = WAIT_HEADER;
}

/* USER CODE END 1 */

