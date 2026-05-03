#include "Utils.h"
#include "gpio.h"

checksum_status_t validateChecksum(command_packet_t *cmd, uint8_t checksum) {
    uint8_t calc_checksum = 0;
    calc_checksum ^= cmd->commandID;
    calc_checksum ^= cmd->length;
    for (uint8_t i = 0; i < cmd->length; i++) {
        calc_checksum ^= cmd->commandData[i];
    }
    return calc_checksum == checksum ? CHECKSUM_OK : CHECKSUM_ERROR;
}

void led_test(led_test_mode_t mode) {
    switch (mode) {
        case LED_TEST_MODE_ENABLE:
            HAL_GPIO_WritePin(led_test_GPIO_Port, led_test_Pin, RESET);
            break;
        case LED_TEST_MODE_DISABLE:
            HAL_GPIO_WritePin(led_test_GPIO_Port, led_test_Pin, SET);
            break;
        case LED_TEST_MODE_BLYNK_SLOW:
            HAL_GPIO_TogglePin(led_test_GPIO_Port, led_test_Pin);
            vTaskDelay(500); // Delay for 500ms
            break;
        case LED_TEST_MODE_BLYNK_FAST:
            HAL_GPIO_TogglePin(led_test_GPIO_Port, led_test_Pin);
            vTaskDelay(250); // Delay for 250ms 
            break;
    }
}