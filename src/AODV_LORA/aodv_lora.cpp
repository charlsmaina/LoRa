#include <Arduino.h>

#include "../include/lora/aodv_layer.h"
#include "../include/lora/mac_layer.h"
#include "../include/lora/messages_types.h"

static void payload_handler(uint8_t *buf, uint8_t len);
static void rreq_handler(RREQ_MESSAGE_t *rreq);
static void rrep_handler(RREP_MESSAGE_t *rrep);

void aodv_callbacks(void)
{
    mac_init(payload_handler, rreq_handler, rrep_handler);
}

static void payload_handler(uint8_t *buf, uint8_t len)
{
    if (buf[2] != NODE_A)
    {
        // drop it:
    }

    Serial.printf("Payload received, len %0X\nData:\n", len);
    for (uint8_t i = 0; i < len; i++)
    {
        Serial.printf("%0X", buf[i]);
    }
    Serial.println();
}
static void rreq_handler(RREP_MESSAGE_t *rreq)
{
}
static void rrep_handler(RREP_MESSAGE_t *rrep, uint8_t len)
{
}