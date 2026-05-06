#include <Arduino.h>

#include "../include/lora/aodv_layer.h"
#include "../include/lora/mac_layer.h"
#include "../include/lora/messages_types.h"
#define MY_NODE_ID NODE_A
#define MAX_ROUTES 6

static volatile uint8_t node_sequence_number = 0;

static void payload_handler(uint8_t *buf, uint8_t len);
static void rreq_handler(RREQ_MESSAGE_t *rreq);
static void rrep_handler(RREP_MESSAGE_t *rrep);

static ROUTE_TABLE_ENTRY_t route_table[MAX_ROUTES];
void aodv_callbacks(void)
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
    return NULL;
}

/*Application layer uses this function to send payload*/
void aodv_sendpayload(uint8_t dest, uint8_t *data, uint8_t len)
{
    uint8_t next_hop = next_hop_route_table_lookup(dest);
    if (next_hop)
    {
        mac_send_payload(data, len);
    }
    else
    {
    }
}
