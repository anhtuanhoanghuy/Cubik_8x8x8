#include "menu_control.h"
#include "PL9823.h"
#include "bitmap.h"
#include "command.h"
#include <usart.h>
#include "data.h"
#include "monitoring.h"

menu_item_t menu_items[] = {
    {
        .label = "1.LED Mode",
        .value_range = {
            .min_value = 0,
            .max_value = 10,
            .step = 1,
        },
        .get_value = &PL9823_get_mode,
        .action = &action_mode
    },
    {
        .label = "2.Brightness",
        .value_range = {
            .min_value = 0,
            .max_value = 100,
            .step = 10,
        },
        .get_value = &PL9823_get_brightness,
        .action = &action_brightness
    },
    {
        .label = "3.LED Speed",
         .value_range = {
            .min_value = 0,
            .max_value = 100,
            .step = 10,
        },
        .get_value = &PL9823_get_speed,
        .action = &action_speed
    },
    // {
    //     .label = "4.Notification",
    //     .get_value = NULL,
    //     .action = &action_notify
    // },
    // {
    //     .label = "5.Auto sleep",
    //     .get_value = NULL,
    //     .action = &action_sleep
    // },
    // {
    //     .label = "6.DispAutoOff",
    //     .get_value = NULL,
    //     .action = &action_display_off
    // },
    // {
    //     .label = "7.AI Realtime",
    //     .get_value = NULL,
    //     .action = &action_AI
    // },
    // {
    //     .label = "8.Volume",
    //     .get_value = NULL,
    //     .action = &action_volume
    // },
    // {
    //     .label = "9.Wifi",
    //     .get_value = NULL,
    //     .action = &action_wifi
    // },
    // {
    //     .label = "10.Check Update",
    //     .get_value = NULL,
    //     .action = &action_check_update
    // },
    // {
    //     .label = "11.Reset Device",
    //     .get_value = NULL,
    //     .action = &action_factory_reset
    // }
};

static menu_state_t menu = {
    .selected_option = 0,
    .old_selected_option = 0,    
    .selected_active = false,
    .top = 0
};

static page_t current_page = HOME_PAGE;
static uint8_t temp_value = 0;

const uint8_t MENU_COUNT = sizeof(menu_items) / sizeof(menu_items[0]);

page_t get_current_page(void) {
    return current_page;
}

void set_current_page(page_t page) {
    current_page = page;
} 

menu_state_t* get_menu(void) {
    return &menu;
}

uint8_t get_menu_count(void) {
    return MENU_COUNT;
}

void action_mode(void) {
     /* xử lý Chế độ */ 
    command_packet_t command;
    command.commandID = CMD_LED_MODE_ID;
    command.commandData[0] = temp_value;
    xQueueSend(received_commandHandle, &command, portMAX_DELAY);
}

void action_brightness(void) { 
    /* chỉnh độ sáng */ 
    command_packet_t command;
    command.commandID = CMD_LED_BRIGHTNESS_ID;
    command.commandData[0] = temp_value;
    xQueueSend(received_commandHandle, &command, portMAX_DELAY);
}

void action_speed(void) { 
    /* chỉnh tốc độ */ 
    command_packet_t command;
    command.commandID = CMD_LED_SPEED_ID;
    command.commandData[0] = temp_value;
    xQueueSend(received_commandHandle, &command, portMAX_DELAY);
}

void action_notify(void) { /* bật/tắt thông báo */ }

void action_sleep(void) { /* tự động ngủ */ }

void action_display_off(void) { /* tắt màn */ }

void action_AI(void) { /* AI realtime */ }

void action_volume(void) { /* âm lượng */ }

void action_wifi(void) { /* wifi */ }

void action_check_update(void) { /* cập nhật */ }

void action_factory_reset(void) { /* reset */ }

void menu_render(void)
{
    switch (current_page)
    {
        case HOME_PAGE:
            render_home_page();
            break;

        case SETTING_PAGE:
            render_setting_page();
            break;

        default:
            break;
    }
}

void render_home_page(void)
{   
    uint8_t value;
    char buffer2[30];
    sprintf(buffer2, "%d new messages", value);
    SH1106_Clear_Range(0, NOTIFY_DIALOG_POSITION_Y, 127, NOTIFY_DIALOG_POSITION_Y+10);
    SH1106_GotoXY(NOTIFY_DIALOG_POSITION_X - count_digits(value), NOTIFY_DIALOG_POSITION_Y);
    SH1106_Puts(buffer2, &Font_7x10, SH1106_COLOR_WHITE);

    update_ambient_info();
   
    SH1106_Update_Range(0,NOTIFY_DIALOG_POSITION_Y,127,NOTIFY_DIALOG_POSITION_Y+10);
}

void render_setting_page(void)
{
    SH1106_Clear();

    for (uint8_t i = 0; i < VISIBLE_LINES; i++)
    {
        uint8_t idx = menu.top + i;

        if (idx >= MENU_COUNT)
            break;

        uint8_t y = i * LINE_HEIGHT;

        // highlight
        if (idx == menu.selected_option)
        {
            SH1106_DrawFilledRectangle(
                0,
                y,
                SH1106_WIDTH,
                LINE_HEIGHT - 1,
                SH1106_COLOR_WHITE
            );

            SH1106_GotoXY(0, y + TEXT_PADDING_Y);

            SH1106_Puts(
                menu_items[idx].label,
                &Font_7x10,
                SH1106_COLOR_BLACK
            );
        }
        else
        {
            SH1106_GotoXY(0, y + TEXT_PADDING_Y);

            SH1106_Puts(
                menu_items[idx].label,
                &Font_7x10,
                SH1106_COLOR_WHITE
            );
        }
    }

    SH1106_UpdateScreen();
}

void frame_render(void)
{
    uint8_t old_y =
        (menu.old_selected_option % VISIBLE_LINES)
        * LINE_HEIGHT;

    uint8_t new_y =
        (menu.selected_option % VISIBLE_LINES)
        * LINE_HEIGHT;

    //
    // redraw old line
    //
    SH1106_Clear_Range(
        0,
        old_y,
        SH1106_WIDTH,
        old_y + LINE_HEIGHT
    );

    SH1106_GotoXY(0, old_y + TEXT_PADDING_Y);

    SH1106_Puts(
        menu_items[menu.old_selected_option].label,
        &Font_7x10,
        SH1106_COLOR_WHITE
    );

    //
    // redraw new line
    //
    SH1106_Clear_Range(
        0,
        new_y,
        SH1106_WIDTH,
        new_y + LINE_HEIGHT
    );

    SH1106_DrawFilledRectangle(
        0,
        new_y,
        SH1106_WIDTH,
        LINE_HEIGHT - 1,
        SH1106_COLOR_WHITE
    );

    SH1106_GotoXY(0, new_y + TEXT_PADDING_Y);

    SH1106_Puts(
        menu_items[menu.selected_option].label,
        &Font_7x10,
        SH1106_COLOR_BLACK
    );

    uint8_t min_y =
        (old_y < new_y) ? old_y : new_y;

    uint8_t max_y =
        (old_y > new_y) ? old_y : new_y;

    SH1106_Update_Range(
        0,
        min_y,
        SH1106_WIDTH,
        max_y + LINE_HEIGHT
    );
}

static void render_setting_value(uint8_t value)
{
    uint8_t y = (menu.selected_option % VISIBLE_LINES) * LINE_HEIGHT;

    char str[5];
    sprintf(str, "%d", value);

    SH1106_Clear_Range(
        SH1106_WIDTH - 24,
        y,
        SH1106_WIDTH,
        y + LINE_HEIGHT);

    SH1106_GotoXY(
        SETTING_TOGGLE_POSITION_X,
        y + TEXT_PADDING_Y);

    SH1106_Puts(
        str,
        &Font_7x10,
        SH1106_COLOR_WHITE);

    SH1106_Update_Range(
        SH1106_WIDTH - 24,
        y,
        SH1106_WIDTH,
        y + LINE_HEIGHT);
}

void active_setting_render(void)
{   
    temp_value = menu_items[menu.selected_option].get_value();
    render_setting_value(temp_value);
}

void deactive_setting_render(void) {
    uint8_t y = (menu.selected_option % VISIBLE_LINES) * LINE_HEIGHT;
    SH1106_Clear_Range(SH1106_WIDTH - 24, y, SH1106_WIDTH, y + LINE_HEIGHT);
    SH1106_DrawFilledRectangle(
        SH1106_WIDTH - 24,
        y,
        SH1106_WIDTH,
        LINE_HEIGHT - 1,
        SH1106_COLOR_WHITE
    );
    SH1106_Update_Range(SH1106_WIDTH - 24, y, SH1106_WIDTH, y + LINE_HEIGHT);
}

void increase_value_setting_render(void)
{
    INCREASE_TO_MAX(
        temp_value,
        menu_items[menu.selected_option].value_range.max_value,
        menu_items[menu.selected_option].value_range.step);

    render_setting_value(temp_value);
}

void decrease_value_setting_render(void)
{
    DECREASE_TO_MIN(
        temp_value,
        menu_items[menu.selected_option].value_range.min_value,
        menu_items[menu.selected_option].value_range.step);

    render_setting_value(temp_value);
}

void update_ambient_info(void) {
    LOCK();
    uint8_t temperature = global_system_data.DHT22.temperature;
    uint8_t humidity = global_system_data.DHT22.humidity;
    UNLOCK();
    char buffer[10];
    char buffer1[10];

    SH1106_ShowFixedInfo();
    sprintf(buffer, "%d*C", temperature);
    sprintf(buffer1, "%d%%", humidity);
     // // ---- TEMP ----
    SH1106_Clear_Range(TEMPERATURE_VALUE_POSITION_X, TEMPERATURE_VALUE_POSITION_Y,TEMPERATURE_VALUE_POSITION_X + VALUE_W,TEMPERATURE_VALUE_POSITION_Y + VALUE_H);
    SH1106_GotoXY(TEMPERATURE_VALUE_POSITION_X,TEMPERATURE_VALUE_POSITION_Y);
    SH1106_Puts(buffer, &Font_7x10, SH1106_COLOR_WHITE);
    
    // // ---- HUMI ----
    SH1106_Clear_Range(HUMIDITY_VALUE_POSITION_X, HUMIDITY_VALUE_POSITION_Y, HUMIDITY_VALUE_POSITION_X + VALUE_W, HUMIDITY_VALUE_POSITION_Y + VALUE_H);
    SH1106_GotoXY(HUMIDITY_VALUE_POSITION_X,HUMIDITY_VALUE_POSITION_Y);
    SH1106_Puts(buffer1, &Font_7x10, SH1106_COLOR_WHITE);
    
    // // ---- UPDATE ONLY REGION
    SH1106_Update_Range(TEMPERATURE_VALUE_POSITION_X, TEMPERATURE_VALUE_POSITION_Y, TEMPERATURE_VALUE_POSITION_X + VALUE_W, TEMPERATURE_VALUE_POSITION_Y + VALUE_H);
    SH1106_Update_Range(HUMIDITY_VALUE_POSITION_X, HUMIDITY_VALUE_POSITION_Y, HUMIDITY_VALUE_POSITION_X + VALUE_W, HUMIDITY_VALUE_POSITION_Y + VALUE_H);
}