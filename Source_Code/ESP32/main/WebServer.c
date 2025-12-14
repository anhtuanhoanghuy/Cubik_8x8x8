#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "Wifi.h"
#include "WebServer.h"

static esp_err_t root_get_handler(httpd_req_t *req){
    const char html[] =
        "<html><body>"
        "<h2>WiFi Config</h2>"
        "<form method='POST' action='/wifi'>"
        "SSID:<br><input name='ssid'><br>"
        "Password:<br><input name='pass' type='password'><br><br>"
        "<input type='submit' value='Save'>"
        "</form>"
        "</body></html>";

    httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* URI handler structure for GET /uri */
httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler,
    .user_ctx = NULL
}; 

httpd_handle_t wifi_start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // Empty handle to http_server
    httpd_handle_t server = NULL;
    // Start the httpd server
    if (httpd_start(&server, &config) == ESP_OK) {
        // Register URI handlers
        httpd_register_uri_handler(server, &uri_get);
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