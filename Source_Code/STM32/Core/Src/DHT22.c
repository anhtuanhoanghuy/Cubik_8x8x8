#include "DHT22.h"
#include "FreeRTOS.h"
#include "task.h"
#include "defines.h"
#include "stm32f1xx_hal.h"
#include "main.h"
#include "gpio.h"
#include <string.h>

static uint8_t buffer[5] = {0};

static void DHT22_Output_Mode_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = sensor_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(sensor_GPIO_Port, &GPIO_InitStruct);
}

static void DHT22_Input_Mode_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = sensor_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(sensor_GPIO_Port, &GPIO_InitStruct);
}

static void DHT22_Send_Start_Signal(void) {
    DHT22_Output_Mode_Init();
    HAL_GPIO_WritePin(sensor_GPIO_Port, sensor_Pin, GPIO_PIN_RESET);
    delay_us(1000);
    HAL_GPIO_WritePin(sensor_GPIO_Port, sensor_Pin, GPIO_PIN_SET);
    delay_us(30);
    DHT22_Input_Mode_Init();
}

static DHT22_Status_t DHT22_ReadData(void)
{
    memset(buffer, 0, sizeof(buffer));

    taskENTER_CRITICAL();

    DHT22_Send_Start_Signal();

    uint32_t time_start;

    // ===== RESPONSE =====
    time_start = micro_times_ellapse;
    while (HAL_GPIO_ReadPin(sensor_GPIO_Port, sensor_Pin) == GPIO_PIN_SET)
    {
        if (micro_times_ellapse - time_start > 100)
            goto error;
    }

    time_start = micro_times_ellapse;
    while (HAL_GPIO_ReadPin(sensor_GPIO_Port, sensor_Pin) == GPIO_PIN_RESET)
    {
        if (micro_times_ellapse - time_start > 100)
            goto error;
    }

    time_start = micro_times_ellapse;
    while (HAL_GPIO_ReadPin(sensor_GPIO_Port, sensor_Pin) == GPIO_PIN_SET)
    {
        if (micro_times_ellapse - time_start > 100)
            goto error;
    }

    // ===== READ 40 BIT =====
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            // wait LOW → HIGH
            time_start = micro_times_ellapse;
            while (HAL_GPIO_ReadPin(sensor_GPIO_Port, sensor_Pin) == GPIO_PIN_RESET)
            {
                if (micro_times_ellapse - time_start > 100)
                    goto error;
            }

            buffer[i] <<= 1;

            delay_us(40);

            if (HAL_GPIO_ReadPin(sensor_GPIO_Port, sensor_Pin) == GPIO_PIN_SET)
            {
                buffer[i] |= 1;
            }

            // wait HIGH → LOW
            time_start = micro_times_ellapse;
            while (HAL_GPIO_ReadPin(sensor_GPIO_Port, sensor_Pin) == GPIO_PIN_SET)
            {
                if (micro_times_ellapse - time_start > 100)
                    goto error;
            }
        }
    }

    taskEXIT_CRITICAL();
    return DHT22_OK;

error:
    taskEXIT_CRITICAL();
    return DHT22_ERROR;
}

DHT22_Status_t DHT22_ReadValue(DHT22_t *data)
{
    // đọc raw data (giả sử hàm này fill buffer[5])
    if (DHT22_ReadData() != DHT22_OK)
    {
        return DHT22_ERROR; // lỗi đọc
    }

    // checksum (mod 256)
    uint8_t sum = buffer[0] + buffer[1] + buffer[2] + buffer[3];
    if (sum != buffer[4])
    {
        return DHT22_CHECKSUM_ERROR; // checksum lỗi
    }

    // ===== HUMIDITY =====
    uint16_t humid_raw = ((uint16_t)buffer[0] << 8) | buffer[1];
    uint16_t humid = humid_raw / 10;

    if (humid <= 100)
    {
        data->humidity = (uint8_t)humid;
    }
    else
    {
        return DHT22_INVALID_DATA; // giá trị không hợp lệ
    }

    // ===== TEMPERATURE =====
    int16_t temp_raw = ((uint16_t)buffer[2] << 8) | buffer[3];

    // xử lý bit dấu
    if (temp_raw & 0x8000)
    {
        temp_raw &= 0x7FFF;
        temp_raw = -temp_raw;
    }

    int16_t temp = temp_raw / 10;

    if (temp >= -40 && temp <= 80)
    {
        data->temperature = (int8_t)temp;
    }
    else
    {
        return DHT22_INVALID_DATA; // giá trị không hợp lệ
    }

    return DHT22_OK; // OK
}
