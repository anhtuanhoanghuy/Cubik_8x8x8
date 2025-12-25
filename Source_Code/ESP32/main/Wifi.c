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

/* Saved WiFi list */
static saved_wifi_t wifi_list[MAX_WIFI_SAVED];
static uint8_t wifi_count = 0;

/* Current connecting info */
static char  current_ssid[WIFI_SSID_MAX] = {0};
static char  current_pass[WIFI_PASS_MAX] = {0};
static int8_t current_rssi = -127;

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

        wifi_store_add_or_update(
            current_ssid,
            current_pass,
            current_rssi
        );
    }
}

/* =========================================================
 * RECONNECT TASK
 * ========================================================= */
static void wifi_reconnect_task(void *arg)
{
    saved_wifi_t best;

    while (1) {
        /* Chờ mất kết nối */
        xEventGroupWaitBits(
            wifi_event_group,
            WIFI_STA_DISCONNECTED_BIT,
            pdTRUE,     // clear bit khi nhận
            pdFALSE,
            portMAX_DELAY
        );

        EventBits_t bits = xEventGroupGetBits(wifi_event_group);

        /* ❌ Không reconnect nếu auth fail */
        if (bits & WIFI_STA_FAIL_PASSWORD_BIT) {
            ESP_LOGW(TAG, "Reconnect skipped: auth failed");
            continue;
        }
        /* ❌ User disconnect */
        if (bits & WIFI_STA_USER_DISCONNECT_BIT) {
            ESP_LOGI(TAG, "Reconnect skipped: user disconnect");
            continue;
        }

        /* ❌ Không reconnect nếu đã có IP */
        if (bits & WIFI_STA_GOT_IP_BIT) {
            ESP_LOGI(TAG, "Already connected, skip reconnect");
            continue;
        }

        ESP_LOGI(TAG, "Reconnect triggered");

        if (wifi_select_best(&best)) {
            ESP_LOGI(TAG, "Reconnect to %s", best.ssid);
            wifi_connect_saved(&best);
        } else {
            ESP_LOGW(TAG, "No saved WiFi to reconnect");
        }

        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}


/* =========================================================
 * INIT
 * ========================================================= */
void wifi_init(void)
{
    wifi_event_group = xEventGroupCreate();

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL));

    ESP_ERROR_CHECK(esp_event_handler_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL));

    sta_netif = esp_netif_create_default_wifi_sta();

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    wifi_store_load();

    // xTaskCreate(
    //     wifi_reconnect_task,
    //     "wifi_reconnect",
    //     4096,
    //     NULL,
    //     5,
    //     NULL
    // );

    ESP_LOGI(TAG, "WiFi initialized");
}

/* =========================================================
 * MODE
 * ========================================================= */
void wifi_enter_station_mode(const char *ssid, const char *password)
{
    wifi_config_t cfg = {0};
    xEventGroupClearBits(wifi_event_group, WIFI_STA_FAIL_PASSWORD_BIT);
    strncpy((char *)cfg.sta.ssid, ssid, sizeof(cfg.sta.ssid) - 1);
    strncpy((char *)cfg.sta.password, password, sizeof(cfg.sta.password) - 1);

    strncpy(current_ssid, ssid, WIFI_SSID_MAX - 1);
    strncpy(current_pass, password, WIFI_PASS_MAX - 1);

    xEventGroupClearBits(wifi_event_group,
        WIFI_STA_CONNECTED_BIT |
        WIFI_STA_GOT_IP_BIT |
        WIFI_STA_DISCONNECTED_BIT|
        WIFI_STA_FAIL_PASSWORD_BIT
    );

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &cfg));
    ESP_ERROR_CHECK(esp_wifi_connect());
}

void wifi_enter_stationAP_mode(const char *ssid, const char *password)
{
    if (!ap_netif)
        ap_netif = esp_netif_create_default_wifi_ap();

    wifi_config_t cfg = {0};

    strncpy((char *)cfg.ap.ssid, ssid, sizeof(cfg.ap.ssid) - 1);
    strncpy((char *)cfg.ap.password, password, sizeof(cfg.ap.password) - 1);

    cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;
    cfg.ap.max_connection = 1;
    cfg.ap.channel = 1;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &cfg));
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
    wifi_enter_station_mode(ssid, password);
}

void wifi_connect_saved(const saved_wifi_t *w)
{
    wifi_enter_station_mode(w->ssid, w->password);
    current_rssi = w->last_rssi;
}

void wifi_disconnect(void)
{
    esp_wifi_disconnect();
    xEventGroupClearBits(wifi_event_group,
    WIFI_STA_FAIL_PASSWORD_BIT | WIFI_STA_DISCONNECTED_BIT);
}

/* =========================================================
 * FORGET / OFF
 * ========================================================= */
void wifi_forget(void)
{
    esp_wifi_disconnect();
    wifi_count = 0;
    wifi_store_save();
    ESP_LOGI(TAG, "All saved WiFi forgotten");
}

void wifi_off(void)
{
    esp_wifi_disconnect();
    esp_wifi_stop();
    ESP_LOGI(TAG, "WiFi OFF");
}

/* =========================================================
 * STORE
 * ========================================================= */
void wifi_store_load(void)
{
    nvs_handle_t nvs;
    if (nvs_open("wifi_store", NVS_READONLY, &nvs) != ESP_OK) {
        wifi_count = 0;
        return;
    }

    size_t size = sizeof(wifi_list);
    if (nvs_get_blob(nvs, "wifi_list", wifi_list, &size) == ESP_OK)
        wifi_count = size / sizeof(saved_wifi_t);
    else
        wifi_count = 0;

    nvs_close(nvs);
}

void wifi_store_save(void)
{
    nvs_handle_t nvs;
    if (nvs_open("wifi_store", NVS_READWRITE, &nvs) == ESP_OK) {
        nvs_set_blob(nvs, "wifi_list",
                     wifi_list,
                     wifi_count * sizeof(saved_wifi_t));
        nvs_commit(nvs);
        nvs_close(nvs);
    }
}

void wifi_store_add_or_update(const char *ssid,
                              const char *pass,
                              int8_t rssi)
{
    for (int i = 0; i < wifi_count; i++) {
        if (strcmp(wifi_list[i].ssid, ssid) == 0) {
            strncpy(wifi_list[i].password, pass, WIFI_PASS_MAX - 1);
            wifi_list[i].last_rssi = rssi;
            wifi_list[i].last_used = esp_log_timestamp();
            wifi_store_save();
            return;
        }
    }

    if (wifi_count == MAX_WIFI_SAVED) {
        int oldest = 0;
        for (int i = 1; i < wifi_count; i++)
            if (wifi_list[i].last_used < wifi_list[oldest].last_used)
                oldest = i;
        wifi_list[oldest] = wifi_list[wifi_count - 1];
        wifi_count--;
    }

    strncpy(wifi_list[wifi_count].ssid, ssid, WIFI_SSID_MAX - 1);
    strncpy(wifi_list[wifi_count].password, pass, WIFI_PASS_MAX - 1);
    wifi_list[wifi_count].last_rssi = rssi;
    wifi_list[wifi_count].last_used = esp_log_timestamp();
    wifi_count++;

    wifi_store_save();
}

bool wifi_select_best(saved_wifi_t *out)
{
    if (wifi_count == 0)
        return false;

    int best = 0;
    for (int i = 1; i < wifi_count; i++)
        if (wifi_list[i].last_rssi > wifi_list[best].last_rssi)
            best = i;

    *out = wifi_list[best];
    return true;
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
