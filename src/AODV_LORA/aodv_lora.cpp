#include <Arduino.h>

#include "../include/lora/aodv_layer.h"
#include "../include/lora/mac_layer.h"
#include "../include/lora/messages_types.h"
/*For printing macros*/
#define STR(x) #x
#define XSTR(x) STR(x)
#define TOA_MS 300
#define CONTENTION_WINDOW 500

#define MY_NODE_ID NODE_C
#define MAX_ROUTES 6

static aodv_to_app aodv_to_ble_app = nullptr;

typedef enum
{
    PENDING_RREQ,
    PENDING_RREP,
    PENDING_ACK,

    AODV_IDLE,

} pending_task_t;

/*Used during to store state during the handshake cycle*/
volatile pending_task_t pending_task = AODV_IDLE;
static RREP_MESSAGE_t pending_rrep;
static RREQ_MESSAGE_t pending_rreq;
static ACK_MESSAGE_t pending_ack;
node_specific_info_t current_node;

/*Stores the packet as a AODV works out the route discovery*/
typedef struct
{
    uint8_t dest;
    uint8_t buf[125];
    uint8_t len;
    bool valid;
} pending_packet_t;

static void aodv_queue_payload(PAYLOAD_MESSAGE_t *payload, uint8_t len);
static void aodv_drain_queue(void);

static void update_route_table_due_to_rreq(RREQ_MESSAGE_t *rreq);
static void update_route_table_due_to_rrep(RREP_MESSAGE_t *rrep);

/*Handlers of different kind of messages once rx done is triggered*/
static void payload_handler(PAYLOAD_MESSAGE_t *payload, uint8_t len);
static void rreq_handler(RREQ_MESSAGE_t *rreq);
static void rrep_handler(RREP_MESSAGE_t *rrep);
static void rts_handler(RTS_MESSAGE_t *rts);
static void cts_handler(CTS_MESSAGE_t *cts);
static void ack_handler(ACK_MESSAGE_t *ack);
static void wait_for_handshake(uint8_t dest_ip, uint8_t src_ip);

void static display_message_delivered(ACK_MESSAGE_t *ack);

/*AODV work requiring use of route table : ie AODV RREQ and RREP*/
static void aodv_send_rreq(uint8_t dest);
static void aodv_send_rrep(RREQ_MESSAGE_t *rreq);

static ROUTE_TABLE_ENTRY_t route_table[MAX_ROUTES]; /*The route table defination: can hold 10 routes max*/

void aodv_control_tick(void)
{
    mac_tick();
}

void aodv_init(aodv_to_app aodv_to_main)
{
    aodv_to_ble_app = aodv_to_main;
    mac_init(payload_handler, rreq_handler, rrep_handler, rts_handler, cts_handler, ack_handler);
    Serial.printf("Current node 0x%02X:\n", MY_NODE_ID);
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
    PAYLOAD_MESSAGE_t payload;
    payload.dest_ip = dest;
    payload.src_ip = MY_NODE_ID;
    memcpy(payload.data, data, len);

    Serial.printf("Payload_sender called:\n");
    uint8_t next_hop = next_hop_route_table_lookup(dest);
    if (next_hop != 0XFF)
    {
        payload.next_hop = next_hop;
        mac_send_payload(&payload, len + 4);
        printf("Route found:\n");
    }
    else
    {
        Serial.printf("Route not found:\n");
        aodv_queue_payload(&payload, len); // buffer it
        aodv_send_rreq(dest);              // discover route
        /*Start process of sending a route request*/
    }
}

/*Different control messages: only sent by  either dest or orig node:*/
static void aodv_send_rreq(uint8_t dest)
{
    uint32_t backoff;

    Serial.printf("Sending a route request:\n");
    Serial.printf("Random backoff to minimize collision:\n");
    backoff = esp_random() % CONTENTION_WINDOW;
    Serial.printf("Backing off for %d ms\n", backoff + TOA_MS);
    delay(backoff + TOA_MS);

    RREQ_MESSAGE_t rreq;

    rreq.type = RREQ_MSG;
    rreq.rreq_id = esp_random();
    current_node.RECENT_RREQ_UNIQUE_ID = rreq.rreq_id;
    rreq.dest_ip = dest;

    rreq.ori_ip = MY_NODE_ID;
    current_node.my_seq_no++;
    rreq.ori_seq_number = current_node.my_seq_no;
    rreq.src_ip = MY_NODE_ID;
    rreq.hop_count = 1;
    Serial.printf("Sent dest ip:0X%02X\nSent orig_ip :0x%02X\n", rreq.dest_ip, rreq.ori_ip);

    mac_send_rreq(&rreq);
}

/*rrep payload population by aodv layer*/
static void aodv_send_rrep(RREQ_MESSAGE_t *rreq)
{
    Serial.printf("AODV called to send a route reply\n");
    pending_rrep.type = RREP_MSG;
    pending_rrep.flags = 0;
    pending_rrep.hop_count = 1;
    pending_rrep.dest_ip = rreq->ori_ip;

    pending_rrep.ori_seq_number = 0;
    pending_rrep.ori_ip = MY_NODE_ID;
    pending_rrep.lifetime = 0;
    pending_rrep.src_ip = MY_NODE_ID;

    uint8_t next_hop;

    if ((next_hop = next_hop_route_table_lookup(rreq->ori_ip)) == 0XFF)
    {
        next_hop = rreq->ori_ip;
    }

    current_node.RECENT_RREP_UNIQUE_ID = esp_random();
    pending_rrep.rrep_id = current_node.RECENT_RREP_UNIQUE_ID;

    Serial.printf("Sending a route reply  from %s:\n", XSTR(MY_NODE_ID));

    pending_task = PENDING_RREP;
    Serial.printf("A route reply message is kept pending to wait for handshake\n");

    wait_for_handshake(next_hop, MY_NODE_ID);
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

/*---------------------------------Route table updates--------------------------------------*/
static void update_route_table_due_to_rreq(RREQ_MESSAGE_t *rreq)
{

    for (uint8_t i = 0; i < MAX_ROUTES; i++)
    {
        if (route_table[i].valid && route_table[i].dest_ip == rreq->ori_ip)
        {
            route_table[i].next_hop = rreq->src_ip;
            route_table[i].hop_count = rreq->hop_count;
            route_table[i].next_hop_RSSI = mac_get_rssi();

            return;
        }
    }
    for (uint8_t i = 0; i < MAX_ROUTES; i++)
    {
        Serial.printf("Creating a new route entry:\n");
        if (!route_table[i].valid)
        {
            route_table[i].dest_ip = rreq->ori_ip;
            route_table[i].next_hop = rreq->src_ip;
            route_table[i].hop_count = rreq->hop_count;
            route_table[i].next_hop_RSSI = mac_get_rssi();
            route_table[i].valid = true;

            Serial.printf("Route to 0X%02X created at 0X%02X:\n", route_table[i].dest_ip, MY_NODE_ID);
            Serial.printf("Destination:0X%02X\nNext hop:0X%02X\nHops to destination:0X%02X\nLink RSSI to next hop:%d\n", route_table[i].dest_ip, route_table[i].next_hop, route_table[i].hop_count, route_table[i].next_hop_RSSI);
            return;
        }
    }
}

static void update_route_table_due_to_rrep(RREP_MESSAGE_t *rrep)
{
    Serial.printf("Updating the route table due to a RREP...\n");
    for (uint8_t i = 0; i < MAX_ROUTES; i++)
    {
        if (!(route_table[i].valid))
        {
            route_table[i].dest_ip = rrep->ori_ip;
            route_table[i].next_hop = rrep->src_ip;
            route_table[i].next_hop_RSSI = mac_get_rssi();
            route_table[i].dest_sequence_number = rrep->ori_seq_number;
            route_table[i].hop_count = rrep->hop_count;
            route_table[i].valid = true;

            Serial.printf("Reverse route to 0X%02X created at 0X%02X:\n", route_table[i].dest_ip, MY_NODE_ID);
            Serial.printf("Destination:0X%02X\nNext hop:0X%02X\nHops to destination:0X%02X\nLink RSSI to next hop:%d\n", route_table[i].dest_ip, route_table[i].next_hop, route_table[i].hop_count, route_table[i].next_hop_RSSI);
            return;
        }
    }
}

/*Different kinds of messages handlers*/
/*----------------------------payload handler------------------------------------*/

static void payload_handler(PAYLOAD_MESSAGE_t *payload, uint8_t len)
{
    Serial.printf("Payload handler called:\n");
    Serial.printf("Payload next hop intended node: 0x%02X\n", payload->next_hop);

    if (payload->next_hop != MY_NODE_ID)
    {
        Serial.printf("Am not supposed to listen to this payload: so drop it:\n...silently");
        return;
    }
    else
    {

        if (payload->dest_ip == MY_NODE_ID)
        {
            Serial.printf("Payload at destination:\n, Receiven payload len %d\nData:\nReceived payload\n", len);

            for (uint8_t i = 0; i < len - 4; i++)
            {
                Serial.printf("%c", payload->data[i]);
            }
            Serial.println();
            Serial.printf("Forward message to app\n");
            if (aodv_to_ble_app)
            {
                aodv_to_ble_app(payload->data, payload->dest_ip);
            }

            Serial.printf("Send acknowledgement:\n");
            /*-------------------Send acknowledgement --------------------------*/
        }
        else
        {
            uint8_t next_hop = next_hop_route_table_lookup(payload->dest_ip);
            if (next_hop != 0XFF)
            {
                Serial.printf("Route for payload exists\n");
                payload->src_ip = MY_NODE_ID;
                payload->next_hop = next_hop;
                mac_forward((uint8_t *)payload, len);
            }
            else
            {
                Serial.printf("Am not supposed to handle this payload:back off and drop it\n");
            }
        }
    }
}

/*---------------------------------------------rreq handler------------------------*/
static void rreq_handler(RREQ_MESSAGE_t *rreq)
{
    Serial.printf("RREQ handler called\n");
    Serial.printf("Source of RREQ: 0x%02X\n", rreq->src_ip);
    Serial.printf("RREQ destination: 0x%02X\n", rreq->dest_ip);

    if (rreq->ori_ip == MY_NODE_ID || rreq->rreq_id == current_node.RECENT_RREQ_UNIQUE_ID)
    {
        Serial.printf("Re-hearing my own  or an already served RREQ\n");
        Serial.printf("Don't respond to that RREQ\n");
    }
    else
    {
        update_route_table_due_to_rreq(rreq);
        Serial.printf("Destination ip:: 0X%02X\nOrigin ip: 0X%02X\n", rreq->dest_ip, rreq->ori_ip);
        Serial.printf("Current node ip::0X%02X\n", MY_NODE_ID);
        if (rreq->dest_ip == MY_NODE_ID)
        {
            current_node.RECENT_RREQ_UNIQUE_ID = rreq->rreq_id;
            Serial.printf("\nRREQ at destination\n");
            aodv_send_rrep(rreq);
        }
        else
        {

            Serial.printf("Forward RREQ\n");
            rreq->hop_count++;
            rreq->src_ip = MY_NODE_ID;
            delay(5000);
            mac_send_rreq(rreq);
        }
    }
}

/*----------------------------------rrep handler------------------------------------*/

static void rrep_handler(RREP_MESSAGE_t *rrep)
{
    if (current_node.RECENT_RREP_UNIQUE_ID == rrep->rrep_id)
    {
        Serial.printf("RREP handled previously:\n");
        return;
    }
    else
    {

        Serial.printf("Route reply forwarded to RREP handler:\n");
        update_route_table_due_to_rrep(rrep);

        if (rrep->dest_ip == MY_NODE_ID)
        {
            Serial.printf("Draining the payload queu\n");
            aodv_drain_queue();
        }
        else
        {
            /*forward the route reply*/
            rrep->src_ip = MY_NODE_ID;
            mac_send_rrep(rrep);
        }
    }
}

/*------------------------------------rts handler --------------------------------------*/
static void rts_handler(RTS_MESSAGE_t *rts)
{
    Serial.printf("Resolving a rts :\n");
    Serial.printf("Node 0X%02X is requesting to send data to node with ip 0X%02X if\n", rts->src_ip, rts->dest_ip);
    Serial, printf("My node ID is : 0X%02X\n", MY_NODE_ID);

    if (rts->dest_ip == MY_NODE_ID)
    {
        CTS_MESSAGE_t cts;

        cts.dest_ip = rts->src_ip;
        cts.src_ip = rts->dest_ip;
        delay(1000);

        mac_send_cts(&cts);
    }
    else
    {
        Serial.printf("Am not the destination so back off:\n");
        return;
    }
}
/*-----------------------------------cts handler------------------------------------------*/
static void cts_handler(CTS_MESSAGE_t *cts)
{
    Serial.printf("Clear to send message received:\n");
    if (cts->dest_ip == MY_NODE_ID)
    {
        switch (pending_task)
        {
        case PENDING_RREQ:
            mac_send_rreq(&pending_rreq);
            pending_task = AODV_IDLE;
            break;

        case PENDING_RREP:
            Serial.printf("Received a CTS message to send the PENDING RREP\n");
            mac_send_rrep(&pending_rrep);
            break;
        case PENDING_ACK:
            mac_send_ack(&pending_ack);
            break;
        default:
            Serial.printf("No pending task detected:\n");
            break;
            uint32_t UNIQUE_RREQ_ID;
        }
    }
    else
    {
        /*back off for some time */
        delay(1000);
        return;
    }
}
/*-----------------------------------ack handler --------------------------------------------*/
static void ack_handler(ACK_MESSAGE_t *ack)
{
    if (ack->ack_id == current_node.RECENT_ACK_UNIQUE_ID)
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

static PAYLOAD_MESSAGE_t pending_payload;
static volatile bool pendingPayload = false;
static uint8_t pendingPayload_len = 0;

static void aodv_queue_payload(PAYLOAD_MESSAGE_t *payload, uint8_t len)
{
    pendingPayload = true;
    Serial.printf("Queu and buffer the data\n");
    pending_payload.dest_ip = payload->dest_ip;
    pending_payload.next_hop = 0;
    pending_payload.src_ip = MY_NODE_ID;
    memcpy(pending_payload.data, payload->data, len);
    pendingPayload_len = len;
}

static void aodv_drain_queue(void)
{
    if (pendingPayload)
    {
        pending_payload.next_hop = next_hop_route_table_lookup(pending_payload.dest_ip);
        mac_send_payload(&pending_payload, pendingPayload_len + 4);
        Serial.printf("Payload sent:\n");
        pendingPayload = false;
        pendingPayload_len = 0;
    }
}

/*---------------handshake mechanism to minimize interference---------------------*/
static void wait_for_handshake(uint8_t dest_ip, uint8_t src_ip)
{
    Serial.printf("Initiating handshake:\n");
    RTS_MESSAGE_t rts;

    rts.dest_ip = dest_ip;
    rts.src_ip = src_ip;
    Serial.printf("Sending rts\n");
    mac_send_rts(&rts);
}
void static display_message_delivered(ACK_MESSAGE_t *ack)
{
}