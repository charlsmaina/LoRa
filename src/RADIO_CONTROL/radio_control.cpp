#include <Arduino.h>
#include <SPI.h>
#include "../../include/lora/sx1276.h"
#include "../../include/lora/radio_control.h"

void set_Mode(lora_mode_t mode)
{
    uint8_t reg_value = readRegister(REG_OP_MODE);
    /*Clear last 3 bits and then set them to desired mode*/
    uint8_t value = (~(((1 << 3) - 1)) & reg_value) | mode;
    writeRegister(REG_OP_MODE, value);
}

void set_lora_mode(void)
{
    set_Mode(SLEEP_MODE);
    writeRegister(REG_OP_MODE, (1 << 7));
}
void set_op_frequency(void)
{
    set_Mode(STDBY_MODE);
    writeRegister(REG_FR_MSB, LORA_FR_MSB);
    writeRegister(REG_FR_MID, LORA_FR_MID);
    writeRegister(REG_FR_LSB, LORA_FR_LSB);
}
void set_pa_config(void)
{
    /*This is about regulating the power levels during transmission:
    Possible paths : PA_BOOST = Max output = +20dBm
    RFO = Max output = +14dBm

    */
    set_Mode(STDBY_MODE);
    writeRegister(REG_PA_CONFIG, LORA_PA_CONFIG);
}
void set_lna(void)
{
    set_Mode(STDBY_MODE);
    writeRegister(REG_LNA, LORA_LNA);
}
void set_ocp(void)
{
    set_Mode(STDBY_MODE);
    writeRegister(REG_OCP, LORA_OCP);
}
void lora_reg_config(uint8_t addr, uint8_t value)
{
    set_Mode(STDBY_MODE);
    writeRegister(addr, value);
}

void reg_group_init(void)
{
    lora_reg_config(REG_FIFO_ADDR_PTR, LORA_FIFO_ADDR_PTR);
    lora_reg_config(REG_FIFO_TX_BASE_ADDR, LORA_FIFO_TX_BASE_ADDR);
    lora_reg_config(REG_FIFO_RX_BASE_ADDR, LORA_FIFO_RX_BASE_ADDR);
    lora_reg_config(REG_IRQ__FLAGS_MASK, LORA_IRQ_FLAGS_MASK);
    lora_reg_config(REG_MODEM_CONFIG1, LORA_MODEM_CONFIG1);
    lora_reg_config(REG_MODEM_CONFIG2, LORA_MODEM_CONFIG2);
    lora_reg_config(REG_SYMB_TIMEOUT_LSB, LORA_SYMB_TIMEOUT_LSB);
    lora_reg_config(REG_PREAMBLE_MSB, LORA_PREAMBLE_MSB);
    lora_reg_config(REG_PREAMBLE_LSB, LORA_PREAMBLE_LSB);
    lora_reg_config(REG_SYNC_WORD, LORA_SYNC_WORD);
}