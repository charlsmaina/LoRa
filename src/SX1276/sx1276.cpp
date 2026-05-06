
/*This file contains a simple sx1276 driver to implement a usable physical layer{ The layer is concered with that sending /receiving functionality}*/
#include <Arduino.h>
#include <SPI.h>
#include "../../include/lora/sx1276.h"
#include "../../include/lora/pin_config.h"

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
bool lora_spi_init(void)
{
    pinMode(PIN_RESET, OUTPUT);
    lora_hardware_reset();

    pinMode(PIN_NSS, OUTPUT);
    digitalWrite(PIN_NSS, HIGH);
    SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_NSS);
    SPI.setFrequency(1000000);

    uint8_t reg_spi = readRegister(REG_REG_VERSION);
    if (reg_spi == 0X12)
    {
        Serial.printf("SPI OK!\n");
        return true;
    }
    else
    {
        Serial.printf("SPI broken!\n");
        return false;
    }
}

void set_Mode(lora_mode_t mode)
{
    uint8_t reg_value = readRegister(REG_OP_MODE);
    /*Clear last 3 bits and then set them to desired mode*/
    uint8_t value = (~(((0X01 << 3) - 1)) & reg_value) | mode;
    writeRegister(REG_OP_MODE, value);
}

void set_lora_mode(void)
{
    set_Mode(SLEEP_MODE);
    writeRegister(REG_OP_MODE, (0X01 << 7));
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
static volatile bool dio0_fired = false;

void IRAM_ATTR dio0_isr(void)
{
    dio0_fired = true;
}

bool poll_dio0(void)
{
    if (dio0_fired)
    {
        Serial.printf("DI00 fired\n");
        dio0_fired = false;

        return true;
    }
    return false;
}
void interrupts_pins_setup()
{
    pinMode(PIN_DIO0, INPUT);

    attachInterrupt(digitalPinToInterrupt(PIN_DIO0), dio0_isr, RISING);
}
