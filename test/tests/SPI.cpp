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
#include "tests.h"

void setup()
{

    Serial.begin(115200);

    lora_hardware_reset();

    if (lora_init)
        Serial.printf("Initialization successfull!\nSPI OK!\n");
    else
        Serial.printf("Initialization failed!\nSPI not working!!\n");
}

void loop() {}
