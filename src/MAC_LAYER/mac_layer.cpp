#include <Arduino.h>

#include "../include/lora/radio_control.h"
#include "../include/lora/mac_layer.h"
#define MAX_FRAME_SIZE 125

static uint8_t rx_buffer[MAX_FRAME_SIZE];
static uint8_t tx_buffer[MAX_FRAME_SIZE];

static payload_cb_t payload_mac_aodv_cb = NULL;
static rreq_cb_t rreq_mac_aodv_cb = NULL;
static rrep_cb_t rrep_mac_aodv_cb = NULL;
static rts_cb_t rts_mac_aodv_cb = NULL;
static cts_cb_t cts_mac_aodv_cb = NULL;
static ack_cb_t ack_mac_aodv_cb = NULL;

static void mac_on_tx_done(void);
static void mac_on_rx_done(void);

void mac_init(payload_cb_t payload_handler, rreq_cb_t rreq_handler, rrep_cb_t rrep_handler, rts_cb_t rts_handler, cts_cb_t cts_handler, ack_cb_t ack_handler)
{
    radio_ini();
    payload_mac_aodv_cb = payload_handler;
    rreq_mac_aodv_cb = rreq_handler;
    rrep_mac_aodv_cb = rrep_handler;
    rts_mac_aodv_cb = rts_handler;
    cts_mac_aodv_cb = cts_handler;
    ack_mac_aodv_cb = ack_handler;

    radio_init(mac_on_tx_done, mac_on_rx_done);
}
void mac_forward(uint8_t *buf, uint8_t len)
{
    switch (buf[0])
    {
    case PAYLOAD_MSG:
        transmit(buf, len);
        break;
    case RREP_MSG:
        Serial.printf("RREP sent back to the requester:\n");
        transmit(buf, sizeof(RREP_MESSAGE_t));

        break;
    case RREQ_MSG:
        Serial.printf("RREQ forwarded to radio layer to be sent\n");
        transmit(buf, sizeof(RREQ_MESSAGE_t));

        break;
    case RTS_MSG:
        Serial.printf("A RTS message forwaded to radio layer by MAC\n");
        transmit(buf, sizeof(RTS_MESSAGE_t));

        break;
    case CTS_MSG:
        Serial.printf("A CTS message forwaded for transmit\n");
        transmit(buf, sizeof(CTS_MESSAGE_t));

        break;
    case ACK_MSG:
        Serial.printf("An acknowledgement message forwaded to radio layer by MAC:\n");
        transmit(buf, sizeof(ACK_MESSAGE_t));

        break;

    default:
        break;
    }
}
static void mac_on_rx_done(void)
{
    uint8_t no_bytes = 0;
    uint8_t *payload_pointer = extract_fifo_payload(rx_buffer, &no_bytes);
    if (!payload_pointer)
        return;
    switch (payload_pointer[0])
    {
    case PAYLOAD_MSG:
        /*Call a aodv call back function to handle payload*/
        if (payload_mac_aodv_cb)
        {
            printf("Payload received:\n");
            payload_mac_aodv_cb(payload_pointer, no_bytes);
        }

        break;
    case RREQ_MSG:
        /*Call a aodv funcuint8_t message_frame(void);tion to handle rreq*/
        if (rreq_mac_aodv_cb)
        {
            Serial.printf("RREQ message detected:\n");
            rreq_mac_aodv_cb((RREQ_MESSAGE_t *)payload_pointer);
        }

        break;
    case RREP_MSG:
        if (rrep_mac_aodv_cb)
        {
            Serial.printf("Route repply message detected:\n");
            rrep_mac_aodv_cb((RREP_MESSAGE_t *)payload_pointer);
        }

        break;
    case RERR_MSG:
        /*Call a aodv function to handle rerr*/
        break;
    case RTS_MSG:
        /*Call a aodv function to handle rts*/
        if (rts_mac_aodv_cb)
        {
            Serial.printf("RTS message detected:\n");
            rts_mac_aodv_cb((RTS_MESSAGE_t *)payload_pointer);
        }

        break;
    case CTS_MSG:
        /*Call a aodv function to handle rtr*/
        if (cts_mac_aodv_cb)
        {
            Serial.printf("CTS message detected:\n");
            cts_mac_aodv_cb((CTS_MESSAGE_t *)payload_pointer);
        }

        break;
    case ACK_MSG:
        if (ack_mac_aodv_cb)
        {
            Serial.printf("ACK message detected:\n");
            ack_mac_aodv_cb((ACK_MESSAGE_t *)payload_pointer);
        }

    default:
        Serial.printf("Undefined payload type:\n");
        break;
    }
}
static void mac_on_tx_done(void)
{
    receive();
}

void mac_send_payload(uint8_t *buf, uint8_t len)
{
    tx_buffer[0] = PAYLOAD_MSG;
    memcpy(tx_buffer + 1, buf, len);
    mac_forward(tx_buffer, len + 1);
}
void mac_send_rreq(RREQ_MESSAGE_t *rreq)
{
    Serial.printf("MAC layer sending a route request:\n");

    rreq->type = RREQ_MSG;

    memcpy(tx_buffer, rreq, sizeof(RREQ_MESSAGE_t));
    mac_forward(tx_buffer, sizeof(RREQ_MESSAGE_t));
}
void mac_send_rrep(RREP_MESSAGE_t *rrep)
{
    rrep->type = RREP_MSG;

    memcpy(tx_buffer, rrep, sizeof(RREP_MESSAGE_t));
    mac_forward(tx_buffer, sizeof(RREP_MESSAGE_t));
}
void mac_send_rts(RTS_MESSAGE_t *rts)
{
    Serial.printf("MAC received request t send RTS:\n");
    rts->type = RTS_MSG;
    Serial.printf("RTS about to compy and forward to MAC\n");
    memcpy(tx_buffer, rts, sizeof(RTS_MESSAGE_t));
    Serial.printf("RTS copied:\n");
    mac_forward(tx_buffer, sizeof(RTS_MESSAGE_t));
}
void mac_send_cts(CTS_MESSAGE_t *cts)
{
    cts->type = CTS_MSG;
    memcpy(tx_buffer, cts, sizeof(CTS_MESSAGE_t));
    mac_forward(tx_buffer, sizeof(RTS_MESSAGE_t));
}

void mac_send_ack(ACK_MESSAGE_t *ack)
{
    ack->type = ACK_MSG;
    memcpy(tx_buffer, ack, sizeof(ACK_MESSAGE_t));
    mac_forward(tx_buffer, sizeof(ACK_MESSAGE_t));
}

void mac_tick(void)
{
    radio_control_tick();
}
