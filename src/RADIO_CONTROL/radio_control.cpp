#include <Arduino.h>
#include <SPI.h>
#include "../../include/lora/sx1276.h"
#include "../../include/lora/radio_control.h"

/*Setting the DIo0 pin interrupt signal*/
#define DIO0_MAP_TX_DONE (0X01u << 6)
#define DIO0_MAP_RX_DONE (0X00u << 6)
#define DIOO_MAP_CAD_DONE (0x01u << 7)

/*Setting of the differrnt flags*/
#define RX_DONE_MASK (0X01u << 6)
#define TX_DONE_MASK (0X01u << 3)
#define PAYLOAD_CRC_ERROR_MASK (0X01u << 5)
#define CAD_DONE_MASK (0X01u << 2)
#define CAD_DETECTED_MASK 0X01

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
void transmit(uint8_t transmit_buffer[])
{
    set_Mode(STDBY_MODE);
    writeRegister(REG_DIO_MAPPING1, DIO0_MAP_TX_DONE);
    writeRegister(REG_IRQ__FLAGS_MASK, TX_DONE_MASK);
    uint8_t fifo_tx_base_pointer = readRegister(REG_FIFO_TX_BASE_ADDR);
    int8_t i;
    for (i = 0; transmit_buffer[i] != '\0'; i++)
    {
        writeRegister(REG_FIFO, transmit_buffer[i]);
    }
    writeRegister(REG_LORA_PAYLOAD_LENGTH, i);
}
