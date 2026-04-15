

#include <Arduino.h>
#include "../include/lora/tests.h"
#include "../include/lora/lora_config.h"
#include "../include/lora/lora_registers.h"
#include <SPI.h>

void setup()
{

  Serial.begin(115200);

  if (lora_init())
    Serial.printf("Initialization successfull!\nSPI OK!\n");
  else
    Serial.printf("Initialization failed!\nSPI not working!!\n");

  printAllRegisters();

  set_Mode(SLEEP_MODE);
  set_lora_mode();
  Serial.printf("New value of REG_OP_MODE: 0x%02X", readRegister(REG_OP_MODE));
}

void loop()
{
}
