#include "PL9823.h"

TaskHandle_t led_task_t;

PL9823_config_t pl9823;

void PL9823_show(uint32_t* leds) {
    static uint16_t pwm_buffer[TOTAL_LED_NUMBERS * LED_BITS + RESET_SLOTS] = {0};
    for (uint16_t led = 0; led < TOTAL_LED_NUMBERS; led++)
    {
        uint8_t r, g, b;
        if (pl9823.status == RESET) {
            r = 0;
            g = 0;
            b = 0;
        } else {
            r = ((leds[led] >> 16) & 0xFF) * pl9823.brightness / 100;
            g = ((leds[led] >> 8) & 0xFF) * pl9823.brightness / 100;
            b = (leds[led] & 0xFF) * pl9823.brightness / 100;
        }

        uint32_t color = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;

        uint16_t index = led * LED_BITS;

        for(uint8_t bit = 0; bit < 24; bit++)
        {
            pwm_buffer[index + bit] = (color & (1UL << (23 - bit))) ? HIGH_DUTY : LOW_DUTY;
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

void PL9823_set_mode(PL9823_led_mode_t mode) {
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

