#include "Utils.h"

bool validateChecksum(uint8_t *data, uint8_t len, uint8_t checksum) {
    uint8_t calc_checksum = 0;
    for (uint8_t i = 0; i < len; i++) {
        calc_checksum^= data[i];
    }
    return calc_checksum == checksum;
}