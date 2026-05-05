#ifndef RADIO_CONTROL_H

#define RADIO_CONTROL_H
void radio_control_tick(void);
typedef void (*dio0_callback)(void);
void radio_callbacks_init(dio0_callback tx_done_handler, dio0_callback rx_done_handler);

#endif