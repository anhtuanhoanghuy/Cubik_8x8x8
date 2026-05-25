#ifndef COMMAND_H
#define COMMAND_H

typedef enum
{
    // ===== INTERNAL =====
    CMD_DUMMY_ID = 0x00,

    // ===== LED COMMANDS =====
    CMD_LED_ON_OFF_ID      = 0x10,
    CMD_LED_MODE_ID        = 0x11,
    CMD_LED_BRIGHTNESS_ID  = 0x12,
    CMD_LED_SPEED_ID       = 0x13,

    // ===== SENSOR COMMANDS =====
    CMD_SENSOR_AMBIENT_UPDATE_ID = 0x20,

    // ===== LCD COMMANDS =====
    CMD_LCD_DISP_AUTO_OFF_ID = 0x30,

    // ===== SYSTEM SETTINGS =====
    CMD_SYSTEM_NOTIFICATION_ID = 0x40,
    CMD_SYSTEM_AUTO_SLEEP_ID  = 0x41,
    CMD_SYSTEM_AI_REALTIME_ID = 0x42,
    CMD_SYSTEM_VOLUME_ID      = 0x43,
    CMD_SYSTEM_WIFI_ID        = 0x44,

    // ===== SYSTEM PACKETS =====
    CMD_SYSTEM_MONITORING_ID  = 0xF0,
    CMD_ACK_RESPONSE_ID       = 0xF1

} command_t;

#endif
