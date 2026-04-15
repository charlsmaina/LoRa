#include "../include/pin_config.h"
#include <Arduino.h>
#include <SPI.h>
// put function declarations here:
uint8_t readReg(uint8_t addr)
{
    digitalWrite(PIN_NSS, LOW);
    SPI.transfer(addr & 0x7F);
    uint8_t val = SPI.transfer(0x00);
    digitalWrite(PIN_NSS, HIGH);
    return val;
}
void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    pinMode(PIN_NSS, OUTPUT);
    digitalWrite(PIN_NSS, HIGH);

    pinMode(PIN_RESET, OUTPUT);
    digitalWrite(PIN_RESET, HIGH);

    SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_NSS);
    SPI.setFrequency(1000000);

    // Hardware reset
    digitalWrite(PIN_RESET, LOW);
    delay(10);
    digitalWrite(PIN_RESET, HIGH);
    delay(10);

    uint8_t version = readReg(0x42);
    Serial.printf("Reg version = 0x%02X\n", version);
    if (version == 0x12)
        Serial.println("SPI OK!");
    else
        Serial.println("SPI FAIL!!");
}

void loop() {}
