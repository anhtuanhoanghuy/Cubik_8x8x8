#include "control.h"
#include "command.h"
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
        case CMD_LED_ENABLE: 
            led_test(LED_TEST_MODE_ENABLE);
            break;
        
        case CMD_LED_DISABLE: 
            led_test(LED_TEST_MODE_DISABLE);
            break;

        case CMD_LED_BLYNK_SLOW:
            led_test(LED_TEST_MODE_BLYNK_SLOW);
            break;

        case CMD_LED_BLYNK_FAST:
            led_test(LED_TEST_MODE_BLYNK_FAST);
            break;

        case CMD_LED_MODE_OFF:
            vTaskSuspend(led_task_t);
            break;
            
        case CMD_LED_MODE_ON:
            vTaskResume(led_task_t);
            break;

        case CMD_LED_SPEED_SET:
            PL9823_set_speed(cmd->commandData[0]);
            break;
            
        default:
            break;
    }
}