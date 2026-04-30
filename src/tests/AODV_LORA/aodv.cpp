
#include "aodv_control.h"
#include "../lib.cpp"

RREQ_MESSAGE_t node_a_rreq;
RREP_MESSAGE_t node_a_rrep;

void send_rreq(RREQ_MESSAGE_t *rrq_ptr);
void send_rrep(RREP_MESSAGE_t *rrp_ptr);

void send_rreq(RREQ_MESSAGE_t *rrq_ptr)
{
    uint8_t rreq_buffer[8];
    rreq_buffer[0] = rrq_ptr->type;
    rreq_buffer[1] = rrq_ptr->flags;
    rreq_buffer[2] = rrq_ptr->hop_count;
    rreq_buffer[3] = rrq_ptr->rreq_id;
    rreq_buffer[4] = rrq_ptr->dest_ip;
    rreq_buffer[5] = rrq_ptr->dest_seq_number;
    rreq_buffer[6] = rrq_ptr->ori_ip;
    rreq_buffer[7] = rrq_ptr->ori_seq_number;

    transmit(rreq_buffer);
}

void send_rrep(RREP_MESSAGE_t *rrep_ptr)
{
    uint8_t rrep_buffer[7];
    rrep_buffer[0] = rrep_ptr->type;
    rrep_buffer[0] = rrep_ptr->flags;
    rrep_buffer[0] = rrep_ptr->hop_count;
    rrep_buffer[0] = rrep_ptr->dest_ip;
    rrep_buffer[0] = rrep_ptr->dest_seq_number;
    rrep_buffer[0] = rrep_ptr->ori_ip;
    rrep_buffer[0] = rrep_ptr->lifetime;

    transmit(rrep_buffer);
}
