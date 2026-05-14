
#ifndef MESSAGES_TYPES_H
#define MESSAGES_TYPES_H
#include <stdint.h>

typedef struct __attribute__((packed)) /*Prevents padding when you cast*/
{
    uint8_t type;

    uint8_t hop_count;
    uint32_t rreq_id;
    uint8_t dest_ip;

    uint8_t ori_ip;
    uint8_t ori_seq_number;
    uint8_t src_ip;
} RREQ_MESSAGE_t;

typedef struct
{
    uint8_t type;
    uint8_t dest_ip;
    uint8_t src_ip;
    uint8_t next_hop;
    uint8_t data[240];
} PAYLOAD_MESSAGE_t;

typedef struct __attribute__((packed))
{
    uint8_t type;
    uint8_t flags;
    uint8_t hop_count;
    uint8_t dest_ip;

    uint8_t ori_seq_number;
    uint8_t ori_ip;
    uint8_t lifetime;
    uint8_t src_ip;
    uint32_t rrep_id;
} RREP_MESSAGE_t;

typedef struct
{
    uint8_t my_node_id;

    uint8_t my_seq_no;
    uint32_t RECENT_RREQ_UNIQUE_ID;
    uint8_t RECENT_RREP_UNIQUE_ID;
    uint32_t RECENT_ACK_UNIQUE_ID;
} node_specific_info_t;

typedef struct __attribute__((packed))
{

    uint8_t dest_ip;
    uint8_t next_hop;
    int next_hop_RSSI;
    bool valid;
    uint8_t dest_sequence_number;
    uint8_t hop_count;

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