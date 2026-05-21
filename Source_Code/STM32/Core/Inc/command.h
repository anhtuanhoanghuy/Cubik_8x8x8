#ifndef COMMAND_H
#define COMMAND_H

typedef enum {
    CMD_DUMMY           = 0x00,
    CMD_LED_ON_OFF,
    CMD_LED_MODE,
    CMD_LED_BRIGHTNESS,
    CMD_LED_SPEED,
    CMD_AMBIENT_UPDATE,

} command_t;

#endif
