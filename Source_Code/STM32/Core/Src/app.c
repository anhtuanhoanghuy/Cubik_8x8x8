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
#include "SH1106.h"
#include "input.h"
#include "bitmap.h"
#include "menu_control.h"
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
        input_event_t event;

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

void ui_task_handler(void)
{
    input_event_t event;
    cmd_lcd_t cmd_tx;

    while (1)
    {
        xQueueReceive(input_Handle, &event, portMAX_DELAY);

        uint8_t old_top = menu.top;
        menu.old_selected_option = menu.selected_option;

        switch (event)
        {
            case EVT_BTN_BACK:
                if (current_page == SETTING_PAGE) {
                    if (menu.selected_active == false) {
                        current_page = HOME_PAGE;
                        cmd_tx = CMD_LCD_RENDER_FULL_PAGE;
                    } else {
                        menu.selected_active = false;
                        cmd_tx = CMD_LCD_RENDER_DEACTIVE_SETTING;
                    }
                }

                break;
            case EVT_BTN_CONFIRM:
                if (current_page != HOME_PAGE) {

                    if (menu.selected_active == false) {
                        menu.selected_active = true;
                        cmd_tx = CMD_LCD_RENDER_ACTIVE_SETTING;
                    } else {
                        menu.selected_active = false;
                        cmd_tx = CMD_LCD_RENDER_DEACTIVE_SETTING;
                    }
                }
                break;
            case EVT_BTN_LED:
                
                break;
            case EVT_ENC_NEXT:
                if (menu.selected_active == false) {
                    if (current_page == HOME_PAGE)   {
                        current_page = SETTING_PAGE;

                        menu.selected_option = 0;
                        menu.top = 0;

                        cmd_tx = CMD_LCD_RENDER_FULL_PAGE;
                    } else {

                        if (menu.selected_option < MENU_COUNT - 1)
                        {
                            menu.selected_option++;

                            if (menu.selected_option >= menu.top + VISIBLE_LINES)
                            {
                                menu.top+= VISIBLE_LINES;
                            }
                            if (old_top != menu.top)
                            {
                                cmd_tx = CMD_LCD_RENDER_FULL_PAGE;
                            }
                            else
                            {
                                cmd_tx = CMD_LCD_RENDER_UPDATE_FRAME;
                            }

                        }
                    }
                } else {
                    cmd_tx = CMD_LCD_RENDER_INCREASE_VALUE_SETTING;
                }

                break;

            case EVT_ENC_PREV:
                if (menu.selected_active == false) {
                    if (current_page == HOME_PAGE) {
                        current_page = SETTING_PAGE;

                        menu.selected_option = 0;
                        menu.old_selected_option = 0;

                        menu.top = 0;

                        cmd_tx = CMD_LCD_RENDER_FULL_PAGE;
                    } else {
                        if (menu.selected_option > 0) {
                            menu.old_selected_option =
                                menu.selected_option;

                            menu.selected_option--;

                            if (menu.selected_option < menu.top)
                            {
                                menu.top -= VISIBLE_LINES;
                            }

                            if (old_top != menu.top)
                            {
                                cmd_tx = CMD_LCD_RENDER_FULL_PAGE;
                            }
                            else
                            {
                                cmd_tx = CMD_LCD_RENDER_UPDATE_FRAME;
                            }
                        }
                    }
                } else {
                    cmd_tx = CMD_LCD_RENDER_DECREASE_VALUE_SETTING;
                }
                break;

            default:
                break;
        }
        if (cmd_tx != CMD_LCD_DUMMY) {

            xQueueSend(lcd_commandHandle, &cmd_tx, pdMS_TO_TICKS(10));
            cmd_tx = CMD_LCD_DUMMY; 
        }

        
    }
}

void lcd_task_handler(void)
{
    vTaskDelay(pdMS_TO_TICKS(2000));    
    cmd_lcd_t cmd_rx;
    int value = 0;
    char buffer[6];
    char buffer1[4];
    char buffer2[30];
    render_home_page();
    
    while (1)
    {   
        xQueueReceive(lcd_commandHandle, &cmd_rx, portMAX_DELAY);
        switch (cmd_rx) {
            case CMD_LCD_RENDER_FULL_PAGE:
                menu_render();
                break;
            case CMD_LCD_RENDER_UPDATE_FRAME:
                frame_render();
                break;
            case CMD_LCD_RENDER_ACTIVE_SETTING:
                active_setting_render();
                break;
            case CMD_LCD_RENDER_DEACTIVE_SETTING:
                deactive_setting_render();
                break;
            case CMD_LCD_RENDER_INCREASE_VALUE_SETTING:
                increase_value_setting_render();
                break;
            case CMD_LCD_RENDER_DECREASE_VALUE_SETTING:
                decrease_value_setting_render();
                break;
            default:
                break;    
        }
    }   
}