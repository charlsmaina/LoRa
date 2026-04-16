
#include <Arduino.h>
#include <SPI.h>
#include "../include/lora/lora_config.h"
#include "../include/lora/tests.h"
#include "../include/lora/lora_registers.h"
#include "../include/lora/pin_config.h"

uint8_t readRegister(uint8_t addr)
{
    digitalWrite(PIN_NSS, LOW);
    SPI.transfer(0x7F & addr); /*0x7F = 0111111, hence MSB will always be 0*/
    uint8_t register_value = SPI.transfer(0x00);
    digitalWrite(PIN_NSS, HIGH);
    return register_value;
}
void writeRegister(uint8_t addr, uint8_t value)
{
    digitalWrite(PIN_NSS, LOW);
    SPI.transfer(addr | 0x80); /*0x80 = 1000000: hence MSB will always be 1*/
    SPI.transfer(value);
    digitalWrite(PIN_NSS, HIGH);
}
void lora_hardware_reset(void) // reset is active low
{

    digitalWrite(PIN_RESET, LOW);
    delay(10);
    digitalWrite(PIN_RESET, HIGH);
    delay(10);
}
bool lora_init(void)
{
    pinMode(PIN_RESET, OUTPUT);
    lora_hardware_reset();

    pinMode(PIN_NSS, OUTPUT);
    digitalWrite(PIN_NSS, HIGH);
    SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_NSS);
    SPI.setFrequency(1000000);

    if (readRegister(REG_REG_VERSION) == 0x12)

        return true;
    else
        return false;
}
void printAllRegisters(void)
{
    Serial.println("Address values");
    for (uint8_t addr = 0x00; addr <= 0x20; addr++)
    {
        uint8_t val = readRegister(addr);
        Serial.printf("Address:0x%02X :: Default value : 0x%02X\n", addr, val);
    }
}

void set_Mode(lora_mode_t mode)
{
    uint8_t reg_value = readRegister(REG_OP_MODE);
    /*Clear last 3 bits and then set them to desired mode*/
    uint8_t value = ~(((1 << 3) - 1)) & reg_value | mode;
    writeRegister(REG_OP_MODE, value);
}

void set_lora_mode(void)
{
    set_Mode(SLEEP_MODE);
    uint8_t reg_value = readRegister(REG_OP_MODE);
    uint8_t lora_mode = (1 << 7) | reg_value;
    writeRegister(REG_OP_MODE, lora_mode);
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