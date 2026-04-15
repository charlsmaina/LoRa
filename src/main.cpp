
#include "../include/pin_config.h" // contains SPI pin mappings to ESP32
#include <Arduino.h>
#include <SPI.h>
#include "tests/tests.h"

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
