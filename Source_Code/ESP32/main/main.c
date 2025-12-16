#include <stdio.h>
#include "Wifi.h"
#include "WebServer.h"

#define MAX_AP_NUM 30
const char ssid[32] = "HELLO_CUBIK";
const char password[32] = "12345678";

void app_main(void)
{
    wifi_init();
    wifi_enter_stationAP_mode(ssid, password);
    wifi_start_webserver();
}

