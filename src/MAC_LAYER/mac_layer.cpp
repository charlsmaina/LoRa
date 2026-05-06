#include <Arduino.h>

#include "../include/lora/radio_control.h"
#include "../include/lora/mac_layer.h"
#define MAX_FRAME_SIZE 125

static uint8_t rx_buffer[MAX_FRAME_SIZE];

static payload_cb_t payload_mac_aodv_cb = NULL;
static rreq_cb_t rreq_mac_aodv_cb = NULL;
static rrep_cb_t rrep_mac_aodv_cb = NULL;

static void mac_on_tx_done(void);
static void mac_on_rx_done(void);

void mac_radio_callbacks()
{
    radio_callbacks_init(mac_on_tx_done, mac_on_rx_done);
}

void mac_aodv_callbacks(payload_cb_t payload_handler, rreq_cb_t rreq_handler, rrep_cb_t rrep_handler)
{

    payload_mac_aodv_cb = payload_handler;
    rreq_mac_aodv_cb = rreq_handler;
    rrep_mac_aodv_cb = rrep_handler;
}

uint8_t *pay_load_type(void)
{
    return check_payload_type(extract_fifo_payload(rx_buffer));
}

uint8_t *check_payload_type(uint8_t *payload_pointer)
{
    switch (*payload_pointer)
    {
    case PAYLOAD_MSG:
        /*Call a aodv function to handle payload*/

        break;
    case RREQ_MSG:
        /*Call a aodv funcuint8_t message_frame(void);tion to handle rreq*/
        break;
    case RREP_MSG:
        /*Call a aodv function to handle rrep*/
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

uint8_t message_frame(defined_messages_t type, uint8_t *routetable)
{
    switch (type)
    {
    case PAYLOAD_MSG:
        /*Write a payload message frame */

        break;
    case RREQ_MSG:
        /*Write a route request message frame */
        break;
    case RREP_MSG:
        /*Write a route reply message frame*/
        break;
    case RERR_MSG:
        /*Write a route error message frame*/
        break;
    case RTS_MSG:
        /*write a route RTS message frame*/
        break;
    case RTR_MSG:
        /*Write a RTR handler*/
        break;

    default:
        break;
    }
}
static void mac_on_rx_done(void)
{
    uint8_t *buf = extract_fifo_payload(rx_buffer);
    if (!buf)
        return;
    check_payload_type(buf);
}
static void mac_on_tx_done(void)
{
    receive();
}