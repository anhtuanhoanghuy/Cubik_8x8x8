#ifndef __LED_MODE_LIB_H__
#define __LED_MODE_LIB_H__

#include <stdint.h>

// LED_MODE_TEST
uint32_t frame1[TOTAL_LED_NUMBERS] = {rgb(0,0,0), rgb(0,0,0),rgb(0,0,0)};
uint32_t frame2[TOTAL_LED_NUMBERS] = {rgb(128,0,0), rgb(0,0,0),rgb(0,0,0)};
uint32_t frame3[TOTAL_LED_NUMBERS] = {rgb(0,0,0), rgb(0,128,0),rgb(0,0,0)};
uint32_t frame4[TOTAL_LED_NUMBERS] = {rgb(0,0,0), rgb(0,0,0),rgb(0,0,128)};
uint32_t frame5[TOTAL_LED_NUMBERS] = {rgb(0,0,0), rgb(0,0,0),rgb(0,0,0)};
uint32_t frame6[TOTAL_LED_NUMBERS] = {rgb(128,0,0), rgb(0,0,0),rgb(0,0,0)};
uint32_t frame7[TOTAL_LED_NUMBERS] = {rgb(128,0,0), rgb(0,128,0),rgb(0,0,0)};
uint32_t frame8[TOTAL_LED_NUMBERS] = {rgb(128,0,0), rgb(0,128,0),rgb(0,0,128)};
uint32_t frame9[TOTAL_LED_NUMBERS] = {rgb(0,0,0), rgb(0,0,0),rgb(0,0,0)};
uint32_t frame10[TOTAL_LED_NUMBERS] = {rgb(128,0,0), rgb(0,128,0),rgb(0,0,128)};
uint32_t frame11[TOTAL_LED_NUMBERS] = {rgb(0,0,0), rgb(0,0,0),rgb(0,0,0)};
uint32_t frame12[TOTAL_LED_NUMBERS] = {rgb(128,0,0), rgb(0,128,0),rgb(0,0,128)};
uint32_t frame13[TOTAL_LED_NUMBERS] = {rgb(0,0,0), rgb(0,0,0),rgb(0,0,0)};
uint32_t frame14[TOTAL_LED_NUMBERS] = {rgb(128,0,0), rgb(0,128,0),rgb(0,0,128)};


uint32_t* led_frames[14] = {
    frame1,
    frame2,
    frame3,
    frame4,
    frame5,
    frame6,
    frame7,
    frame8,
    frame9,
    frame10,
    frame11,
    frame12,
    frame13,
    frame14
};

#endif
