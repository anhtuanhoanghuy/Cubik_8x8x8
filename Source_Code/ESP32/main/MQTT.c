#include "MQTT.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"
#include "Defines.h"
#include "Utils.h"

static const char *TAG = "mqtts_example";
static esp_mqtt_client_handle_t client = NULL;
extern QueueHandle_t wifi_ble_rx_queue; 

esp_mqtt_client_handle_t getMQTTClient(void) {
    return client;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32, base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_publish(client, topic_status, "1", 0, 1, 1);
        esp_mqtt_client_subscribe_single(client, topic_command, 1);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        // ---------------------------------------------------------
        printf("DATA: ");
        for (int i = 0; i < event->data_len; i++) {
            printf("%02X ", event->data[i]);
        }
        printf("\n");
        // ---------------------------------------------------------
        wifi_ble_command_t cmd = {0};
        if (decode_data(&cmd, (const uint8_t*) event->data, event->data_len) == DATA_VALID){
            // Đẩy sang task xử lý
            xQueueSendFromISR(wifi_ble_rx_queue, &cmd, NULL);
            ESP_LOGI(TAG, "BLE receive:%d bytes", event->data_len);  
        }
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGI(TAG, "Last captured errno : %d (%s)",  event->error_handle->esp_transport_sock_errno,
                     strerror(event->error_handle->esp_transport_sock_errno));
        } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
            ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        } else {
            ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
        break;

    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_app_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address.uri = CONFIG_BROKER_URI,
            .verification.crt_bundle_attach = esp_crt_bundle_attach
        },
        .credentials = {
            .username = user_name,
            .authentication.password = pass_word
        },
        .session = {
            .keepalive = 30,
            .last_will = {
                .topic = topic_status,
                .msg =  "0",
                .qos = 1,
                .retain = true
                
            }
        },
        .network = {
            .reconnect_timeout_ms = 3000,
            .timeout_ms = 10000,
            .refresh_connection_after_ms = 0,
            .disable_auto_reconnect = false
        }
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
 
}