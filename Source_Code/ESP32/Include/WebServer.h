#if !defined(__WEBSERVER_H__)
#define __WEBSERVER_H__

#include "esp_err.h"
#include "esp_http_server.h"

esp_err_t root_get_handler(httpd_req_t *);

httpd_handle_t wifi_start_webserver(void);

void wifi_stop_webserver(httpd_handle_t);

void test_lib(void);

#endif // __WEBSERVER_H__
