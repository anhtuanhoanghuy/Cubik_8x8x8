#include "input.h"
#include <task.h>
#include <stdint.h>

QueueHandle_t input_Handle = NULL;
TimerHandle_t go_to_home_timer = NULL;

volatile uint32_t last_btn_press_time = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    button_process(GPIO_Pin);
}

void button_process(uint16_t GPIO_Pin)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t now = xTaskGetTickCountFromISR();

    if ((now - last_btn_press_time) < pdMS_TO_TICKS(200))
        return;

    last_btn_press_time = now;

    input_event_t event;

    if (GPIO_Pin == back_btn_Pin) {
        event = EVT_BTN_BACK;
    } 
    else if (GPIO_Pin == confirm_btn_Pin) {
        event = EVT_BTN_CONFIRM;
    } 
    else if (GPIO_Pin == led_btn_Pin) {
        event = EVT_BTN_LED;
    }
    else return;

    xQueueSendFromISR(input_Handle, &event, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}