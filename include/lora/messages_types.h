
#ifndef MESSAGES_TYPES_H
#define MESSAGES_TYPES_H
#include <stdint.h>

typedef struct __attribute__((packed)) /*Prevents padding when you cast*/
{
    uint8_t type;

    uint8_t hop_count;
    uint32_t rreq_id;
    uint8_t dest_ip;
    uint8_t dest_seq_number;
    uint8_t ori_ip;
    uint8_t ori_seq_number;
    uint8_t src_ip;
} RREQ_MESSAGE_t;

typedef struct __attribute__((packed))
{
    uint8_t type;
    uint8_t flags;
    uint8_t hop_count;
    uint8_t dest_ip;
    uint8_t next_hop;
    uint8_t dest_seq_number;
    uint8_t ori_ip;
    uint8_t lifetime;
    uint8_t src_ip;
    uint32_t rrep_id;
} RREP_MESSAGE_t;

typedef struct
{

    uint8_t node_seq;
    uint8_t node_ip;
    uint8_t next_hop;
    uint8_t hop_count;
    uint8_t dest_ip;
    uint8_t dest_seq_no;
    bool valid;

} ROUTE_TABLE_ENTRY_t;

typedef struct
{
    uint8_t type;
    uint8_t src_ip;
    uint8_t dest_ip;
} RTS_MESSAGE_t;

typedef struct
{
    uint8_t type;
    uint8_t src_ip;
    uint8_t dest_ip;
} CTS_MESSAGE_t;
typedef struct
{
    uint8_t type;
    uint8_t src_ip;
    uint8_t dest_ip;
    uint32_t ack_id;
} ACK_MESSAGE_t;

#endif