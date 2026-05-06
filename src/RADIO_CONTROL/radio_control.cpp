#include <Arduino.h>
#include <SPI.h>
#include "../../include/lora/sx1276.h"
#include "../../include/lora/radio_control.h"

/*Setting the DIo0 pin interrupt signal*/
#define DIO0_MAP_TX_DONE (0X01u << 6)
#define DIO0_MAP_RX_DONE 0X00
#define DIOO_MAP_CAD_DONE (0x01u << 7)

/*Setting of the differrnt flags*/
#define RX_DONE_MASK (0X01u << 6)
#define TX_DONE_MASK (0X01u << 3)
#define PAYLOAD_CRC_ERROR_MASK (0X01u << 5)
#define CAD_DONE_MASK (0X01u << 2)
#define CAD_DETECTED_MASK 0X01
#define RELEVANT_MASKS (RX_DONE_MASK | TX_DONE_MASK | PAYLOAD_CRC_ERROR_MASK | CAD_DONE_MASK | CAD_DETECTED_MASK)

static uint8_t op_mode_before_dio0_fired = STDBY_MODE;

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
    writeRegister(REG_IRQ__FLAGS_MASK, ~RELEVANT_MASKS);
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
        switch (op_mode_before_dio0_fired)
        {
        case TX_MODE:
            writeRegister(REG_IRQ_FLAGS, TX_DONE_MASK);
            if (tx_done_cb)
            {
                tx_done_cb();
            }

            break;
        case RX_CONT_MODE:
            writeRegister(REG_IRQ_FLAGS, RX_DONE_MASK);
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
void transmit(uint8_t transmit_buffer[], uint8_t len)
{
    op_mode_before_dio0_fired = TX_MODE;
    set_Mode(STDBY_MODE);
    writeRegister(REG_DIO_MAPPING1, DIO0_MAP_TX_DONE);

    uint8_t fifo_tx_base_pointer = readRegister(REG_FIFO_TX_BASE_ADDR);
    writeRegister(REG_FIFO_ADDR_PTR, fifo_tx_base_pointer);
    int8_t i;
    for (i = 0; i < len; i++)
    {
        writeRegister(REG_FIFO, transmit_buffer[i]);
    }

    writeRegister(REG_LORA_PAYLOAD_LENGTH, i);
    set_Mode(TX_MODE);
}

void receive(void)
{
    op_mode_before_dio0_fired = RX_CONT_MODE;
    set_Mode(STDBY_MODE);
    writeRegister(REG_DIO_MAPPING1, DIO0_MAP_RX_DONE);

    set_Mode(RX_CONT_MODE);
    Serial.printf("\nListening...");
}

uint8_t *extract_fifo_payload(uint8_t rx_buffer[])
{
    uint8_t no_bytes = readRegister(REG_RX_NB_BYTES);
    uint8_t rx_current_address = readRegister(REG_FIFO_RX_CURRENT_ADDR);
    writeRegister(REG_FIFO_ADDR_PTR, rx_current_address);

    uint8_t reg_irq_flags = readRegister(REG_IRQ_FLAGS);

    if (!(reg_irq_flags & PAYLOAD_CRC_ERROR_MASK))
    {
        Serial.printf("\nNo CRC error detected");
        for (uint8_t i = 0; i < no_bytes; i++)
        {
            rx_buffer[i] = readRegister(REG_FIFO);
        }
        Serial.printf("\nExtraction done:\n");
        return rx_buffer;
    }

    else
    {
        Serial.printf("CRC error detected:payload dropped");
        return NULL;
    }
}