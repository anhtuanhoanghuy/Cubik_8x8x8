#ifndef __MENU_CONTROL_H__
#define __MENU_CONTROL_H__

#include <stdint.h>
#include <stdbool.h>
#include "SH1106.h"

#define ARROW_X (SH1106_WIDTH - 8 - 2)  // = 118
#define TEXT_X 0
#define TEXT_MAX_WIDTH (ARROW_X - 2)  // chừa khoảng cách

#define LINE_HEIGHT 16
#define VISIBLE_LINES (SH1106_HEIGHT/LINE_HEIGHT)
#define TEXT_PADDING_Y 4

typedef void (*menu_action_t)(void);

typedef struct {
    const char *label;
    menu_action_t action;
} menu_item_t;


typedef struct {
    uint8_t selected_option;  // item đang chọn
    uint8_t old_selected_option;
    bool selected_active;
    uint8_t top;       // item đầu hiển thị (scroll)
} menu_state_t;

typedef enum {
    HOME_PAGE = 0,
    SETTING_PAGE
} page_t;

typedef enum {
    CMD_LCD_DUMMY = 0x00,
    CMD_LCD_RENDER_FULL_PAGE,
    CMD_LCD_RENDER_UPDATE_FRAME,
    CMD_LCD_RENDER_ACTIVE_SETTING,
    CMD_LCD_RENDER_DEACTIVE_SETTING
} cmd_lcd_t;

extern menu_item_t menu_items[];

extern const uint8_t MENU_COUNT;

extern menu_state_t menu;
extern page_t current_page;

void action_mode(void);

void action_brightness(void);

void action_speed(void);

void action_notify(void);

void action_sleep(void);

void action_display_off(void);

void action_AI(void);

void action_volume(void);

void action_wifi(void);

void action_check_update(void);

void action_factory_reset(void);

void menu_render(void);

void render_home_page(void);

void render_setting_page(void);

void active_setting_render(void);

void deactive_setting_render(void);

#endif
