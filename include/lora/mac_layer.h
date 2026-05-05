#ifndef MAC_LAYER_H
#define MAC_LAYER_H
#include <stdint.h>
#include "../include/lora/messages_types.h"
uint8_t *pay_load_type(void);
uint8_t *check_payload_type(uint8_t *payload_pointer);

typedef void (*payload_cb_t)(uint8_t *buf, uint8_t len);

typedef void (*rreq_cb_t)(RREQ_MESSAGE_t *rreq);
typedef void (*rrep_cb_t)(RREP_MESSAGE_t *rreq);
void mac_callbacks_init(payload_cb_t payload, rreq_cb_t rreq, rrep_cb_t rrep);

typedef enum
{
    PAYLOAD,
    RREQ,
    RREP,
    RERR,
    RTS,
    RTR,

} defined_messages_t;

#endif