#if !defined (__MONITORING_H__)
#define __MONITORING_H__

#include "MQTT.h"
#include "freertos/timers.h"

typedef enum {
    MONITORING,
    ACK,
} type_t;

int send_mqtt(type_t type, const char *data, uint16_t len);

#endif