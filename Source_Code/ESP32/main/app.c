#include "app.h"
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "portmacro.h"
#include "uart.h"
#include <string.h>
#include "control.h"

void wifi_task_handler(void) {
    ESP_LOGW("WIFI", "Wifi task run");
    vTaskSuspend(NULL);
    while (1) {
    }
}

void uart_task_handler(void) {
    uart_event_t event;
    uint8_t byte = 0;
    parser_context_t parser = {0};
    parser.state = WAIT_HEADER;
    uart_tx_packet_t packet_to_send;
    while (1) {
        if (xQueueReceive(received_eventHandle, &event, portMAX_DELAY) == pdPASS) {
            switch ((uint8_t)event.type) {
                case UART_EVENT_RX:
                    for (uint16_t i = 0; i < event.size; i++) {
                        if (uart_read_bytes(UART_PORT, &byte, 1, 0) > 0) {
                            if (byte == 0xAA && parser.state != WAIT_HEADER) {
                                parser_reset(&parser);
                                parser.state = WAIT_CMD_KEY; // Hoặc trạng thái tiếp theo sau header của bạn
                                continue;
                            }

                            if (parse_byte(&parser, byte) == UART_PARSER_SUCCESS) {
                                // Gửi command đã parse thành công sang Task Logic xử lý
                                xQueueSend(received_commandHandle, &parser.command, portMAX_DELAY);
                                parser_reset(&parser);
                            }
                        }
                    }
                    break;

                case UART_EVENT_TX_REQUEST:
                case UART_EVENT_TX_COMPLETE:
                    
                    if (!uart_tx_busy) {
                        if (xQueueReceive(transmit_Handle, &packet_to_send, 0) == pdPASS) {
                            uart_tx_busy = true;
                            uint16_t bytes_written = uart_write_bytes(UART_PORT, (const char*)packet_to_send.data, packet_to_send.size);                           
                            if (bytes_written > 0) {
                                uart_tx_busy = false; 
                            }
                        }
                    }
                    break;

                case UART_EVENT_OVERFLOW:
                    ESP_LOGW("UART", "Hardware FIFO Overflow!");
                    uart_flush_input(UART_PORT);
                    parser_reset(&parser);
                    break;

                default:
                    break;
            }
        }
    }
}

void control_task_handler(void) {
    command_packet_t command = {0};
    while (1) {
        if (xQueueReceive(received_commandHandle, &command, portMAX_DELAY) == pdPASS) {
            ack_packet_t ack_packet;
            ack_packet.seq_id = command.commandID;
            ack_packet.status = process_command(&command);
            send_ACK(&ack_packet);
        }
    }
}