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
        .type  = SETTING_TYPE_RANGE,
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
        .type  = SETTING_TYPE_RANGE,
        .value_range = {
            .min_value = 10,
            .max_value = 100,
            .step = 10,
        },
        .get_value = &PL9823_get_brightness,
        .action = &action_brightness
    },
    {
        .label = "3.LED Speed",
        .type  = SETTING_TYPE_RANGE,
        .value_range = {
            .min_value = 0,
            .max_value = 100,
            .step = 10,
        },
        .get_value = &PL9823_get_speed,
        .action = &action_speed
    },
    {
        .label = "4.Notification",
        .type  = SETTING_TYPE_TOGGLE,
        .value_range = {
            .min_value = RESET,
            .max_value = SET,
            .step = 1,
        },
        .get_value = &SysData_get_notification_setting,
        .action = &action_notify
    },
    {
        .label = "5.Auto sleep",
        .type  = SETTING_TYPE_TOGGLE,
        .value_range = {
            .min_value = RESET,
            .max_value = SET,
            .step = 1,
        },
        .get_value = &SysData_get_auto_sleep_setting,
        .action = &action_sleep
    },
    {
        .label = "6.DispAutoOff",
        .type  = SETTING_TYPE_TOGGLE,
        .value_range = {
            .min_value = RESET,
            .max_value = SET,
            .step = 1,
        },
        .get_value = &SysData_get_disp_auto_off_setting,
        .action = &action_display_off
    },
    {
        .label = "7.AI Realtime",
        .type  = SETTING_TYPE_TOGGLE,
        .value_range = {
            .min_value = RESET,
            .max_value = SET,
            .step = 1,
        },
        .get_value = &SysData_get_AI_realtime_setting,
        .action = &action_AI
    },
    {
        .label = "8.Volume",
        .type  = SETTING_TYPE_RANGE,
        .value_range = {
            .min_value = 0,
            .max_value = 100,
            .step = 10,
        },
        .get_value = &SysData_get_volume_setting,
        .action = &action_volume
    },
    {
        .label = "9.Wifi",
        .type  = SETTING_TYPE_TOGGLE,
        .value_range = {
            .min_value = RESET,
            .max_value = SET,
            .step = 1,
        },
        .get_value = &SysData_get_wifi_setting,
        .action = &action_wifi
    },
    {
        .label = "10.Check Update",
        // .get_value = NULL,
        .action = &action_check_update
    },
    {
        .label = "11.Reset Device",
        // .get_value = NULL,
        .action = &action_factory_reset
    }
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

void send_command(command_t cmd_id, const uint8_t *data, uint8_t length) {
    command_packet_t command = {0};
    command.commandID = cmd_id;
    command.length = length;

    if ((data != NULL) && (length > 0))
    {
        memcpy(command.commandData,
               data,
               length);
    }

    xQueueSend(received_commandHandle,
                      &command,
                      portMAX_DELAY);
}

void action_mode(void)
{
    send_command(
        CMD_LED_MODE_ID,
        &temp_value,
        1
    );
}

void action_brightness(void)
{
    send_command(
        CMD_LED_BRIGHTNESS_ID,
        &temp_value,
        1
    );
}

void action_speed(void)
{
    send_command(
        CMD_LED_SPEED_ID,
        &temp_value,
        1
    );
}

void action_notify(void)
{
    send_command(
        CMD_SYSTEM_NOTIFICATION_ID,
        &temp_value,
        1
    );
}

void action_sleep(void)
{
    send_command(
        CMD_SYSTEM_AUTO_SLEEP_ID,
        &temp_value,
        1
    );
}

void action_display_off(void)
{
    send_command(
        CMD_LCD_DISP_AUTO_OFF_ID,
        &temp_value,
        1
    );
}

void action_AI(void)
{
    send_command(
        CMD_SYSTEM_AI_REALTIME_ID,
        &temp_value,
        1
    );
}

void action_volume(void)
{
    send_command(
        CMD_SYSTEM_VOLUME_ID,
        &temp_value,
        1
    );
}

void action_wifi(void)
{
    send_command(
        CMD_SYSTEM_WIFI_ID,
        &temp_value,
        1
    );
}

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
    SH1106_ShowFixedInfo();
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

static void render_setting_value(uint8_t value, setting_type_t type )
{
    char str[5];
    switch (type) {
        case SETTING_TYPE_TOGGLE:
            strcpy(str, value ? "ON" : "OFF");
            break;

        case SETTING_TYPE_RANGE:
            sprintf(str, "%d", value);
            break;

        case SETTING_TYPE_ENUM:
            break;
    }

    uint8_t y = (menu.selected_option % VISIBLE_LINES) * LINE_HEIGHT;
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
    render_setting_value(temp_value, menu_items[menu.selected_option].type);
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

    render_setting_value(temp_value, menu_items[menu.selected_option].type);
}

void decrease_value_setting_render(void)
{
    DECREASE_TO_MIN(
        temp_value,
        menu_items[menu.selected_option].value_range.min_value,
        menu_items[menu.selected_option].value_range.step);

    render_setting_value(temp_value, menu_items[menu.selected_option].type);
}

void update_ambient_info(void) {
    LOCK();
    uint8_t temperature = global_system_data.DHT22.temperature;
    uint8_t humidity = global_system_data.DHT22.humidity;
    UNLOCK();
    char buffer[10];
    char buffer1[10];

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