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

void process_command(command_packet_t *cmd) {
    switch (cmd->commandID) {
        case CMD_LED_ON_OFF:
            LOCK();
            global_system_data.LED.status = cmd->commandData[0];
            UNLOCK();
            PL9823_set_status(global_system_data.LED.status);
            if(global_system_data.LED.status == SET) {
                vTaskResume(led_task_t);
            }
            break;
        case CMD_LED_MODE:
            LOCK();
            global_system_data.LED.mode = cmd->commandData[0];
            UNLOCK();
            PL9823_set_mode(global_system_data.LED.mode);
            break;
        case CMD_LED_BRIGHTNESS:
            LOCK();
            global_system_data.LED.brightness = cmd->commandData[0];
            UNLOCK();
            PL9823_set_brightness( global_system_data.LED.brightness);
            break;
        case CMD_LED_SPEED:
            LOCK();
            global_system_data.LED.speed = cmd->commandData[0];
            UNLOCK();
            PL9823_set_speed( global_system_data.LED.speed);
            break;
        case CMD_AMBIENT_UPDATE:
            LOCK();
            global_system_data.DHT22.temperature = cmd->commandData[0];
            global_system_data.DHT22.humidity = cmd->commandData[1];
            UNLOCK();
        default:
            break;
    }
}