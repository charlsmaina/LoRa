

#ifndef AODV_CONTROL_H
#define AODV_CONTROL_H
#include <stdint.h>

typedef struct
{
    uint8_t type;
    uint8_t flags;
    uint8_t hop_count;
    uint8_t rreq_id;
    char dest_ip;
    uint8_t dest_seq_number;
    uint8_t ori_ip;
    uint8_t ori_seq_number;
} RREQ_MESSAGE_t;

typedef struct
{
    uint8_t type;
    uint8_t flags;
    uint8_t hop_count;
    uint8_t dest_ip;
    uint8_t dest_seq_number;
    uint8_t ori_ip;
    uint8_t lifetime;
} RREP_MESSAGE_t;

#endif