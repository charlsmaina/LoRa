/*
---------------------------------------------------------------------------------------------------------------
This sketch is a test to check if SPI is working between the sx1276 chip and the esp32
-------How it works----------------
- SPI.h provides with the SPI transport layer implementation. More like HAL implementation. How the Hardware is manipulated to send data
- SPI is a simultaneos two way protocol. As master sends to slave, master receives from slave
- For sx1276, SPI works by sending two bytes successively. First byte is the register address to write to or read from with MSB indicating if its a write or read operation
- Second byte contains the data. For read , you send a dummy data, and the sx1276 MISO sends the data stored in the specified address.
- The whole read or write operation must be executed while the chip is selected: ie NSS should be low: NB: NSS is active LOW

--------------------------------------------------------------------------------------------------------------
*/

#include "../../include/pin_config.h" // contains SPI pin mappings to ESP32
#include <Arduino.h>
#include <SPI.h>

uint8_t readReg(uint8_t addr)
{
    digitalWrite(PIN_NSS, LOW);
    SPI.transfer(addr & 0x7F);        // 0x7F = 0111111 : hence MSB is 0 for read
    uint8_t val = SPI.transfer(0x00); // save returned value via MISO
    digitalWrite(PIN_NSS, HIGH);
    return val;
}
void setup()
{

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
