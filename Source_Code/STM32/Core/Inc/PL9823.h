#ifndef __PL9823_H__
#define __PL9823_H__

#include <stdint.h>
#include "tim.h"
#include "FreeRTOS.h"
#include "task.h"

#define led_pl9823_Pin          GPIO_PIN_2
#define led_pl9823_GPIO_Port    GPIOA

#define TOTAL_LED_NUMBERS   3
#define LED_BITS            24
#define HIGH_DUTY           63
#define LOW_DUTY            27
#define RESET_SLOTS         50
#define SPEED_DEFAULT       50 
#define SPEED_MIN_DELAY_MS   2000 //slowest time betweens two frames
#define SPEED_MAX_DELAY_MS   100  //fastest time betweens two frames
#define SPEED_RANGE_MS       (SPEED_MIN_DELAY_MS - SPEED_MAX_DELAY_MS)
#define BRIGHTNESS_DEFAULT  50
#define rgb(r, g, b)   ( ((uint32_t)(g) << 16) | ((uint32_t)(r) << 8) | (b) )
#define SPEED_TO_DELAY(speed) \
    ((SPEED_MIN_DELAY_MS - ((uint32_t)(speed) * SPEED_RANGE_MS) / 100))

extern TaskHandle_t led_task_t;

typedef enum {
    NORMAL_MODE = 0,
    RAINBOW_MODE,
    CUSTOM_MODE
} PL9823_led_mode_t;

typedef struct {
    uint8_t status;
    uint8_t mode;
    uint8_t speed;
    uint8_t brightness;
} PL9823_config_t;


void PL9823_show(uint32_t*);


void PL9823_set_status(uint8_t);

uint8_t PL9823_get_status(void);

void PL9823_set_mode(PL9823_led_mode_t);

uint8_t PL9823_get_mode(void);

void PL9823_set_speed(uint8_t);

uint8_t PL9823_get_speed(void);

void PL9823_set_brightness(uint8_t);

uint8_t PL9823_get_brightness(void);

#endif
