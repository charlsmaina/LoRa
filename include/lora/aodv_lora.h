

#ifndef AODV_CONTROL_H
#define AODV_CONTROL_H
#include <stdint.h>
#define A 0X00
#define B 0X01
#define C 0X02
#define D 0X03

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