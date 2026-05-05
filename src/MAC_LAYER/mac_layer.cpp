#include <Arduino.h>

#include "../include/lora/radio_control.h"
uint8_t rx_buffer[]; /*This has to be passed from the application level or??*/

typedef enum
{
    PAYLOAD,
    RREQ,
    RREP,
    RERR,
    RTS,
    RTR,

} defined_messages_t;

uint8_t *pay_load_type(void)
{
    return check_payload_type(extract_fifo_payload(rx_buffer));
}

uint8_t *check_payload_type(uint8_t *payload_pointer)
{
    switch (*payload_pointer)
    {
    case PAYLOAD:
        /*Call a aodv function to handle payload*/
        break;
    case RREQ:
        /*Call a aodv function to handle rreq*/
        break;
    case RREP:
        /*Call a aodv function to handle rrep*/
        break;
    case RERR:
        /*Call a aodv function to handle rerr*/
        break;
    case RTS:
        /*Call a aodv function to handle rts*/
        break;
    case RTR:
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
    case PAYLOAD:
        /*Write a payload message frame */

        break;
    case RREQ:
        /*Write a route request message frame */
        break;
    case RREP:
        /*Write a route reply message frame*/
        break;
    case RERR:
        /*Write a route error message frame*/
        break;
    case RTS:
        /*write a route RTS message frame*/
        break;
    case RTR:
        /*Write a RTR handler*/
        break;

    default:
        break;
    }
}
