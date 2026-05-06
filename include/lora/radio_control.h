#ifndef RADIO_CONTROL_H

#define RADIO_CONTROL_H

void radio_ini(void);
void radio_control_tick(void);
typedef void (*dio0_callback)(void);

void radio_init(dio0_callback tx_done_handler, dio0_callback rx_done_handler);
void transmit(uint8_t transmit_buffer[], uint8_t len);
void receive(void);
uint8_t *extract_fifo_payload(uint8_t rx_buffer[], uint8_t *no_bytes);

#endif