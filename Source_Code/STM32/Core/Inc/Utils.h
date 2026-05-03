#ifndef __UTILS_H__
#define __UTILS_H__

#include "usart.h"

typedef enum {
    LED_TEST_MODE_ENABLE,
    LED_TEST_MODE_DISABLE,
    LED_TEST_MODE_BLYNK_SLOW,
    LED_TEST_MODE_BLYNK_FAST,
} led_test_mode_t;

typedef enum {
    CHECKSUM_OK = 0,
    CHECKSUM_ERROR
} checksum_status_t;    

checksum_status_t validateChecksum(command_packet_t *cmd, uint8_t checksum);

void led_test(led_test_mode_t mode);
#endif
