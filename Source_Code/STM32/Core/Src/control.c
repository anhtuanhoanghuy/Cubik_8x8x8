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

void process_command(command_packet_t *cmd) {
    switch (cmd->commandID) {
        case CMD_LED_ON_OFF:
            global_data.LED.status = cmd->commandData[0];
            PL9823_set_status(global_data.LED.status);
            if(global_data.LED.status == SET) {
                vTaskResume(led_task_t);
            }
            break;
        case CMD_LED_MODE:
            global_data.LED.mode = cmd->commandData[0];
            PL9823_set_mode(global_data.LED.mode);
            break;
        case CMD_LED_BRIGHTNESS:
            global_data.LED.brightness = cmd->commandData[0];
            PL9823_set_brightness( global_data.LED.brightness);
            break;
        case CMD_LED_SPEED:
            global_data.LED.speed = cmd->commandData[0];
            PL9823_set_speed( global_data.LED.speed);
            break;
        default:
            break;
    }
}