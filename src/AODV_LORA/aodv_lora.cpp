#include <Arduino.h>

#include "../include/lora/aodv_layer.h"
#include "../include/lora/mac_layer.h"
#include "../include/lora/messages_types.h"
/*For printing macros*/
#define STR(x) #x
#define XSTR(x) STR(x)

#define MY_NODE_ID NODE_B
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
static volatile uint32_t RECENT_RREQ_UNIQUE_ID = 0;
static volatile uint32_t RECENT_RREP_UNIQUE_ID = 0;
static volatile uint32_t RECENT_ACK_UNIQUE_ID = 0;
static volatile bool cts_flag = false;

static void add_route_to_sender(uint8_t dest, uint8_t next_hop, uint8_t hop_count);

/*Handlers of different kind of messages once rx done is triggered*/
static void payload_handler(uint8_t *buf, uint8_t len);
static void rreq_handler(RREQ_MESSAGE_t *rreq);
static void rrep_handler(RREP_MESSAGE_t *rrep);
static void rts_handler(RTS_MESSAGE_t *rts);
static void cts_handler(CTS_MESSAGE_t *cts);
static void ack_handler(ACK_MESSAGE_t *ack);
void static wait_for_handshake(uint8_t dest_ip, uint8_t src_ip);

void static display_message_delivered(ACK_MESSAGE_t *ack);

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
    mac_init(payload_handler, rreq_handler, rrep_handler, rts_handler, cts_handler, ack_handler);
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

/*Different control messages: only sent by  either dest or orig node:*/
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

    Serial.printf("Sent dest ip:0X%02X\nSent orig_ip :0x%02X\n", rreq.dest_ip, rreq.ori_ip);

    RECENT_RREQ_UNIQUE_ID = rreq.rreq_id;
    mac_send_rreq(&rreq);
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
    rrep.next_hop = next_hop_route_table_lookup(rreq->ori_ip);
    rrep.rrep_id = esp_random();

    Serial.printf("Sending a route reply  from %s:\n", XSTR(MY_NODE_ID));

    RECENT_RREP_UNIQUE_ID = rrep.rrep_id;
    wait_for_handshake(rrep.next_hop, rrep.dest_ip);

    while (cts_flag)
    {
        cts_flag = false;
        mac_send_rrep(&rrep);
    }
}

/*void static aodv_send_rts(uint8_t dest_ip, uint8_t src_ip);*/

/*Add an entry in a route table if it does not exist*/
/*
------------------replaced by wait_for_handshake()--------------

void static aodv_send_rts(uint8_t dest_ip, uint8_t src_ip)
{
    RTS_MESSAGE_t *rts;
    rts->dest_ip = dest_ip;
    rts->src_ip = src_ip;

    cts_flag = false;
    mac_send_rts(rts);
}


*/

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
        Serial.printf("New route created:\n");
        if (!route_table[i].valid)
        {
            route_table[i].dest_ip = dest;
            route_table[i].next_hop = src_ip;
            route_table[i].valid = true;
            return;
        }
    }
}

/*Different kinds of messages handlers*/
/*----------------------------payload handler------------------------------------*/

static void payload_handler(uint8_t *buf, uint8_t len)
{
    Serial.printf("Payload handler called:\n");
    /*uint8_t dest = buf[1];

    if (dest != MY_NODE_ID)

    {
        uint8_t next_hop = next_hop_route_table_lookup(dest);
        if (next_hop)
        {
            mac_forward(buf, len);
        }

        return;
    }*/

    Serial.printf("Payload received, len %0X\nData:\n", len);
    for (uint8_t i = 0; i < len; i++)
    {
        Serial.printf("%c", buf[i]);
    }
    Serial.println();
}

/*---------------------------------------------rreq handler------------------------*/
static void rreq_handler(RREQ_MESSAGE_t *rreq)
{
    Serial.printf("HAnndle rreq:\n");
    add_route_to_sender(rreq->dest_ip, rreq->src_ip, rreq->hop_count);
    Serial.printf("Destination: 0X%02X\n Origin: 0X%02X\n", rreq->dest_ip, rreq->ori_ip);

    Serial.printf("Current destination:0X%02X\n", MY_NODE_ID);
    if (rreq->dest_ip == MY_NODE_ID)
    {
        Serial.printf("\nAm destination\n");
        aodv_send_rrep(rreq);
    }
    else
    {
        /*Prevent two nodes from entering in a route request cycle*/
        if (rreq->rreq_id == RECENT_RREQ_UNIQUE_ID)
        {
            Serial.printf("ROute request already handled:\n");
            return;
        }
        else
        {
            rreq->hop_count++;
            mac_send_rreq(rreq);
        }
    }
}

/*----------------------------------rrep handler------------------------------------*/

static void rrep_handler(RREP_MESSAGE_t *rrep)
{
    if (RECENT_RREP_UNIQUE_ID == rrep->rrep_id)
    {
        Serial.printf("RREP handled previously:\n");
        return;
    }
    else
    {

        Serial.printf("Route reply detected:\n");
        add_route_to_sender(rrep->dest_ip, rrep->src_ip, rrep->hop_count);
        if (rrep->ori_ip == MY_NODE_ID)
        {
            aodv_drain_queue();
        }
        else
        {
            /*forward the route reply*/
            mac_send_rrep(rrep);
        }
    }
}

/*------------------------------------rts handler --------------------------------------*/
static void rts_handler(RTS_MESSAGE_t *rts)
{
    if (rts->dest_ip == MY_NODE_ID)
    {
        CTS_MESSAGE_t *cts;

        cts->dest_ip = rts->src_ip;
        cts->src_ip = rts->dest_ip;

        mac_send_cts(cts);
    }
    else
        return;
}
/*-----------------------------------cts handler------------------------------------------*/
static void cts_handler(CTS_MESSAGE_t *cts)
{
    if (cts->dest_ip == MY_NODE_ID)
    {
        cts_flag = true;
    }
    else
    {
        /*back off for some time */
        int start_time = millis();
        int timeout = 5000;
        while ((millis() - start_time) < timeout)
            ;
        return;
    }
}
/*-----------------------------------ack handler --------------------------------------------*/
static void ack_handler(ACK_MESSAGE_t *ack)
{
    if (ack->ack_id == RECENT_ACK_UNIQUE_ID)
    {
        /*Drop the ack packet*/
        Serial.printf("ACK dropped: handled previously\n");
        return;
    }
    else if (ack->dest_ip == MY_NODE_ID)
    {
        display_message_delivered(ack);
    }
    else
        mac_send_ack(ack);
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
        Serial.printf("Payload sent:\n");
        pending.valid = false;
    }
}

/*---------------handshake mechanism to minimize interference---------------------*/
void wait_for_handshake(uint8_t dest_ip, uint8_t src_ip)
{
    while (!cts_flag)
    {
        RTS_MESSAGE_t *rts;
        rts->dest_ip = dest_ip;
        rts->src_ip = src_ip;
        mac_send_rts(rts);
    }
}

void static display_message_delivered(ACK_MESSAGE_t *ack)
{
}