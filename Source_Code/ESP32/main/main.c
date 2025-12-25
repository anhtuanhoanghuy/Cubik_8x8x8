#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "Wifi.h"
#include "WebServer.h"


#define MAX_AP_NUM 30
const char ssid[32] = "HELLO_CUBIK";
const char password[32] = "12345678";
static bool apsta_active = false;

static const char *TAG = "MAIN";

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

            wifi_enter_stationAP_mode(ssid, password);
            wifi_start_webserver();

            apsta_active = true;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


void app_main(void)
{
    ESP_LOGI("MAIN", "Boot");

    wifi_init();
    wifi_store_load();

    saved_wifi_t best;

    if (wifi_select_best(&best)) {

        ESP_LOGI("MAIN", "Auto connect to %s", best.ssid);
        wifi_connect_saved(&best);

        EventBits_t bits = xEventGroupWaitBits(
            wifi_get_event_group(),
            WIFI_STA_GOT_IP_BIT | WIFI_STA_DISCONNECTED_BIT,
            pdFALSE,
            pdFALSE,
            pdMS_TO_TICKS(8000)
        );

        if (bits & WIFI_STA_GOT_IP_BIT) {
            ESP_LOGI("MAIN", "WiFi connected");
        } else {
            ESP_LOGW("MAIN", "WiFi connect failed");
        }

    } else {
        ESP_LOGW("MAIN", "No saved WiFi");
    }

    
    // wifi_enter_stationAP_mode(ssid, password);
    // wifi_start_webserver();

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

