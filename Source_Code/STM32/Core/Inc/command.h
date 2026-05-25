#ifndef COMMAND_H
#define COMMAND_H

typedef enum {
    CMD_DUMMY           = 0x00,
    CMD_LED_ON_OFF_ID,
    CMD_LED_MODE_ID,
    CMD_LED_BRIGHTNESS_ID,
    CMD_LED_SPEED_ID,
    CMD_AMBIENT_UPDATE_ID,
    CMD_MONITORING_ID,
    CMD_ACK_RESPONSE_ID
} command_t;

#endif
