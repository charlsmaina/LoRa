

#ifndef AODV_CONTROL_H
#define AODV_CONTROL_H
#include <stdint.h>
#define NODE_A 0X20
#define NODE_B 0X21
#define NODE_C 0X22
#define NODE_D 0X23

uint8_t next_hop_route_table_lookup(uint8_t dest);
void aodv_sendpayload(uint8_t dest, uint8_t *data, uint8_t len);
void aodv_control_tick(void);
typedef void (*aodv_to_app)(uint8_t *msg, uint8_t src);
void aodv_init(aodv_to_app aodv_to_main);

#endif