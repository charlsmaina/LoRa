

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
  set_lora_mode();

  printAllRegisters();

  set_lora_mode();
  Serial.printf("New value of REG_OP_MODE: 0x%02X", readRegister(REG_OP_MODE));
  set_op_frequency();
  Serial.printf("\nNew op frequecy configuration\nMSB: 0x%02X\nMID:0x%02X\nLSB: 0x%02X\n", readRegister(REG_FR_MSB), readRegister(REG_FR_MID), readRegister(REG_FR_LSB));
  set_pa_config();
  Serial.printf("New PA configuration: 0x%02X", readRegister(REG_PA_CONFIG));
}

void loop()
{
}
