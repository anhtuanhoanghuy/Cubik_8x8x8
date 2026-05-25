#include "control.h"
#include "command.h"
#include "data.h"
#include "app.h"
#include "FreeRTOS.h"
#include "defines.h"
#include "task.h"
#include "queue.h"
#include "DHT22.h"
#include "PL9823.h"
#include "Utils.h"
#include "monitoring.h"

void send_ACK(ack_packet_t *packet)
{
    uart_tx_packet_t packet_tx = {0};
    uint8_t tx_buffer[UART_TX_MAX_SIZE] = {0};
    uint8_t i = 0;
    
    tx_buffer[i++] = 0xAA; //header
    tx_buffer[i++] = CMD_ACK_RESPONSE_ID; //commandID
    tx_buffer[i++] = 2; //data length
    tx_buffer[i++] = packet->seq_id;
    tx_buffer[i++] = packet->status;
    tx_buffer[i++] = CMD_ACK_RESPONSE_ID ^ 2 ^ packet->seq_id ^ packet->status;
    memcpy(&packet_tx.data, tx_buffer, i);
    packet_tx.size = i;
    xQueueSend(transmit_Handle, &packet_tx, portMAX_DELAY);

    xEventGroupSetBits(uart_event_group,UART_EVENT_TX_REQUEST);
}

ack_status_t process_command(command_packet_t *cmd) {
    switch (cmd->commandID) {
        case CMD_LED_ON_OFF_ID:
            LOCK();
            global_system_data.LED.status = cmd->commandData[0];
            UNLOCK();
            PL9823_set_status(global_system_data.LED.status);
            if(global_system_data.LED.status == SET) {
                vTaskResume(led_task_t);
            }
            return ACK_OK;
        case CMD_LED_MODE_ID:
            LOCK();
            global_system_data.LED.mode = cmd->commandData[0];
            UNLOCK();
            PL9823_set_mode(global_system_data.LED.mode);
            return ACK_OK;
        case CMD_LED_BRIGHTNESS_ID:
            LOCK();
            global_system_data.LED.brightness = cmd->commandData[0];
            UNLOCK();
            PL9823_set_brightness( global_system_data.LED.brightness);
            return ACK_OK;
        case CMD_LED_SPEED_ID:
            LOCK();
            global_system_data.LED.speed = cmd->commandData[0];
            UNLOCK();
            PL9823_set_speed( global_system_data.LED.speed);
            return ACK_OK;
        case CMD_AMBIENT_UPDATE_ID:
            LOCK();
            global_system_data.DHT22.temperature = cmd->commandData[0];
            global_system_data.DHT22.humidity = cmd->commandData[1];
            UNLOCK();
            return ACK_OK;
        default:
            return ACK_INVALID_PARAM;
    }
}