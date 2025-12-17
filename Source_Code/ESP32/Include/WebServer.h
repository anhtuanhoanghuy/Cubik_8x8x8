#if !defined(__WEBSERVER_H__)
#define __WEBSERVER_H__

#include "esp_err.h"
#include "esp_http_server.h"
#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "Wifi.h"
#include "WebServer.h"

httpd_handle_t wifi_start_webserver(void);

/* Function for stopping the webserver */
void wifi_stop_webserver(httpd_handle_t server);

#endif // __WEBSERVER_H__
