#include "data.h"

global_data_t global_data = {
    .LED = {
        .brightness = 10,
        .mode = 1,
        .speed = 1,
        .status = 1
    },
    .DHT22 = {
        .temperature = 0.0f,
        .humidity = 0.0f
    },
    .notification = 1,
    .auto_sleep = 1,
    .disp_auto_off = 0,
    .ai_realtime = 1,
    .volumn = 10
};