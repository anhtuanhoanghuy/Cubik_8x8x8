#ifndef __DHT22_H__
#define __DHT22_H__

#include <stdint.h>

typedef enum {
    DHT22_OK = 0,
    DHT22_ERROR,
    DHT22_CHECKSUM_ERROR,
    DHT22_INVALID_DATA
} DHT22_Status_t;

typedef struct {
    int8_t temperature;
    uint8_t humidity;
} DHT22_t;

DHT22_Status_t DHT22_ReadValue(DHT22_t *);

#endif
