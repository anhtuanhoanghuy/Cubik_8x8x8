#include "PL9823.h"

PL9823_config_t pl9823 = {
    .speed = 100,
    .brightness = BRIGHTNESS_DEFAULT,
};

void PL9823_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = led_pl9823_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(led_pl9823_GPIO_Port, &GPIO_InitStruct);
}

void PL9823_show(uint32_t* leds) {
    static uint16_t pwm_buffer[TOTAL_LED_NUMBERS * LED_BITS + RESET_SLOTS] = {0};
    for (uint16_t led = 0; led < TOTAL_LED_NUMBERS; led++) {
        for(uint8_t led_bit = 0; led_bit < LED_BITS; led_bit++) {
            if((leds[led])& (1 << (23 - led_bit))) {
                pwm_buffer[led*LED_BITS + led_bit] = HIGH_DUTY;
            } else {
                pwm_buffer[led*LED_BITS + led_bit] = LOW_DUTY;
            }
        }

    }
    for (uint16_t i = TOTAL_LED_NUMBERS * LED_BITS; i < TOTAL_LED_NUMBERS * LED_BITS + RESET_SLOTS; i++) {
        pwm_buffer[i] = 0;
    }
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_3, (uint32_t*)pwm_buffer,TOTAL_LED_NUMBERS * LED_BITS + RESET_SLOTS);
}

void PL9823_set_status(uint8_t status) {
    pl9823.status = status;
}

uint8_t PL9823_get_status(void) {
    return pl9823.status;
}

void PL9823_set_mode(uint8_t mode) {
    pl9823.mode = mode;
}

uint8_t PL9823_get_mode(void) {
    return pl9823.mode;
}

void PL9823_set_speed(uint8_t speed_percent) {
    pl9823.speed = speed_percent;
}

uint8_t PL9823_get_speed(void) {
    return pl9823.speed;
}


void PL9823_set_brightness(uint8_t brightness_percent) {
    pl9823.brightness = brightness_percent;
}

uint8_t PL9823_get_brightness(void) {
    return pl9823.brightness;
}