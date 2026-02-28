#include "Monitoring.h"
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
// struct Monitoring {
//   int isOnline;
//   int isConnectedBLE;
//   String device_name;
//   String device_tag;
//   int wifiConnected;
//   String wifiInfo;
//   int LED;
//   int voiceMessage;
//   int autoOff;
//   TimeOfDay sleepStartTime = TimeOfDay(hour: 22, minute: 00);
//   TimeOfDay sleepEndTime = TimeOfDay(hour: 6, minute: 00);
//   int ledMode = LedMode.RAINBOW.value;
//   int brightness = 50;
//   int speed = 50;
// }

int mqtt_monitoring(const char *data) {
    uint8_t data1[] = {0xAA, 0x07, 0x01, 0x3A, 0x01, 0x2c, 0x02, 0x3A, 0x00,0X83};
  if (getMQTTClient() == NULL) {
    ESP_LOGE("MQTT", "MQTT client not initialized");
    return -1;
  }
   int msg_id = esp_mqtt_client_publish(
        getMQTTClient(),
        topic_monitoring,
        (const char *)data1,   // cast sang char*
        sizeof(data1),         // length chính xác
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
