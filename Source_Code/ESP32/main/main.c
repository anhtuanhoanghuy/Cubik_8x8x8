#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "Wifi.h"
#include "WebServer.h"

const char ssid[32] = "HELLO_CUBIK";
const char password[32] = "12345678";
static bool apsta_active = false;

static void console_task(void *arg)
{
    int ch;

    while (1) {
        ch = getchar();
        if (ch == 'W' || ch == 'w') {

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
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


void app_main(void)
{
    ESP_LOGI("MAIN", "Boot");

    wifi_init();
    wifi_store_init();
    wifi_start_webserver();
    /* Không AP – không Web */
    xTaskCreate(
        console_task,
        "console",
        4096,
        NULL,
        5,
        NULL
    );
}

