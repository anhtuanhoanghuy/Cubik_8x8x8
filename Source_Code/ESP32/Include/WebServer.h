#if !defined(__WEBSERVER_H__)
#define __WEBSERVER_H__

#include "esp_err.h"
#include "esp_http_server.h"

#include "esp_wifi.h"

httpd_handle_t wifi_start_webserver(void);

/* Function for stopping the webserver */
void wifi_stop_webserver(void);

#endif // __WEBSERVER_H__
