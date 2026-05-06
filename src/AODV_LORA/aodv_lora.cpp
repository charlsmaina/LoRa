#include <Arduino.h>

#include "../include/lora/aodv_layer.h"
#include "../include/lora/mac_layer.h"
#include "../include/lora/messages_types.h"
/*For printing macros*/
#define STR(x) #x
#define XSTR(x) STR(x)

#define MY_NODE_ID NODE_A
#define MAX_ROUTES 6

typedef struct
{
    uint8_t dest;
    uint8_t buf[125];
    uint8_t len;
    bool valid;
} pending_packet_t;
static void aodv_queue(uint8_t dest, uint8_t *data, uint8_t len);
static void aodv_drain_queue(void);

static volatile uint8_t node_sequence_number = 0;
static void add_route_to_sender(uint8_t dest, uint8_t next_hop, uint8_t hop_count);

static void payload_handler(uint8_t *buf, uint8_t len);
static void rreq_handler(RREQ_MESSAGE_t *rreq);
static void rrep_handler(RREP_MESSAGE_t *rrep);

/*Route request sending:*/
static void aodv_send_rreq(uint8_t dest);
/*Route reply payload filling by aodv*/
static void aodv_send_rrep(RREQ_MESSAGE_t *rreq);

static ROUTE_TABLE_ENTRY_t route_table[MAX_ROUTES];

void aodv_control_tick(void)
{
    mac_tick();
}

void aodv_init(void)
{
    mac_init(payload_handler, rreq_handler, rrep_handler);
}

static void payload_handler(uint8_t *buf, uint8_t len)
{
    uint8_t dest = buf[1];
    if (dest != MY_NODE_ID)

    {
        uint8_t next_hop = next_hop_route_table_lookup(dest);
        if (next_hop)
        {
            mac_forward(buf, len);
        }

        return;
    }

    Serial.printf("Payload received, len %0X\nData:\n", len);
    for (uint8_t i = 0; i < len; i++)
    {
        Serial.printf("%0X", buf[i]);
    }
    Serial.println();
}

uint8_t next_hop_route_table_lookup(uint8_t dest)
{
    for (uint8_t i = 0; i < MAX_ROUTES; i++)
    {
        if (route_table[i].valid && route_table[i].dest_ip == dest)
        {
            return (route_table[i].next_hop);
        }
    }
    return 0XFF;
}

/*Application layer uses this function to send payload*/
void aodv_sendpayload(uint8_t dest, uint8_t *data, uint8_t len)
{
    Serial.printf("Payload_sender called:\n");
    uint8_t next_hop = next_hop_route_table_lookup(dest);
    if (next_hop != 0XFF)
    {
        mac_send_payload(data, len);
        printf("Route found:\n");
    }
    else
    {
        Serial.printf("Route not found:\n");
        aodv_queue(dest, data, len); // buffer it
        aodv_send_rreq(dest);        // discover route
        /*Start process of sending a route request*/
    }
}

/*Different control messages:*/
static void aodv_send_rreq(uint8_t dest)
{
    Serial.printf("Sending a route request:\n");
    RREQ_MESSAGE_t rreq;
    rreq.type = RREQ_MSG;
    rreq.rreq_id = esp_random();
    rreq.dest_ip = dest;
    rreq.dest_seq_number = 0;
    rreq.ori_ip = MY_NODE_ID;
    rreq.ori_seq_number = node_sequence_number++;

    mac_send_rreq(&rreq);
}

/*Add an entry in a route table if it does not exist*/
static void add_route_to_sender(uint8_t dest, uint8_t src_ip, uint8_t hop_count)
{

    for (uint8_t i = 0; i < MAX_ROUTES; i++)
    {
        if (route_table[i].valid && route_table[i].dest_ip == dest)
        {
            route_table[i].next_hop = src_ip;
            route_table[i].hop_count = hop_count;
            return;
        }
    }
    for (uint8_t i = 0; i < MAX_ROUTES; i++)
    {
        if (!route_table[i].valid)
        {
            route_table[i].dest_ip = dest;
            route_table[i].next_hop = src_ip;
            route_table[i].valid = true;
            return;
        }
    }
}

/*RREQ and RREP handlers*/
static void rreq_handler(RREQ_MESSAGE_t *rreq)
{
    add_route_to_sender(rreq->dest_ip, rreq->src_ip, rreq->hop_count);
    if (rreq->dest_ip == MY_NODE_ID)
    {
        Serial.printf("\nAm destination\n");
        aodv_send_rrep(rreq);
    }
    else
    {
        rreq->hop_count++;
        mac_send_rreq(rreq);
    }
}
/*rrep payload population by aodv layer*/
static void aodv_send_rrep(RREQ_MESSAGE_t *rreq)
{
    RREP_MESSAGE_t rrep;
    rrep.type = RREP_MSG;
    rrep.flags = 0;
    rrep.hop_count = 0;
    rrep.dest_ip = MY_NODE_ID;
    rrep.dest_seq_number = 2;
    rrep.ori_ip = rreq->ori_ip;
    rrep.lifetime = 0;
    Serial.printf("Sending a route reply  from %s:\n", XSTR(MY_NODE_ID));
    mac_send_rrep(&rrep);
}

static void rrep_handler(RREP_MESSAGE_t *rrep)
{
    add_route_to_sender(rrep->dest_ip, rrep->src_ip, rrep->hop_count);
    if (rrep->ori_ip == MY_NODE_ID)
    {
        aodv_drain_queue();
    }
}

/*AODV queus*/

static pending_packet_t pending = {.valid = false};

static void aodv_queue(uint8_t dest, uint8_t *data, uint8_t len)
{
    Serial.printf("Queu and buffer the data\n");
    pending.dest = dest;
    pending.len = len;
    pending.valid = true;
    memcpy(pending.buf, data, len);
}

static void aodv_drain_queue(void)
{
    if (pending.valid)
    {
        mac_send_payload(pending.buf, pending.len);
        pending.valid = false;
    }
}