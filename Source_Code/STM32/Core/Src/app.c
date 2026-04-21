#include "app.h"
#include "main.h"
#include "FreeRTOS.h"
#include "defines.h"
#include "task.h"
#include "gpio.h"
#include "tim.h"
#include "DHT22.h"
#include "PL9823.h"
#include "LED_mode.h"

DHT22_t dht22 = {0};
void sensor_task_handler(void) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,1);
    TickType_t xSensorTime = xTaskGetTickCount();
    while (1) {
    
    if (DHT22_ReadValue(&dht22) == DHT22_OK) {
        ;
    }
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    vTaskDelayUntil(&xSensorTime, pdMS_TO_TICKS(5000));
    }
}

void led_task_handler(void) {
    TickType_t xFrameInterval_ms = xTaskGetTickCount();
    while(1) {
        for(int frame = 0; frame < 14; frame++) {
            if (PL9823_get_speed() == 0) {
                vTaskDelay(pdMS_TO_TICKS(20));
                continue;
            }
            PL9823_show(led_frames[frame]);
            vTaskDelayUntil(&xFrameInterval_ms, pdMS_TO_TICKS(SPEED_TO_DELAY(PL9823_get_speed())));
        }
    }
    
}

void uart_task_handler(void) {
    while(1) {

    }
}
