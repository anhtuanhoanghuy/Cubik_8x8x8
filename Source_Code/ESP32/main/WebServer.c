#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "Wifi.h"
#include "WebServer.h"

extern const char admin_html_start[] asm("_binary_Admin_html_start");
extern const char admin_html_end[]   asm("_binary_Admin_html_end");

static esp_err_t root_get_handler(httpd_req_t *req) {
    httpd_resp_send(req, admin_html_start,
        admin_html_end - admin_html_start);
    return ESP_OK;
}

static esp_err_t scan_wifi_get_handler(httpd_req_t *req) {
    ESP_LOGI("HTTP", "scan_wifi handler called");
    wifi_scan_start();
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, "{\"status\":\"scanning\"}", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t scan_result_get_handler(httpd_req_t *req)
{
    ESP_LOGI("HTTP", "scan_result handler called");
    httpd_resp_set_type(req, "application/json");
    if (wifi_get_scan_state() != WIFI_SCAN_DONE) {
        httpd_resp_send(req,
            "{\"status\":\"running\",\"aps\":[]}",
            HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    }
    char json[1024];
    int len = 0;

    uint16_t record_count = wifi_get_record_count();
    wifi_ap_record_t *list = wifi_get_record_list();

    ESP_LOGI("WIFI", "%d wifi found", record_count);

    for (int i = 0; i < record_count; i++) {
        ESP_LOGI("WIFI",
            "[%2d] SSID: %-32s | RSSI: %4d | CH: %2d | Auth: %d",
            i,
            (char *)list[i].ssid,
            list[i].rssi,
            list[i].primary,
            list[i].authmode
        );
    }

    /* Build JSON */
    len += snprintf(json + len, sizeof(json) - len,
        "{\"status\":\"done\",\"count\":%d,\"aps\":[",
        record_count
    );

    for (int i = 0; i < record_count && len < sizeof(json); i++) {
        len += snprintf(json + len, sizeof(json) - len,
            "{\"ssid\":\"%s\",\"rssi\":%d,\"secure\":%s}%s",
            (char *)list[i].ssid,
            list[i].rssi,
            (list[i].authmode != WIFI_AUTH_OPEN) ? "true" : "false",
            (i < record_count - 1) ? "," : ""
        );
    }

    len += snprintf(json + len, sizeof(json) - len, "]}");

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, len);
    return ESP_OK;
}

httpd_uri_t uri_scan_result_get = {
    .uri      = "/scan_result",
    .method   = HTTP_GET,
    .handler  = scan_result_get_handler,
    .user_ctx = NULL
};

/* URI handler structure for GET /uri */
static httpd_uri_t uri_root_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler,
    .user_ctx = NULL
}; 

/* URI handler structure for GET /uri */
static httpd_uri_t uri_scan_wifi_get = {
    .uri = "/scan_wifi",
    .method = HTTP_GET,
    .handler = scan_wifi_get_handler,
    .user_ctx = NULL
}; 


httpd_handle_t wifi_start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // Empty handle to http_server
    httpd_handle_t server = NULL;
    // Start the httpd server
    if (httpd_start(&server, &config) == ESP_OK) {
        // Register URI handlers
        httpd_register_uri_handler(server, &uri_root_get);
        httpd_register_uri_handler(server, &uri_scan_wifi_get);
        httpd_register_uri_handler(server, &uri_scan_result_get);
        ESP_LOGI("HTTP", "Starting web server...");
    }
    return server;
}

/* Function for stopping the webserver */
void wifi_stop_webserver(httpd_handle_t server) {
    if (server) {
        /* Stop the httpd server */
        httpd_stop(server);
    }
}