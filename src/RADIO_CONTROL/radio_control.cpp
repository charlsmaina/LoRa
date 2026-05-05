#include <Arduino.h>
#include <SPI.h>
#include "../../include/lora/sx1276.h"
#include "../../include/lora/radio_control.h"
void radio_ini(void)
{
    lora_hardware_reset();
    lora_spi_init();
    set_lora_mode();
    set_op_frequency();
    set_pa_config();
    set_lna();
    set_ocp();
    reg_group_init();
    interrupts_pins_setup();
}
static dio0_callback tx_done_cb = nullptr;
static dio0_callback rx_done_cb = nullptr;
void radio_callbacks_init(dio0_callback tx_done_handler, dio0_callback rx_done_handler)
{
    tx_done_cb = tx_done_handler;
    rx_done_cb = rx_done_handler;
}

void radio_control_tick(void)
{
    if (poll_dio0())
    {
        switch (readRegister(REG_OP_MODE) & 0X07)
        {
        case TX_MODE:
            if (tx_done_cb)
            {
                tx_done_cb();
            }

            break;
        case RX_CONT_MODE:
            if (rx_done_cb)
            {
                rx_done_cb();
            }

            break;

        default:
            break;
        }
    }
}