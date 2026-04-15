
#include <Arduino.h>
#include <SPI.h>
#include "../../include/pin_config.h"
#include "tests.h"
#include "../lora/lora_registers.h"

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
    pinMode(PIN_NSS, OUTPUT);
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