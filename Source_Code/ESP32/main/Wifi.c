#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "Wifi.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "nvs_flash.h"
#include "nvs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

/* =========================================================
 * TAG
 * ========================================================= */
static const char *TAG = "WIFI";

/* =========================================================
 * GLOBAL
 * ========================================================= */

static esp_netif_t *sta_netif = NULL;
static esp_netif_t *ap_netif  = NULL;

static EventGroupHandle_t wifi_event_group;

/* Scan result */
static wifi_ap_record_t ap_records[MAX_AP_NUM];
static uint16_t ap_count = 0;
static wifi_scan_state_t scan_state = WIFI_SCAN_IDLE;

/* Current connecting info */
static char  current_ssid[WIFI_SSID_MAX] = {0};
static char  current_pass[WIFI_PASS_MAX] = {0};
static int8_t current_rssi = -127;
static bool wifi_config_from_user = false;

/* =========================================================
 * EVENT HANDLER
 * ========================================================= */
static void wifi_event_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    if (event_base == WIFI_EVENT) {

        switch (event_id) {

        case WIFI_EVENT_SCAN_DONE:
            ap_count = MAX_AP_NUM;
            esp_wifi_scan_get_ap_records(&ap_count, ap_records);
            scan_state = WIFI_SCAN_DONE;
            ESP_LOGI(TAG, "Scan done: %d APs", ap_count);
            break;

        case WIFI_EVENT_STA_CONNECTED:
            xEventGroupClearBits(wifi_event_group, WIFI_STA_DISCONNECTED_BIT);
            xEventGroupClearBits(wifi_event_group, WIFI_STA_FAIL_PASSWORD_BIT);
            xEventGroupSetBits(wifi_event_group, WIFI_STA_CONNECTED_BIT);
            ESP_LOGI(TAG, "STA connected");
            break;

        case WIFI_EVENT_STA_DISCONNECTED: {
            wifi_event_sta_disconnected_t *event =
                (wifi_event_sta_disconnected_t *)event_data;

            ESP_LOGW(TAG, "Disconnected, reason=%d", event->reason);

            /* Luôn clear trạng thái kết nối */
            xEventGroupClearBits(
                wifi_event_group,
                WIFI_STA_CONNECTED_BIT | WIFI_STA_GOT_IP_BIT
            );

            /* 1️⃣ User chủ động disconnect → tuyệt đối không reconnect */
            if (xEventGroupGetBits(wifi_event_group) &
                WIFI_STA_USER_DISCONNECT_BIT) {

                ESP_LOGI(TAG, "User disconnect → skip reconnect");
                return;
            }

            /* 2️⃣ Sai mật khẩu → không reconnect */
            if (event->reason == WIFI_REASON_AUTH_FAIL ||
                event->reason == WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT ||
                event->reason == WIFI_REASON_HANDSHAKE_TIMEOUT) {

                xEventGroupSetBits(wifi_event_group, WIFI_STA_FAIL_PASSWORD_BIT);
                ESP_LOGE(TAG, "Authentication failed → stop reconnect");
                return;
            }

            /* 3️⃣ Disconnected bình thường → cho phép reconnect */
            xEventGroupClearBits(wifi_event_group, WIFI_STA_FAIL_PASSWORD_BIT);
            xEventGroupSetBits(wifi_event_group, WIFI_STA_DISCONNECTED_BIT);
            break;
        }



        default:
            break;
        }
    }

    if (event_base == IP_EVENT &&
        event_id == IP_EVENT_STA_GOT_IP) {

        wifi_ap_record_t ap;
        if (esp_wifi_sta_get_ap_info(&ap) == ESP_OK) {
            current_rssi = ap.rssi;
        }

        xEventGroupSetBits(wifi_event_group, WIFI_STA_GOT_IP_BIT);
        ESP_LOGI(TAG, "Got IP (%d dBm)", current_rssi);
        if (wifi_config_from_user) {
            ESP_LOGI(TAG, "Lưu WIFI mới: %s", current_ssid);
            wifi_store_save(current_ssid, current_pass);
            wifi_config_from_user = false;
        } else {
            ESP_LOGI(TAG, "WiFi auto connect, không lưu");
        }

    }
}

// /* =========================================================
//  * RECONNECT TASK
//  * ========================================================= */
// static void wifi_reconnect_task(void *arg)
// {
//     saved_wifi_t best;

//     while (1) {
//         /* Chờ mất kết nối */
//         xEventGroupWaitBits(
//             wifi_event_group,
//             WIFI_STA_DISCONNECTED_BIT,
//             pdTRUE,     // clear bit khi nhận
//             pdFALSE,
//             portMAX_DELAY
//         );

//         EventBits_t bits = xEventGroupGetBits(wifi_event_group);

//         /* ❌ Không reconnect nếu auth fail */
//         if (bits & WIFI_STA_FAIL_PASSWORD_BIT) {
//             ESP_LOGW(TAG, "Reconnect skipped: auth failed");
//             continue;
//         }
//         /* ❌ User disconnect */
//         if (bits & WIFI_STA_USER_DISCONNECT_BIT) {
//             ESP_LOGI(TAG, "Reconnect skipped: user disconnect");
//             continue;
//         }

//         /* ❌ Không reconnect nếu đã có IP */
//         if (bits & WIFI_STA_GOT_IP_BIT) {
//             ESP_LOGI(TAG, "Already connected, skip reconnect");
//             continue;
//         }

//         ESP_LOGI(TAG, "Reconnect triggered");

//         if (wifi_select_best(&best)) {
//             ESP_LOGI(TAG, "Reconnect to %s", best.ssid);
//             wifi_connect_saved(&best);
//         } else {
//             ESP_LOGW(TAG, "No saved WiFi to reconnect");
//         }

//         vTaskDelay(pdMS_TO_TICKS(3000));
//     }
// }


/* =========================================================
 * INIT
 * ========================================================= */
void wifi_init(void)
{
    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    sta_netif = esp_netif_create_default_wifi_sta();
    ap_netif  = esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL));

    wifi_on();
}

/* =========================================================
 * SCAN
 * ========================================================= */
void wifi_scan_start(void)
{
    if (scan_state == WIFI_SCAN_RUNNING)
        return;

    wifi_scan_config_t cfg = {
        .show_hidden = true
    };

    ap_count = 0;
    scan_state = WIFI_SCAN_RUNNING;
    esp_wifi_scan_start(&cfg, false);
}

uint16_t wifi_get_record_count(void)
{
    return ap_count;
}

wifi_ap_record_t *wifi_get_record_list(void)
{
    return ap_records;
}

uint8_t wifi_get_scan_state(void)
{
    return scan_state;
}

/* =========================================================
 * CONNECT
 * ========================================================= */
void wifi_connect(const char *ssid, const char *password)
{
    wifi_config_t sta_cfg = {0};

    // Reset STA-related flags
    xEventGroupClearBits(wifi_event_group,
        WIFI_STA_CONNECTED_BIT |
        WIFI_STA_GOT_IP_BIT |
        WIFI_STA_DISCONNECTED_BIT |
        WIFI_STA_FAIL_PASSWORD_BIT
    );

    // Copy config
    strncpy((char *)sta_cfg.sta.ssid, ssid, sizeof(sta_cfg.sta.ssid) - 1);
    strncpy((char *)sta_cfg.sta.password, password, sizeof(sta_cfg.sta.password) - 1);

    // Cache for persistence
    strncpy(current_ssid, ssid, WIFI_SSID_MAX - 1);
    strncpy(current_pass, password, WIFI_PASS_MAX - 1);
    wifi_config_from_user = true;

    /*
     * QUAN TRỌNG:
     * - KHÔNG esp_wifi_stop()
     * - KHÔNG esp_wifi_set_mode()
     * - AP vẫn chạy bình thường
     */
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_cfg));
    ESP_ERROR_CHECK(esp_wifi_connect());

    ESP_LOGI(TAG, "connectWifi(): connecting to STA [%s]", ssid);
}

void wifi_disconnect(void)
{
    esp_wifi_disconnect();
    xEventGroupClearBits(wifi_event_group,
    WIFI_STA_FAIL_PASSWORD_BIT | WIFI_STA_DISCONNECTED_BIT);
}

void wifi_on(void)
{
    // ⚠️ SET MODE DUY NHẤT Ở ĐÂY
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi APSTA initialized");
}

void wifi_off(void)
{
    esp_wifi_disconnect();
    esp_wifi_stop();
    ESP_LOGI(TAG, "WiFi OFF");
}

// /* =========================================================
//  * STORE
//  * ========================================================= */

void wifi_store_init(void)
{
    wifi_config_t cfg = {0};

    if (wifi_store_load(&cfg)) {
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cfg));
        ESP_ERROR_CHECK(esp_wifi_connect());
    }
}

bool wifi_store_load(wifi_config_t *cfg)
{
    nvs_handle_t nvs;
    size_t len;

    if (nvs_open("WIFI_STORE", NVS_READONLY, &nvs) != ESP_OK)
        return false;

    len = sizeof(cfg->sta.ssid);
    if (nvs_get_str(nvs, "ssid", (char *)cfg->sta.ssid, &len) != ESP_OK)
    {
        nvs_close(nvs);
        return false;
    }

    len = sizeof(cfg->sta.password);
    if (nvs_get_str(nvs, "pass", (char *)cfg->sta.password, &len) != ESP_OK)
    {
        nvs_close(nvs);
        return false;
    }

    nvs_close(nvs);
    return true;
}

void wifi_store_save(const char *ssid, const char *pass)
{
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("WIFI_STORE", NVS_READWRITE, &nvs);
    if (err != ESP_OK) return;

    nvs_set_str(nvs, "ssid", (const char *)ssid);
    nvs_set_str(nvs, "pass", (const char *)pass);

    nvs_commit(nvs);
    nvs_close(nvs);

    ESP_LOGI("WIFI_STORE", "WiFi saved: %s", ssid);
}

/* =========================================================
 * EVENT GROUP API
 * ========================================================= */
EventGroupHandle_t wifi_get_event_group(void)
{
    return wifi_event_group;
}

bool wifi_get_current_status(wifi_status_info_t *info)
{
    if (!info) return false;

    wifi_ap_record_t ap;
    esp_err_t err = esp_wifi_sta_get_ap_info(&ap);

    if (err == ESP_OK) {
        info->connected = true;
        strncpy(info->ssid, (char *)ap.ssid, WIFI_SSID_MAX);
        info->rssi = ap.rssi;
        info->secure = (ap.authmode != WIFI_AUTH_OPEN) ? true : false;
        return true;
    }

    info->connected = false;
    info->ssid[0] = 0;
    info->rssi = -127;
    info->secure = false;
    return false;
}
