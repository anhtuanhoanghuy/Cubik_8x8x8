#if !defined(__WIFI_H__)
#define __WIFI_H__

#include "freertos/event_groups.h"

extern EventGroupHandle_t wifi_event_group;

#define WIFI_SCAN_DONE_BIT   (1<<0) //bit0
#define WIFI_STA_GOT_IP_BIT  (1<<1) //bit1
#define WIFI_STA_CONNECTED_BIT  (1<<2) //bit2
#define WIFI_STA_DISCONNECTED_BIT (1<<3) //bit3

void wifi_reset(void);

void wifi_init(void);

void wifi_enter_station_mode(const char *, const char *);

void wifi_enter_stationAP_mode(const char *, const char *);

void wifi_scan(void);

void wifi_connect(const char *, const char *);

void wifi_exit_StationAP_mode(void);

void wifi_off(void);
 
void wifi_on(void);

void wifi_get_record_count(uint16_t *);

void wifi_get_record_list(uint16_t *, wifi_ap_record_t *);

void wifi_get_one_record_list(wifi_ap_record_t *);

void wifi_clear_all_list(void);

#endif // __WIFICONNECT_H__
