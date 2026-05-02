#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>
#include "usart.h"

typedef enum {
    LED_TEST_MODE_ENABLE,
    LED_TEST_MODE_DISABLE,
    LED_TEST_MODE_BLYNK_SLOW,
    LED_TEST_MODE_BLYNK_FAST,
} led_test_mode_t;

bool validateChecksum(command_packet_t *cmd, uint8_t checksum);

void led_test(led_test_mode_t mode);
#endif
