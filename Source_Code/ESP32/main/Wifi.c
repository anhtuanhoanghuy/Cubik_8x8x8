#include <stdio.h>
#include <string.h>
#include "Wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/task.h"


static const char *TAG = "MY_CHECKER";
static esp_netif_t *ap_netif  = NULL;
static esp_netif_t *sta_netif = NULL;
static bool wifi_inited = false;
static EventGroupHandle_t wifi_event_group = NULL;
static wifi_scan_state_t scan_state = WIFI_SCAN_IDLE;
static wifi_ap_record_t ap_records[MAX_AP_NUM];
static uint16_t ap_count = MAX_AP_NUM;

EventGroupHandle_t wifi_get_event_group(void) {
    return wifi_event_group;
}

static void wifi_event_handler(void *arg,
                               esp_event_base_t event_base,
                               int32_t event_id,
                               void *event_data)
{
    if (event_base == WIFI_EVENT) {
        switch (event_id) {

        case WIFI_EVENT_SCAN_DONE:
            ESP_LOGI("WIFI", "SCAN DONE");
            esp_wifi_scan_get_ap_records(&ap_count, ap_records);
            scan_state = WIFI_SCAN_DONE;
            for (int i = 0; i < ap_count; i++) {
                ESP_LOGI("WIFI",
                    "[%2d] SSID: %-32s | RSSI: %4d | CH: %2d | Auth: %d",
                    i,
                    (char *)ap_records[i].ssid,
                    ap_records[i].rssi,
                    ap_records[i].primary,
                    ap_records[i].authmode
                );
            }
            break;

        case WIFI_EVENT_STA_CONNECTED:
            xEventGroupClearBits(wifi_event_group, WIFI_STA_DISCONNECTED_BIT);
            xEventGroupSetBits(wifi_event_group, WIFI_STA_CONNECTED_BIT);
            ESP_LOGI("WIFI", "STA connected (no IP yet)");
            break;

        case WIFI_EVENT_STA_DISCONNECTED:
            xEventGroupClearBits(wifi_event_group, WIFI_STA_CONNECTED_BIT | WIFI_STA_GOT_IP_BIT);
            xEventGroupSetBits(wifi_event_group, WIFI_STA_DISCONNECTED_BIT);
            ESP_LOGW("WIFI", "STA disconnected");
            break;

        default:
            break;
        }

    } else if (event_base == IP_EVENT &&
               event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI("WIFI", "STA connected");
        xEventGroupSetBits(wifi_event_group, WIFI_STA_GOT_IP_BIT);
    }
}

void wifi_reset(void)
{
    // 1. Ngắt Wi-Fi nếu đang chạy
    esp_wifi_stop();

    // 2. XÓA toàn bộ Wi-Fi config đã lưu trong NVS
    ESP_ERROR_CHECK(esp_wifi_restore());

    // 3. Destroy netif (TCP/IP)
    if (ap_netif) {
        esp_netif_destroy(ap_netif);
        ap_netif = NULL;
    }
    if (sta_netif) {
        esp_netif_destroy(sta_netif);
        sta_netif = NULL;
    }

    // 4. Deinit Wi-Fi driver
    esp_wifi_deinit();
    if (wifi_event_group) {
        vEventGroupDelete(wifi_event_group);
        wifi_event_group = NULL;
    }
    wifi_inited = false;

    ESP_LOGI(TAG, "Wi-Fi reset: all credentials cleared");
}

void wifi_init(void)
{
    wifi_event_group = xEventGroupCreate();
    if (wifi_event_group == NULL) {
        ESP_LOGE("WIFI", "Failed to create event group");
        return;
    }
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // Nếu NVS cũ bị lỗi, erase và init lại
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
        ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &wifi_event_handler,
            NULL,
            NULL
        )
    );

    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &wifi_event_handler,
            NULL,
            NULL
        )
    );
    wifi_inited = true;
}

void wifi_enter_station_mode(const char *ssid, const char *password)
{
    if (sta_netif == NULL) {
        sta_netif = esp_netif_create_default_wifi_sta();
    }

    wifi_config_t wifi_config = {0};

    strncpy((char *)wifi_config.sta.ssid, ssid,
             sizeof(wifi_config.sta.ssid));
    strncpy((char *)wifi_config.sta.password, password,
             sizeof(wifi_config.sta.password));

    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
}


void wifi_enter_stationAP_mode(const char *ssid, const char *password) {
    if (ap_netif == NULL) {
        ap_netif = esp_netif_create_default_wifi_ap();
    } 
    if (sta_netif == NULL) {
        sta_netif = esp_netif_create_default_wifi_sta();
    } 

    wifi_config_t wifi_config = {0};

    strncpy((char *)wifi_config.ap.ssid, ssid,
             sizeof(wifi_config.ap.ssid));
    strncpy((char *)wifi_config.ap.password, password,
             sizeof(wifi_config.ap.password));

    wifi_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.ap.max_connection = 1;
    wifi_config.ap.channel = 1;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void wifi_scan_start(void) {
    if (scan_state == WIFI_SCAN_RUNNING) return;
    wifi_scan_config_t scan_cfg = {
        .show_hidden = true
    };
    ap_count = MAX_AP_NUM;
    scan_state = WIFI_SCAN_RUNNING;
    esp_wifi_scan_start(&scan_cfg, false);
}

void wifi_scan_stop(void) {
    esp_wifi_scan_stop();
}

void wifi_connect(const char *ssid, const char *password) {
    wifi_config_t sta_cfg = {0};

    strncpy((char *)sta_cfg.sta.ssid, ssid,
             sizeof(sta_cfg.sta.ssid));
    strncpy((char *)sta_cfg.sta.password, password,
             sizeof(sta_cfg.sta.password));

    sta_cfg.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    sta_cfg.sta.pmf_cfg.capable = true;
    sta_cfg.sta.pmf_cfg.required = false;

    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_cfg));
    ESP_ERROR_CHECK(esp_wifi_connect());
}

void wifi_exit_StationAP_mode(void) {
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    if (ap_netif) {
        esp_netif_destroy(ap_netif);
        ap_netif = NULL;
    }
}

void wifi_off(void)
{
    // 1. Ngắt kết nối nếu đang connect
    esp_wifi_disconnect();

    // 2. Dừng Wi-Fi driver
    esp_wifi_stop();

    ESP_LOGI(TAG, "Wi-Fi turned OFF");
}
 
void wifi_on(void)
{
    esp_wifi_start();
    esp_wifi_connect();
}

uint16_t wifi_get_record_count() {
    return ap_count;
}

wifi_ap_record_t *wifi_get_record_list() {
    return ap_records;
}


void wifi_get_one_record_list(wifi_ap_record_t *ap_record) {
    esp_wifi_scan_get_ap_record(ap_record);
}

void wifi_clear_all_list(void) {
    esp_wifi_clear_ap_list();
}

uint8_t wifi_get_scan_state(void) {
    return scan_state;
}

void wifi_forget(void)
{
    esp_wifi_disconnect();

    wifi_config_t empty = {0};
    esp_wifi_set_config(WIFI_IF_STA, &empty);

    nvs_handle_t nvs;
    if (nvs_open("wifi", NVS_READWRITE, &nvs) == ESP_OK) {
        nvs_erase_all(nvs);
        nvs_commit(nvs);
        nvs_close(nvs);
    }

    xEventGroupClearBits(wifi_event_group,
        WIFI_STA_CONNECTED_BIT |
        WIFI_STA_GOT_IP_BIT
    );
    xEventGroupSetBits(wifi_event_group, WIFI_STA_DISCONNECTED_BIT);

    ESP_LOGI("WIFI", "WiFi credentials forgotten");
}

