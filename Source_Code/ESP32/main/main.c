#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "app.h"

#include "Wifi.h"
#include "Bluetooth.h"
#include "MQTT.h"
#include "Utils.h"
#include "Monitoring.h"
#include "uart.h"
#include "control.h"

System_Variable system_Variable = {0};

void wifi_task(void *argument) {
    wifi_task_handler();
}

void control_task(void *argument) {
    control_task_handler();
}

void uart_task(void *argument) {
    uart_task_handler();
}

void app_main(void)
{
    //Init NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || 
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI("NVS", "✅ NVS initialized");
    bluetooth_init();
    wifi_init();
    wifi_store_init();
    mqtt_app_start();
    uart_init();
    
    // Create Queue
    received_commandHandle = xQueueCreate(10, sizeof(command_packet_t));
    transmit_Handle = xQueueCreate(10, sizeof(uart_tx_packet_t));
    
    // Create task
    xTaskCreate(
        uart_task,
        "console",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        PRIORITY_MEDIUM,
        NULL
    );

    xTaskCreate(
        uart_task,
        "uart_task",
        configMINIMAL_STACK_SIZE * 2,
        NULL,
        PRIORITY_HIGH,
        NULL
    );

    xTaskCreate(
        control_task,
        "control_task",
        configMINIMAL_STACK_SIZE * 4,
        NULL,
        PRIORITY_LOW,
        NULL
    );

}

