#ifndef CONTROL_H
#define CONTROL_H

#include "usart.h"

typedef enum
{
    ACK_OK = 0,
    ACK_INVALID_PARAM,
    ACK_BUSY,
} ack_status_t;

typedef struct
{
    uint8_t seq_id;
    uint8_t status;
} ack_packet_t;

ack_status_t process_command(command_packet_t *);

void send_ACK(ack_packet_t *);

#endif
