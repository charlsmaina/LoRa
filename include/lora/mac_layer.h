#ifndef MAC_LAYER_H
#define MAC_LAYER_H
#include <stdint.h>
uint8_t *pay_load_type(void);
uint8_t *check_payload_type(uint8_t *payload_pointer);

uint8_t message_frame(void);
void frame_and_transmit(uint8_t route_table, uint8_t message_type);

#endif