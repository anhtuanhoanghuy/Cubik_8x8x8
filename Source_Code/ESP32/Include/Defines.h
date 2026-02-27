#if !defined (__DEFINES_H__)
#define __DEFINES_H__

#define ENABLE      1
#define DISNABLE    0
#define ON          1
#define OFF         0

#define MIN_PACKAGE_LENGTH 4
#define WIFI_BLE_DATA_MAX_PACKET_LENGTH 255

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


//command via Bluetooth
enum {
    CMD_BLE_WIFI_SCAN           = 0x00,
    CMD_BLE_WIFI_CONNECT,
    CMD_BLE_WIFI_DISCONNECT,
    CMD_BLE_WIFI_ON_OFF,
    CMD_BLE_TEST_MODE
};

//command via Bluetooth/Wifi
enum {
    CMD_WIFI_BLE_NAME_CHANGE    = 0X05,
    CMD_WIFI_BLE_TAG_CHANGE,
    CMD_WIFI_BLE_LED_ON_OFF,
    CMD_WIFI_BLE_VOICE_ON_OFF ,
    CMD_WIFI_BLE_AUTO_OFF,
    CMD_WIFI_BLE_AUTO_OFF_SET_TIME,
    CMD_WIFI_BLE_RESTART,
    CMD_WIFI_BLE_RESET,
    CMD_WIFI_BLE_DELETE,
    CMD_WIFI_BLE_LED_MODE,
    CMD_WIFI_BLE_SPEED,
    CMD_WIFI_BLE_BRIGHTNESS,
};

//command via wifi
enum {
    CMD_WIFI_CHECK_UPDATE = 0X11,
    CMD_WIFI_MESSAGE,
};

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
#endif