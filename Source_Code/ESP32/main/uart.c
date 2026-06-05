#include "uart.h"
#include <string.h>
#include "Utils.h"
 
TaskHandle_t uart_task_t;
QueueHandle_t received_eventHandle = NULL;
QueueHandle_t received_commandHandle = NULL;
QueueHandle_t transmit_Handle = NULL;
EventGroupHandle_t uart_event_group;
volatile bool uart_tx_busy = false;

void uart_init(void) {
    uart_config_t uart_config = {
        .source_clk = UART_SCLK_DEFAULT,
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    
    ESP_ERROR_CHECK(uart_set_pin(
        UART_PORT,
        GPIO_NUM_17, // TX
        GPIO_NUM_16, // RX
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE
    ));

    ESP_ERROR_CHECK(uart_driver_install(
        UART_PORT,
        2048,
        2048,
        20,
        &received_eventHandle,
        0
    ));

    char* test_str = "This is a test string.\n";
    uart_write_bytes(UART_PORT, (const char*)test_str, strlen(test_str));
}


void parser_reset(parser_context_t *parser) {
    memset(&parser->command, 0, sizeof(parser->command));
    parser->data_index = 0;
    parser->state = WAIT_HEADER;
}

void send_uart_packet(uart_tx_packet_t *packet) {
    // 1. Đẩy DATA vào Queue transmit trước (Bỏ hàng vào kho)
    if (xQueueSend(transmit_Handle, packet, pdMS_TO_TICKS(10)) == pdPASS) {
        
        // 2. DATA vào kho an toàn rồi mới tạo EVENT để bấm chuông
        uart_event_t tx_event;
        tx_event.type = (uart_event_type_t)UART_TX_REQUEST;
        tx_event.size = 0;
        
        // Bắn chuông đánh thức Task UART
        xQueueSend(received_eventHandle, &tx_event, portMAX_DELAY);
    }
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