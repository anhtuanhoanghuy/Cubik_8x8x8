#if !defined(__WIFI_H__)
#define __WIFI_H__

#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define WIFI_STA_GOT_IP_BIT  (1<<1) //bit1
#define WIFI_STA_CONNECTED_BIT  (1<<2) //bit2
#define WIFI_STA_DISCONNECTED_BIT (1<<3) //bit3
#define WIFI_STA_USER_DISCONNECT_BIT (1<<4) //bit4
#define WIFI_STA_FAIL_PASSWORD_BIT (1<<5) //bit5


#define MAX_AP_NUM 30
#define MAX_WIFI_SAVED 10
#define WIFI_SSID_MAX  32
#define WIFI_PASS_MAX  64

typedef enum {
    WIFI_SCAN_IDLE = 0,
    WIFI_SCAN_RUNNING,
    WIFI_SCAN_DONE
} wifi_scan_state_t;


typedef struct {
    char ssid[WIFI_SSID_MAX];
    char password[WIFI_PASS_MAX];
    int8_t last_rssi;
    uint32_t last_used;   // unix time hoặc counter
} saved_wifi_t;

typedef struct {
    bool connected;
    char ssid[WIFI_SSID_MAX];
    int8_t rssi;
} wifi_status_info_t;

bool wifi_get_current_status(wifi_status_info_t *info);

void wifi_init(void);

void wifi_enter_station_mode(const char *, const char *);

void wifi_enter_stationAP_mode(const char *, const char *);

void wifi_scan_start(void);

uint16_t wifi_get_record_count(void);

wifi_ap_record_t *wifi_get_record_list(void);

uint8_t wifi_get_scan_state(void);

void wifi_connect(const char *, const char *);

void wifi_connect_saved(const saved_wifi_t *);

void wifi_disconnect(void);

void wifi_forget(void);

void wifi_off(void);

void wifi_store_load(void);

void wifi_store_save(void);

void wifi_store_add_or_update(const char *, const char *, int8_t );

bool wifi_select_best(saved_wifi_t *);

EventGroupHandle_t wifi_get_event_group(void);

#endif // __WIFICONNECT_H__
