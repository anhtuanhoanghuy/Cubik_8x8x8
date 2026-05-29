/*
 * bitmap.h
 *
 *  Created on: 17-Jun-2019
 *      Author: poe
 */

#ifndef __BITMAP_H__
#define __BITMAP_H__

#define WIFI_ICON_POSITION            0,0
#define BLUETOOTH_ICON_POSITION       17,0
#define BELL_ICON_POSITION            110,0
#define TEMPERATURE_ICON_POSITION_X   13
#define TEMPERATURE_ICON_POSITION_Y   50
#define HUMIDITY_ICON_POSITION_X      78
#define HUMIDITY_ICON_POSITION_Y      50
#define TEMPERATURE_VALUE_POSITION_X  (TEMPERATURE_ICON_POSITION_X + 15)
#define TEMPERATURE_VALUE_POSITION_Y  (TEMPERATURE_ICON_POSITION_Y + 3)
#define HUMIDITY_VALUE_POSITION_X     (HUMIDITY_ICON_POSITION_X + 20)
#define HUMIDITY_VALUE_POSITION_Y     (HUMIDITY_ICON_POSITION_Y + 3)
#define VALUE_W                       30
#define VALUE_H                       10

#define SETTING_VALUE_POSITION_X      13
#define SETTING_VALUE_POSITION_Y      30

#define SETTING_TOGGLE_POSITION_X     105
// #define SETTING_TOGGLE_POSITION_Y     30

#define NOTIFY_DIALOG_POSITION_X    
#define NOTIFY_DIALOG_POSITION_Y    30

// bell_icon_16x16.png | 16x16 | 1-bit vertical bytes (LSB=top)
extern const unsigned char bell_icon_16x16[];

// bell_off_icon_16x16.png | 16x16 | 1-bit vertical bytes (LSB=top)
extern const unsigned char bell_off_icon_16x16[];

// bluetooth_icon_16x16.png | 16x16 | 1-bit vertical bytes (LSB=top)
extern const unsigned char bluetooth_icon_16x16[];

// bluetooth_off_icon_16x16.png | 16x16 | 1-bit vertical bytes (LSB=top)
extern const unsigned char bluetooth_off_icon_16x16[];

// humidity_icon_16x16.png | 16x16 | 1-bit vertical bytes (LSB=top)
extern const unsigned char humidity_icon_16x16[];

// temperature_icon_16x16.png | 16x16 | 1-bit vertical bytes (LSB=top)
extern const unsigned char temperature_icon_16x16[];

// wifi_icon_16x16.png | 16x16 | 1-bit vertical bytes (LSB=top)
extern const unsigned char wifi_icon_16x16[];

// wifi_off_icon_16x16.png | 16x16 | 1-bit vertical bytes (LSB=top)
extern const unsigned char wifi_off_icon_16x16[];

extern const unsigned char arrow[];

// icons8-toggle-off-16.png | 16x16 | 1-bit vertical bytes (LSB=top)
extern const unsigned char icons8_toggle_off_16[];

// icons8-toggle-on-16.png | 16x16 | 1-bit vertical bytes (LSB=top)
extern const unsigned char icons8_toggle_on_16[];

#endif /* BITMAP_H_ */
