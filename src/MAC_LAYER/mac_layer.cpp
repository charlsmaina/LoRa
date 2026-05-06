#include <Arduino.h>

#include "../include/lora/radio_control.h"
#include "../include/lora/mac_layer.h"
#define MAX_FRAME_SIZE 125

static uint8_t rx_buffer[MAX_FRAME_SIZE];
static uint8_t tx_buffer[MAX_FRAME_SIZE];

static payload_cb_t payload_mac_aodv_cb = NULL;
static rreq_cb_t rreq_mac_aodv_cb = NULL;
static rrep_cb_t rrep_mac_aodv_cb = NULL;

static void mac_on_tx_done(void);
static void mac_on_rx_done(void);

void mac_init(payload_cb_t payload_handler, rreq_cb_t rreq_handler, rrep_cb_t rrep_handler)
{
    radio_ini();
    payload_mac_aodv_cb = payload_handler;
    rreq_mac_aodv_cb = rreq_handler;
    rrep_mac_aodv_cb = rrep_handler;

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
        transmit(buf, sizeof(RREP_MESSAGE_t));
        break;
    case RREQ_MSG:
        transmit(buf, sizeof(RREQ_MESSAGE_t));
        Serial.printf("RREQ forwarded to radio layer to be sent\n");
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
        break;
    case RTR_MSG:
        /*Call a aodv function to handle rtr*/
        break;

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

void mac_tick(void)
{
    radio_control_tick();
}
