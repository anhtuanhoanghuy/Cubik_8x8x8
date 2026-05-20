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
    uint8_t auto_sleep;
    uint8_t ai_realtime;
    uint8_t volumn;
} global_data_t;

extern global_data_t global_data;

#endif