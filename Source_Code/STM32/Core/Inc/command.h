#ifndef COMMAND_H
#define COMMAND_H

typedef enum {
    CMD_DUMMY           = 0x00,
    CMD_LED_ENABLE,
    CMD_LED_DISABLE,
    CMD_LED_BLYNK_SLOW,
    CMD_LED_BLYNK_FAST,
    CMD_LED_MODE_OFF,
    CMD_LED_MODE_ON,
    CMD_LED_SPEED_SET,
} command_t;

#endif
