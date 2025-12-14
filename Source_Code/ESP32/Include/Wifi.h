#if !defined(__WIFI_H__)
#define __WIFI_H__

void wifi_reset(void);

void wifi_init(void);

void wifi_enter_station_mode(const char *, const char *);

void wifi_enter_stationAP_mode(const char *, const char *);

void wifi_scan(void);

void wifi_connect(const char *, const char *);

void wifi_exit_StationAP_mode(void);

void wifi_off(void);
 
void wifi_on(void);

void test_lib(void);

#endif // _WIFICONNECT_H__
