#ifndef MAC_LAYER_H
#define MAC_LAYER_H
#include <stdint.h>
#include "../include/lora/messages_types.h"

typedef void (*payload_cb_t)(uint8_t *buf, uint8_t len);
typedef void (*rreq_cb_t)(RREQ_MESSAGE_t *rreq);
typedef void (*rrep_cb_t)(RREP_MESSAGE_t *rrep);
typedef void (*rts_cb_t)(RTS_MESSAGE_t *rts);
typedef void (*cts_cb_t)(CTS_MESSAGE_t *cts);
typedef void (*ack_cb_t)(ACK_MESSAGE_t *ack);

void mac_init(payload_cb_t payload_handler, rreq_cb_t rreq_handler, rrep_cb_t rrep_handler, rts_cb_t rts_handler, cts_cb_t cts_handler, ack_cb_t ack_handler);

void mac_forward(uint8_t *buf, uint8_t len);
void mac_send_payload(uint8_t *buf, uint8_t len);
void mac_send_rreq(RREQ_MESSAGE_t *rreq);
void mac_send_rrep(RREP_MESSAGE_t *rrep);
void mac_send_rts(RTS_MESSAGE_t *rts);
void mac_send_cts(CTS_MESSAGE_t *cts);
void mac_send_ack(ACK_MESSAGE_t *ack);

void mac_tick(void);

typedef enum
{
    PAYLOAD_MSG,
    RREQ_MSG,
    RREP_MSG,
    RERR_MSG,
    RTS_MSG,
    CTS_MSG,
    ACK_MSG,

} defined_messages_t;

#endif