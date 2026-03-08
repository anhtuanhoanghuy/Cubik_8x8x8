#include "Monitoring.h"
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "Utils.h"
#include "Defines.h"

extern TimerHandle_t periodic_timer;
extern System_Variable system_Variable;

int mqtt_monitoring(const char *data, uint16_t len) {
  if (getMQTTClient() == NULL) {
    ESP_LOGE("MQTT", "MQTT client not initialized");
    return -1;
  }
   int msg_id = esp_mqtt_client_publish(
        getMQTTClient(),
        topic_monitoring,
        data,
        len,
        1,                    // QoS
        1                     // retain
    );
  if (msg_id < 0) {
    ESP_LOGE("MQTT", "Failed to publish");
  } else {
    ESP_LOGI("MQTT", "Published to %s, msg_id=%d", topic_monitoring, msg_id);
  }
  
  return msg_id;
}

void publish_monitoring_callback(TimerHandle_t xTimer)
{
  mqtt_monitoring((char*)encode_monitoring(&system_Variable),sizeof(System_Variable) + 3);
  if (xTimer != periodic_timer) {
    xTimerReset(periodic_timer,0);
  }
}
