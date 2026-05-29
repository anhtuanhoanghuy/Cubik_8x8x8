#include "data.h"

uint8_t SysData_get_notification_setting(void)
{
    return global_system_data.notification;
}

void SysData_set_notification_setting(uint8_t value)
{
    global_system_data.notification = value;
}

uint8_t SysData_get_auto_sleep_setting(void)
{
    return global_system_data.auto_sleep;
}

void SysData_set_auto_sleep_setting(uint8_t value)
{
    global_system_data.auto_sleep = value;
}

uint8_t SysData_get_disp_auto_off_setting(void)
{
    return global_system_data.disp_auto_off;
}

void SysData_set_disp_auto_off_setting(uint8_t value)
{
    global_system_data.disp_auto_off = value;
}

uint8_t SysData_get_AI_realtime_setting(void)
{
    return global_system_data.ai_realtime;
}

void SysData_set_AI_realtime_setting(uint8_t value)
{
    global_system_data.ai_realtime = value;
}

uint8_t SysData_get_volume_setting(void)
{
    return global_system_data.volume;
}

void SysData_set_volume_setting(uint8_t value)
{
    global_system_data.volume = value;
}

uint8_t SysData_get_wifi_setting(void)
{
    return global_system_data.wifi_status;
}

void SysData_set_wifi_setting(uint8_t value)
{
    global_system_data.wifi_status = value;
}