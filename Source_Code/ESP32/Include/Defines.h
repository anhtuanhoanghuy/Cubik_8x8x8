#if !defined (__DEFINES_H__)
#define __DEFINES_H__

#define PRIORITY_LOW            1
#define PRIORITY_LOW_MEDIUM     2
#define PRIORITY_MEDIUM         3
#define PRIORITY_MEDIUM_HIGH    4
#define PRIORITY_HIGH           5
#define PRIORITY_VERY_HIGH      6

#define ENABLE      1
#define DISNABLE    0
#define ON          1
#define OFF         0

#define MIN_PACKAGE_LENGTH 4
#define WIFI_BLE_DATA_MAX_PACKET_LENGTH 32
typedef struct {
    //System config
    uint8_t device_name[30];
    uint8_t device_tag[30];
    uint8_t wifiInfo[32];
    uint8_t sleepStartHour;
    uint8_t sleepStartMinute;
    uint8_t sleepEndHour;
    uint8_t sleepEndMinute;
    uint8_t voiceMessageEnable;
    uint8_t autoSleepEnable;
    uint8_t dispAutoOffEnable;
    uint8_t aiEnable;
    
    //System status
    uint8_t ledMode;
    uint8_t brightness;
    uint8_t speed;
    uint8_t volume;
    uint8_t isWiFiConnected;
    uint8_t isOnline; //MQTT connected
    uint8_t LEDStatus;
    uint8_t DispStatus;
} System_Variable;

enum {
    INVALID_ATTR_VALUE_LEN,
    DATA_VALID,
    MISTAKE_DATA,
};

typedef struct
{
    uint8_t command_id;
    uint8_t len;
    uint8_t data[WIFI_BLE_DATA_MAX_PACKET_LENGTH];
} wifi_ble_command_t;


//LED mode
enum {
    LED_MODE_RAINBOW = 0X01,
    LED_MODE_WAVE,
    LED_MODE_PULSE,
    LED_MODE_MUSIC,
    LED_MODE_HEART,
    LED_MODE_STAR,
    LED_MODE_CUSTOM,
};

typedef enum
{
    // ===== INTERNAL =====
    CMD_DUMMY_ID = 0x00,

    // ===== WIFI COMMANDS =====
    CMD_WIFI_SCAN_ID           = 0x10,
    CMD_WIFI_CONNECT_ID ,
    CMD_WIFI_DISCONNECT_ID ,

    // ===== LED COMMANDS =====
    CMD_LED_ON_OFF_ID      = 0x20,
    CMD_LED_MODE_ID,
    CMD_LED_BRIGHTNESS_ID,
    CMD_LED_SPEED_ID,
    CMD_LED_AUTO_OFF_SET_TIME_ID,

    // ===== SENSOR COMMANDS =====
    CMD_SENSOR_AMBIENT_UPDATE_ID = 0x30,

    // ===== LCD COMMANDS =====
    CMD_LCD_DISP_AUTO_OFF_ID = 0x40,

    // ===== SPEAKER COMMANDS =====
    CMD_SPEAKER_VOLUME_ID = 0x50,

    // ===== SYSTEM SETTINGS =====
    CMD_SYSTEM_NOTIFICATION_ID = 0x60,
    CMD_SYSTEM_AI_REALTIME_ID,
    CMD_SYSTEM_RESTART_ID,
    CMD_SYSTEM_RESET_ID,
    CMD_SYSTEM_DELETE_ID,
    CMD_SYSTEM_CHECK_UPDATE_ID,
    CMD_SYSTEM_TEST_MODE_ID,
    CMD_SYSTEM_NAME_CHANGE_ID,
    CMD_SYSTEM_TAG_CHANGE_ID,

    // ===== SYSTEM PACKETS =====
    CMD_SYSTEM_MONITORING_ID  = 0xF0,
    CMD_ACK_RESPONSE_ID,
    CMD_WIFI_MESSAGE,

} command_t;

#endif