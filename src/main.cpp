

#include <Arduino.h>
#include "../include/lora/tests.h"
#include "../include/lora/lora_config.h"
#include "../include/lora/lora_registers.h"
#include <SPI.h>
#include "tests/AODV_LORA/aodv_control.h"

void setup()
{

  Serial.begin(115200);

  if (lora_spi_init())
    Serial.printf("Initialization successfull!\nSPI OK!\n");

  else
    Serial.printf("Initialization failed!\nSPI not working!!\n");

  set_lora_mode();
  set_op_frequency();
  set_pa_config();
  reg_group_init();
}

void loop()
{
}
