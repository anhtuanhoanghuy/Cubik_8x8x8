#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_http_server.h"

#include "Wifi.h"
#include "WebServer.h"
#include "cJSON.h"

httpd_handle_t server = NULL;

/* HTML binary */
extern const char admin_html_start[] asm("_binary_Admin_html_start");
extern const char admin_html_end[]   asm("_binary_Admin_html_end");

static const char *TAG = "HTTP";

/* =========================================================
 * ROOT PAGE
 * ========================================================= */
static esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_send(req,
        admin_html_start,
        admin_html_end - admin_html_start
    );
    return ESP_OK;
}

/* =========================================================
 * SCAN WIFI
 * ========================================================= */
static esp_err_t scan_wifi_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "/scan_wifi");

    wifi_scan_start();

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req,
        "{\"status\":\"scanning\"}",
        HTTPD_RESP_USE_STRLEN
    );
    return ESP_OK;
}

/* =========================================================
 * SCAN RESULT
 * ========================================================= */
static esp_err_t scan_result_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");

    if (wifi_get_scan_state() != WIFI_SCAN_DONE) {
        httpd_resp_send(req,
            "{\"status\":\"running\",\"aps\":[]}",
            HTTPD_RESP_USE_STRLEN
        );
        return ESP_OK;
    }

    uint16_t count = wifi_get_record_count();
    wifi_ap_record_t *list = wifi_get_record_list();

    char json[1024];
    int len = 0;

    len += snprintf(json + len, sizeof(json) - len,
        "{\"status\":\"done\",\"count\":%d,\"aps\":[",
        count
    );

    for (int i = 0; i < count && len < sizeof(json); i++) {
        len += snprintf(json + len, sizeof(json) - len,
            "{\"ssid\":\"%s\",\"rssi\":%d,\"secure\":%s}%s",
            (char *)list[i].ssid,
            list[i].rssi,
            (list[i].authmode != WIFI_AUTH_OPEN) ? "true" : "false",
            (i < count - 1) ? "," : ""
        );
    }

    len += snprintf(json + len, sizeof(json) - len, "]}");

    httpd_resp_send(req, json, len);
    return ESP_OK;
}

/* =========================================================
 * CONNECT WIFI
 * ========================================================= */
static esp_err_t connect_wifi_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "/connect_wifi");

    if (req->content_len <= 0 || req->content_len > 512) {
        httpd_resp_send_err(req,
            HTTPD_400_BAD_REQUEST,
            "Invalid content length"
        );
        return ESP_FAIL;
    }

    char *buf = calloc(1, req->content_len + 1);
    if (!buf) {
        httpd_resp_send_err(req,
            HTTPD_500_INTERNAL_SERVER_ERROR,
            "No memory"
        );
        return ESP_FAIL;
    }

    if (httpd_req_recv(req, buf, req->content_len) <= 0) {
        free(buf);
        httpd_resp_send_err(req,
            HTTPD_500_INTERNAL_SERVER_ERROR,
            "Failed to read body"
        );
        return ESP_FAIL;
    }

    cJSON *root = cJSON_Parse(buf);
    free(buf);

    if (!root) {
        httpd_resp_send_err(req,
            HTTPD_400_BAD_REQUEST,
            "Invalid JSON"
        );
        return ESP_FAIL;
    }

    cJSON *ssid = cJSON_GetObjectItem(root, "ssid");
    cJSON *password = cJSON_GetObjectItem(root, "password");

    if (!cJSON_IsString(ssid) || !cJSON_IsString(password)) {
        cJSON_Delete(root);
        httpd_resp_send_err(req,
            HTTPD_400_BAD_REQUEST,
            "Missing ssid or password"
        );
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Connecting to SSID: %s", ssid->valuestring);

    wifi_connect(ssid->valuestring, password->valuestring);

    cJSON_Delete(root);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req,
        "{\"status\":\"ok\",\"message\":\"connecting\"}",
        HTTPD_RESP_USE_STRLEN
    );

    return ESP_OK;
}

/* =========================================================
 * WIFI STATUS
 * ========================================================= */
static esp_err_t wifi_status_get_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "/wifi_status");
    EventBits_t bits = xEventGroupGetBits(wifi_get_event_group());
    const char *status = "idle";

    /* ❌ Ưu tiên lỗi trước */
    if (bits & WIFI_STA_FAIL_PASSWORD_BIT) {
        status = "fail";
    }
    else if (bits & WIFI_STA_GOT_IP_BIT) {
        status = "got_ip";
    }
    else if (bits & WIFI_STA_CONNECTED_BIT) {
        status = "connecting";
    }
    else if (bits & WIFI_STA_DISCONNECTED_BIT) {
        status = "reconnecting";
    }

    char resp[64];
    snprintf(resp, sizeof(resp),
        "{\"status\":\"%s\"}", status);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


/* =========================================================
 * DISCONNECT / FORGET
 * ========================================================= */
static esp_err_t disconnect_wifi_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "/disconnect_wifi");

    /* Đánh dấu user chủ động disconnect */
    xEventGroupSetBits(
        wifi_get_event_group(),
        WIFI_STA_USER_DISCONNECT_BIT
    );

    wifi_disconnect();

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req,
        "{\"status\":\"ok\",\"message\":\"disconnected\"}",
        HTTPD_RESP_USE_STRLEN
    );

    return ESP_OK;
}

static esp_err_t forget_wifi_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "/forget_wifi");

    wifi_forget();

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req,
        "{\"status\":\"ok\",\"message\":\"forgotten\"}",
        HTTPD_RESP_USE_STRLEN
    );
    return ESP_OK;
}

static esp_err_t exit_ap_post_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Exit AP mode");

    /* Mở lại auto reconnect */
    // xEventGroupClearBits(
    //     wifi_get_event_group(),
    //     WIFI_STA_MANUAL_CONNECT_BIT
    // );

    /* 1. Trả response trước */
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req,
        "{\"status\":\"ok\",\"message\":\"AP disabled\"}",
        HTTPD_RESP_USE_STRLEN
    );

    /* 2. Stop HTTP server */
    wifi_stop_webserver();
    // apsta_active = false;   

    /* 3. Chuyển sang STA-only */
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    return ESP_OK;
}


static esp_err_t wifi_current_get_handler(httpd_req_t *req)
{
    wifi_status_info_t info = {0};

    httpd_resp_set_type(req, "application/json");

    if (wifi_get_current_status(&info) && info.connected && info.ssid[0] != '\0') {

        char resp[192];
        snprintf(resp, sizeof(resp),
            "{\"connected\":true,\"ssid\":\"%s\",\"rssi\":%d,\"secure\":%s}",
            info.ssid,
            info.rssi,
            info.secure ? "true" : "false"
        );
        httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    } else {
        httpd_resp_send(req,
            "{\"connected\":false}",
            HTTPD_RESP_USE_STRLEN
        );
    }

    return ESP_OK;
}

/* =========================================================
 * URI TABLE
 * ========================================================= */
static httpd_uri_t uri_root = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = root_get_handler
};

static httpd_uri_t uri_scan_wifi = {
    .uri      = "/scan_wifi",
    .method   = HTTP_GET,
    .handler  = scan_wifi_get_handler
};

static httpd_uri_t uri_scan_result = {
    .uri      = "/scan_result",
    .method   = HTTP_GET,
    .handler  = scan_result_get_handler
};

static httpd_uri_t uri_connect_wifi = {
    .uri      = "/connect_wifi",
    .method   = HTTP_POST,
    .handler  = connect_wifi_post_handler
};

static httpd_uri_t uri_wifi_status = {
    .uri      = "/wifi_status",
    .method   = HTTP_GET,
    .handler  = wifi_status_get_handler
};

static httpd_uri_t uri_disconnect_wifi = {
    .uri      = "/disconnect_wifi",
    .method   = HTTP_POST,
    .handler  = disconnect_wifi_post_handler
};

static httpd_uri_t uri_forget_wifi = {
    .uri      = "/forget_wifi",
    .method   = HTTP_POST,
    .handler  = forget_wifi_post_handler
};

static httpd_uri_t uri_exit_ap = {
    .uri      = "/exit_ap",
    .method   = HTTP_POST,
    .handler  = exit_ap_post_handler
};

static httpd_uri_t uri_wifi_current = {
    .uri    = "/wifi_current",
    .method = HTTP_GET,
    .handler= wifi_current_get_handler
};

/* =========================================================
 * START / STOP SERVER
 * ========================================================= */
httpd_handle_t wifi_start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &uri_root);
        httpd_register_uri_handler(server, &uri_wifi_current);
        httpd_register_uri_handler(server, &uri_wifi_status);
        httpd_register_uri_handler(server, &uri_connect_wifi);
        httpd_register_uri_handler(server, &uri_disconnect_wifi);
        httpd_register_uri_handler(server, &uri_forget_wifi);
        httpd_register_uri_handler(server, &uri_scan_wifi);
        httpd_register_uri_handler(server, &uri_scan_result);
        httpd_register_uri_handler(server, &uri_exit_ap);

        /* CUỐI CÙNG */



        ESP_LOGI(TAG, "HTTP server started");
    }

    return server;
}

void wifi_stop_webserver(void)
{
    if (server) {
        httpd_stop(server);
        server = NULL;
        ESP_LOGI(TAG, "HTTP server stopped");
    }
}
