#include "app.h"
#include "main.h"
#include "FreeRTOS.h"
#include "defines.h"
#include "task.h"
#include "queue.h"
#include "gpio.h"
#include "tim.h"
#include "usart.h"
#include "DHT22.h"
#include "PL9823.h"
#include "LED_mode.h"
#include "input.h"
#include "Utils.h"
#include <string.h>
#include "control.h"
int index_value = 0;
void sensor_task_handler(void) {
	DHT22_t dht22 = {0};
    HAL_GPIO_WritePin(led_test_GPIO_Port, led_test_Pin, GPIO_PIN_SET);
    TickType_t xSensorTime = xTaskGetTickCount();
    while (1) {
        if (DHT22_ReadValue(&dht22) == DHT22_OK) {
            ;
        }
        HAL_GPIO_TogglePin(led_test_GPIO_Port, led_test_Pin);
        vTaskDelayUntil(&xSensorTime, pdMS_TO_TICKS(5000));
    }
}

void led_task_handler(void) {
    TickType_t xFrameInterval_ms = xTaskGetTickCount();
    while(1) {
        for(int frame = 0; frame < 14; frame++) {
            if (PL9823_get_speed() == 0) {
                vTaskDelay(pdMS_TO_TICKS(20));
                continue;
            }
            PL9823_show(led_frames[frame]);
            vTaskDelayUntil(&xFrameInterval_ms, pdMS_TO_TICKS(SPEED_TO_DELAY(PL9823_get_speed())));
        }
    }
    
}

void uart_task_handler(void)
{
    uint8_t byte = 0;
    parser_context_t parser = {0};
    parser.state = WAIT_HEADER;
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (ring_buffer.overflow_flag == SET)
        {
            ring_buffer.overflow_flag = RESET;
            ring_buffer.tail = ring_buffer.head;
            parser_reset(&parser);
        }

        while (uart_rb_pop(&ring_buffer, &byte) == UART_RING_BUFFER_OK) {
            if (byte == 0xAA && parser.state != WAIT_HEADER)
            {
                parser_reset(&parser);
                parser.state = WAIT_CMD_KEY;
                continue;
            }

            if (parse_byte(&parser, byte) == UART_PARSER_SUCCESS) {
                xQueueSend(received_commandHandle, &parser.command, portMAX_DELAY);
                parser_reset(&parser);
            }
        }
    }
}

void control_task_handler(void) {
    command_packet_t command = {0};
    while (1) {
        if (xQueueReceive(received_commandHandle, &command, portMAX_DELAY) == pdPASS) {
            process_command(&command);
            vTaskDelay(10);
        }
    }
}

void input_task_handler(void) {
    int16_t last = __HAL_TIM_GET_COUNTER(&htim3);
    int16_t accum = 0;

    while (1)
    {
        int16_t current = __HAL_TIM_GET_COUNTER(&htim3);
        int16_t diff = (int16_t)(current - last);
        uint8_t event;

        if (diff != 0)
        {
            last = current;
            accum += diff;
        }

        if (accum >= 4)
        {
            accum -= 4;
            event = EVT_ENC_NEXT;
            xQueueSend(input_Handle, &event, 0);
        }
        else if (accum <= -4)
        {
            accum += 4;
            event = EVT_ENC_PREV;
            xQueueSend(input_Handle, &event, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void ui_task_handler(void) {
    input_event_t event;

    while (1) {
        xQueueReceive(input_Handle, &event, portMAX_DELAY);
        switch (event) {
            case EVT_BTN_BACK:
                index_value--;
                break;

            case EVT_BTN_CONFIRM:
                index_value = 100;    
                break;

            case EVT_BTN_LED:
                index_value++;
                break;
                
            case EVT_ENC_PREV:
                index_value-=10;
                break;
            
            case EVT_ENC_NEXT:
                index_value+=10;
                break;

            default:
                break;
        }
    }
}

void input_task_handler(void) {
//    uint16_t encoder_value = 0;
    while (1) {
        encoder_value = __HAL_TIM_GET_COUNTER(&htim3);
        if(encoder_value != 0) {;}
        vTaskDelay(10);
    }
}
