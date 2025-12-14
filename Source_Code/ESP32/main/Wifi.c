#include <string.h>
#include "esp_wifi.h"
#include "Wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "MY_CHECKER";
static esp_netif_t *ap_netif  = NULL;
static esp_netif_t *sta_netif = NULL;
static bool wifi_inited = false;

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
    wifi_inited = false;

    ESP_LOGI(TAG, "Wi-Fi reset: all credentials cleared");
}

void wifi_init(void)
{
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

void wifi_scan(void) {
    wifi_scan_config_t scan_cfg = {
    .show_hidden = true
    };
    esp_wifi_scan_start(&scan_cfg, true);
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
