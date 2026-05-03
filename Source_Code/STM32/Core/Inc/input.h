#ifndef __INPUT_H__
#define __INPUT_H__

#include "main.h"

#define btn_Port                            GPIOB
#define back_btn_Pin                        GPIO_PIN_0
#define back_btn_GPIO_Port                  GPIOB
#define back_btn_EXTI_IRQn                  EXTI0_IRQn
#define confirm_btn_Pin                     GPIO_PIN_1
#define confirm_btn_GPIO_Port               GPIOB
#define confirm_btn_EXTI_IRQn               EXTI1_IRQn
#define led_btn_Pin                         GPIO_PIN_3
#define led_btn_GPIO_Port                   GPIOB
#define led_btn_EXTI_IRQn                   EXTI3_IRQn
#define encoder_A_Pin                       GPIO_PIN_6
#define encoder_A_GPIO_Port                 GPIOA
#define encoder_B_Pin                       GPIO_PIN_7
#define encoder_B_GPIO_Port                 GPIOA

void button_process(uint16_t GPIO_Pin);

#endif

