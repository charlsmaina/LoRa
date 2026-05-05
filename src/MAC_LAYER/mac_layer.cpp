#include <Arduino.h>

#include "../include/lora/radio_control.h"
uint8_t rx_buffer[]; /*This has to be passed from the application level or??*/

uint8_t *pay_load_type(void)
{
    return check_payload_type(extract_fifo_payload(rx_buffer));
}
uint8_t *check_payload_type(uint8_t *payload_pointer)
{
    switch (*payload_pointer)
    {
    case 0x00:
        /*Call a aodv function to handle payload*/
        break;
    case 0x01:
        /*Call a aodv function to handle rreq*/
        break;
    case 0x02:
        /*Call a aodv function to handle rrep*/
        break;
    case 0x03:
        /*Call a aodv function to handle rerr*/
        break;
    case 0x04:
        /*Call a aodv function to handle rts*/
        break;
    case 0x05:
        /*Call a aodv function to handle rtr*/
        break;
    case 0x06:
        /*Call a aodv function to handle*/
        break;

    default:
        Serial.printf("Undefined payload type:\n");
        break;
    }
}
