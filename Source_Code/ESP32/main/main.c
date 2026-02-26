#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "Wifi.h"
#include "WebServer.h"
#include "Bluetooth.h"
#include "MQTT.h"

const char ssid[32] = "HELLO_CUBIK";
const char password[32] = "12345678";
static bool apsta_active = false;

static void console_task(void *arg)
{
    int key;
    
    while (1) {
        // Đọc 1 byte từ UART (console)
        key = getchar();
            
        switch (key) {
        
        case 'B':
            ESP_LOGI("BLUETOOTH", "🔵 [B] pressed - Starting BLE...");
            bluetooth_start();
            ESP_LOGI("BLUETOOTH", "✅ BLE started - Ready for connections!");
            break;
        
        case 'S':
            ESP_LOGI("BLUETOOTH", "🛑 [S] pressed - Stopping BLE");
            bluetooth_stop();
            break;
        
        case 'I':
            ESP_LOGI("BLUETOOTH", "ℹ️  [I] pressed - Status info");
            ESP_LOGI("BLUETOOTH", "Connected: %s", 
                        bluetooth_is_connected() ? "YES ✅" : "NO ❌");
            break;
        
        case 'W':
            if (apsta_active) {
                ESP_LOGI("MAIN", "APSTA already active");
                continue;
            }

            ESP_LOGW("MAIN", "ENTER APSTA MODE");

        // /* 🔒 Khóa auto reconnect */
        //     xEventGroupSetBits(
        //         wifi_get_event_group(),
        //         WIFI_STA_MANUAL_CONNECT_BIT
        //     );


            apsta_active = true;
            break;
        default:
            if (key >= 32 && key <= 126) { // Printable characters
                ESP_LOGW("MAIN", "Press [B], [S], [I], [W]");
            }
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


void app_main(void)
{
    ESP_LOGI("MAIN", "Boot");

// ✅ BƯỚC 1: Khởi tạo NVS (BẮT BUỘC trước WiFi/BLE)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || 
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI("NVS", "✅ NVS initialized");


    wifi_init();
    wifi_store_init();
    wifi_start_webserver();
    bluetooth_init();
    mqtt_app_start();
    /* Không AP – không Web */


    xTaskCreate(
        console_task,
        "console",
        4096,
        NULL,
        3,
        NULL
    );

    xTaskCreate(
        ble_process_task,
        "ble_process",
        4096,      // stack WORD → ~16 KB
        NULL,
        4,         // priority
        NULL
    );

}

