#if !defined (__BLUETOOTH_H__)
#define __BLUETOOTH_H__

#include <stdint.h>
#include <stdbool.h>

#define BLE_DATA_MAX_PACKET_LENGTH 255

/* =========================================================
 * BLE STATE MACHINE
 * ========================================================= */
typedef enum {
    BLE_STATE_OFF = 0,
    BLE_STATE_READY,
    BLE_STATE_ADVERTISING,      // Advertising và có thể connect
    BLE_STATE_CONNECTED,
} ble_state_t;

enum {
    CMD_BLE_WIFI_CONNECT    = 0x01,
    CMD_BLE_WIFI_DISCONNECT = 0X02,
    CMD_BLE_WIFI_ON_OFF     = 0X03,
    CMD_BLE_TEST_MODE       = 0X04
};

typedef struct
{
    uint8_t command_id;
    uint8_t len;
    uint8_t data[BLE_DATA_MAX_PACKET_LENGTH];
} ble_command_t;

/**
 * @brief Khởi tạo Bluetooth stack
 * 
 * Hàm này phải được gọi SAU khi nvs_flash_init() đã được gọi
 */
void bluetooth_init(void);

/**
 * @brief Bật BLE advertising và cho phép kết nối
 * 
 * Khi gọi hàm này, ESP32 sẽ:
 * - Phát BLE advertising packets (thiết bị sẽ thấy)
 * - Cho phép thiết bị khác kết nối vào
 * - Cung cấp GATT service để đọc/ghi dữ liệu
 */
void bluetooth_start(void);

/**
 * @brief Dừng BLE advertising
 */
void bluetooth_stop(void);

/**
 * @brief Kiểm tra trạng thái kết nối
 * 
 * @return true nếu có thiết bị đang kết nối, false nếu không
 */
bool bluetooth_is_connected(void);

void process_ble_command(ble_command_t *);

void ble_process_task(void *);

#endif // BLUETOOTH_H