#ifndef __DATA_H__
#define __DATA_H__

#include <stdint.h>
#include "PL9823.h"
#include "DHT22.h"
#include "SH1106.h"

typedef struct {
    PL9823_config_t LED;
    DHT22_t DHT22;
    SH1106_config_t LCD;
    uint8_t wifi_status;
    uint8_t notification;
    uint8_t disp_auto_off;
    uint8_t auto_sleep;
    uint8_t ai_realtime;
    uint8_t volume;
} global_system_data_t;

extern global_system_data_t global_system_data;

uint8_t SysData_get_notification_setting(void);
void SysData_set_notification_setting(uint8_t value);

uint8_t SysData_get_auto_sleep_setting(void);
void SysData_set_auto_sleep_setting(uint8_t value);

uint8_t SysData_get_disp_auto_off_setting(void);
void SysData_set_disp_auto_off_setting(uint8_t value);

uint8_t SysData_get_AI_realtime_setting(void);
void SysData_set_AI_realtime_setting(uint8_t value);

uint8_t SysData_get_volume_setting(void);
void SysData_set_volume_setting(uint8_t value);

uint8_t SysData_get_wifi_setting(void);
void SysData_set_wifi_setting(uint8_t value);

#endif