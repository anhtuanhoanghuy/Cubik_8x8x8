#include "input.h"
#include <FreeRTOS.h>
#include <task.h>
#include <stdint.h>

int test = 50;
volatile uint32_t last_btn_press_time = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    button_process(GPIO_Pin);
}

void button_process(uint16_t GPIO_Pin) {
    uint32_t now = xTaskGetTickCountFromISR();

    if ((now - last_btn_press_time) < pdMS_TO_TICKS(500))
        return; // bỏ qua nhiễu/bounce

    last_btn_press_time = now;

    if (GPIO_Pin == back_btn_Pin) {
        test--;
        // back_btn_press();
    } else if (GPIO_Pin == confirm_btn_Pin) {
        test++;
        // confrm_btn_press();
    } else if (GPIO_Pin == led_btn_Pin) {
        test+=10;
        // led_btn_press();
    }
}
