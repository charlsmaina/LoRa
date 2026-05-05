
#ifndef MESSAGES_TYPES_H
#define MESSAGES_TYPE_H
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

typedef struct
{
    uint8_t dest_ip;
    uint8_t route[3] = {0};
    uint8_t dest_seq_no;
} OUTER_NODES_t;

typedef struct
{

    uint8_t node_seq;
    uint8_t node_ip;
    OUTER_NODES_t route[3];

} ROUTE_TABLE_ENTRY_t;

#endif