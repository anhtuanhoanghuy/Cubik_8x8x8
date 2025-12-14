#include <stdio.h>
#include "Wifi.h"
const char ssid[32] = "HELLO_CUBIK";
const char password[32] = "12345678";
void app_main(void)
{
    wifi_init();
    wifi_enter_stationAP_mode(ssid, password);
}
