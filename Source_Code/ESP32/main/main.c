#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "app.h"

#include "Wifi.h"
#include "WebServer.h"
#include "Bluetooth.h"
#include "MQTT.h"
#include "Utils.h"
#include "Monitoring.h"

const char ssid[32] = "HELLO_CUBIK";
const char password[32] = "12345678";
static bool apsta_active = false;
QueueHandle_t wifi_ble_rx_queue = NULL; 
TimerHandle_t debounce_timer;
TimerHandle_t periodic_timer;
System_Variable system_Variable = {0};

void wifi_task(void) {
    wifi_task_handler();
}
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

    wifi_ble_rx_queue = queue_init();
    if (wifi_ble_rx_queue == NULL) {
        ESP_LOGE("QUEUE", "Queue init failed!");
    }
    wifi_init();
    wifi_store_init();
    wifi_start_webserver();
    bluetooth_init();
    mqtt_app_start();
    /* Không AP – không Web */

    xTaskCreate(
        wifi_task,
        "wifi_task",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        PRIORITY_MEDIUM,
        NULL
    )
    xTaskCreate(
        console_task,
        "console",
        4096,
        NULL,
        3,
        NULL
    );

    xTaskCreate(
        data_received_process_task,
        "wifi_ble_process",
        4096,      // stack WORD → ~16 KB
        NULL,
        4,         // priority
        NULL
    );

        // Debounce 1 giây (one-shot)
    debounce_timer = xTimerCreate(
        "debounce_timer",
        pdMS_TO_TICKS(1000),
        pdFALSE,      // one-shot
        NULL,
        publish_monitoring_callback
    );

    // Periodic 15 giây
    periodic_timer = xTimerCreate(
        "periodic_timer",
        pdMS_TO_TICKS(15000),
        pdTRUE,       // auto-reload
        NULL,
        publish_monitoring_callback
    );

    xTimerStart(periodic_timer, 0);

}

