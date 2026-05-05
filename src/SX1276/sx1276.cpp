
/*This file contains a simple sx1276 driver to implement a usable physical layer{ The layer is concered with that sending /receiving functionality}*/
#include <Arduino.h>
#include <SPI.h>
#include "../../include/lora/sx1276.h"
#include "../../include/lora/pin_config.h"

static dio0_callback_t s_dio0_cb = nullptr;
static ble_proxy_callback_t s_ble_proxy_cb = nullptr;

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

    (readRegister(REG_REG_VERSION) == 0x12) ? Serial.printf("SPI is ok!\n") : Serial.printf("SPI is faulty!");
}
static void on_dio0_rise(void);
static void on_ble_proxy_rise(void);

void interrupts_pins_setup(dio0_callback_t dio0_cb, ble_proxy_callback_t ble_proxy_cb)
{
    dio0_cb = on_dio0_rise;
    ble_proxy_cb = on_ble_proxy_rise;

    pinMode(PIN_DIO0, INPUT);
    pinMode(PIN_BLE_PROXY, INPUT);

    attachInterrupt(digitalPinToInterrupt(PIN_DIO0), on_dio0_rise, RISING);
    attachInterrupt(digitalPinToInterrupt(PIN_BLE_PROXY), on_ble_proxy_rise, RISING);
}
