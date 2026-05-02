#include "control.h"
#include "command.h"
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

        default:
            break;
    }
}