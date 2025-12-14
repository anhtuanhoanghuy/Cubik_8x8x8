#include <stdio.h>
#include "Wifi.h"
#include "WebServer.h"

#define MAX_AP_NUM 30
const char ssid[32] = "HELLO_CUBIK";
const char password[32] = "12345678";
uint16_t ap_count = 0;
uint16_t max_ap_number = MAX_AP_NUM;
wifi_ap_record_t ap_records[MAX_AP_NUM];
EventGroupHandle_t wifi_event_group;
void app_main(void)
{
    wifi_init();
    wifi_enter_stationAP_mode(ssid, password);
    wifi_start_webserver();
    wifi_scan_start();
    while (1) {
        EventBits_t bits = xEventGroupWaitBits(
            wifi_event_group,
            WIFI_SCAN_DONE_BIT |
            WIFI_STA_CONNECTED_BIT |
            WIFI_STA_GOT_IP_BIT |
            WIFI_STA_DISCONNECTED_BIT,
            pdTRUE,    // clear bit sau khi nhận
            pdFALSE,   // OR logic
            portMAX_DELAY
        );
    
        if (bits & WIFI_SCAN_DONE_BIT) {
            wifi_get_record_count(&ap_count);
            if(ap_count > 0) {
                wifi_get_record_list(&max_ap_number, ap_records);
                for (uint16_t i = 0; i < ap_count; i++) {
                    printf("SSID:%s, RSSI: %d",(char *)ap_records[i].ssid, ap_records[i].rssi);
                }
            } else {
                printf("No wifi found.\n");
            }
        }
        
        if (bits & WIFI_STA_CONNECTED_BIT) {
            println("WiFi connected (no IP yet)");
        }
        if (bits & WIFI_STA_GOT_IP_BIT) {
            println("Wifi Connected and got IP.\n");
        }
        if (bits & WIFI_STA_DISCONNECTED_BIT) {
            println("Wifi Disconnected.\n");
        }
    }
}
