#if !defined (__BLUETOOTH_H__)
#define __BLUETOOTH_H__

#include <stdint.h>
#include <stdbool.h>
#include "Defines.h"

/* =========================================================
 * BLE STATE MACHINE
 * ========================================================= */
typedef enum {
    BLE_STATE_OFF = 0,
    BLE_STATE_READY,
    BLE_STATE_ADVERTISING,      // Advertising và có thể connect
    BLE_STATE_CONNECTED,
} ble_state_t;

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

#endif // BLUETOOTH_H