#if !defined (__MQTT_H__)
#define __MQTT_H__

#include "mqtt_client.h"

#define CONFIG_BROKER_URI "mqtts://35a196d8b54146f08f917c8c382e1c0a.s1.eu.hivemq.cloud:8883"
#define user_name "HELLO_CUBIK_8X8X8"
#define pass_word "hello_cubik_8X8X8"
#define topic_command "Cubik/30102002/Command"
#define topic_monitoring "Cubik/30102002/Monitoring"
#define topic_status "Cubik/30102002/Status"

esp_mqtt_client_handle_t getMQTTClient(void);
void mqtt_app_start(void);

#endif