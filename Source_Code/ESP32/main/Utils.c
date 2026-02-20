#include "Utils.h"

bool validateChecksum(uint8_t *data, uint8_t checksum) {
    int calc_checksum = 0;
    for (int i = 0; i <= data.length; i++) {
        calc_checksum+= data[i];
    }
    return calc_checksum == checksum;
}