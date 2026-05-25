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

int count_digits(int n)
{
    int count = 0;

    if (n == 0) return 1;

    if (n < 0) n = -n; // xử lý số âm

    while (n > 0) {
        n /= 10;
        count++;
    }

    return count;
}

uint8_t calculate_checksum(uint8_t *data, uint8_t size) {
    uint8_t checksum = 0;

    for (uint8_t i = 0; i < size; i++) {
        checksum ^= data[i];
    }

    return checksum;
}