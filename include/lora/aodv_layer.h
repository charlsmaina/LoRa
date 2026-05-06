

#ifndef AODV_CONTROL_H
#define AODV_CONTROL_H
#include <stdint.h>
#define NODE_A 0X00
#define NODE_B 0X01
#define NODE_C 0X02
#define NODE_D 0X03

void aodv_init(void);
uint8_t next_hop_route_table_lookup(uint8_t dest);
void aodv_sendpayload(uint8_t dest, uint8_t *data, uint8_t len);
void aodv_control_tick(void);

#endif