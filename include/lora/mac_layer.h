#ifndef MAC_LAYER_H
#define MAC_LAYER_H
#include <stdint.h>
#include "../include/lora/messages_types.h"

typedef void (*payload_cb_t)(uint8_t *buf, uint8_t len);

typedef void (*rreq_cb_t)(RREQ_MESSAGE_t *rreq);
typedef void (*rrep_cb_t)(RREP_MESSAGE_t *rrep);

void mac_init(payload_cb_t payload_handler, rreq_cb_t rreq_handler, rrep_cb_t rrep_handler);

typedef enum
{
    PAYLOAD_MSG,
    RREQ_MSG,
    RREP_MSG,
    RERR_MSG,
    RTS_MSG,
    RTR_MSG,

} defined_messages_t;

#endif