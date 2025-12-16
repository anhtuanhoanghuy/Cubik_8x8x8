#if !defined(__WIFI_H__)
#define __WIFI_H__

#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define WIFI_STA_GOT_IP_BIT  (1<<1) //bit1
#define WIFI_STA_CONNECTED_BIT  (1<<2) //bit2
#define WIFI_STA_DISCONNECTED_BIT (1<<3) //bit3

#define MAX_AP_NUM 30

typedef enum {
    WIFI_SCAN_IDLE = 0,
    WIFI_SCAN_RUNNING,
    WIFI_SCAN_DONE
} wifi_scan_state_t;

EventGroupHandle_t wifi_get_event_group(void);

void wifi_reset(void);

void wifi_init(void);

void wifi_enter_station_mode(const char *, const char *);

void wifi_enter_stationAP_mode(const char *, const char *);

void wifi_scan_start(void);

void wifi_scan_stop(void);

void wifi_connect(const char *, const char *);

void wifi_exit_StationAP_mode(void);

void wifi_off(void);
 
void wifi_on(void);

void wifi_get_record_count(uint16_t *);

uint16_t wifi_get_record_list(wifi_ap_record_t *, uint16_t);

void wifi_get_one_record_list(wifi_ap_record_t *);

void wifi_clear_all_list(void);

uint8_t wifi_get_scan_state(void);



#endif // __WIFICONNECT_H__
