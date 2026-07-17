#include "Monitoring.h"
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "Utils.h"
#include "Defines.h"

int send_mqtt(type_t type, const char *data, uint16_t len) {
  if (getMQTTClient() == NULL) {
    ESP_LOGE("MQTT", "MQTT client not initialized");
    return -1;
  }
   int msg_id = esp_mqtt_client_publish(
        getMQTTClient(),
        type == MONITORING ? 
        topic_monitoring : topic_command,
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