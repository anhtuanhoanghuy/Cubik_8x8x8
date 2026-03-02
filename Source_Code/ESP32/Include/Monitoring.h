#if !defined (__MONITORING_H__)
#define __MONITORING_H__

#include "MQTT.h"
#include "freertos/timers.h"

int mqtt_monitoring(const char *data, uint16_t len);
void publish_monitoring_callback(TimerHandle_t);

#endif