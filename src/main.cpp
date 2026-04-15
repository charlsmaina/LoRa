

#include <Arduino.h>
#include "lora/lora_config.h"
#include "tests/tests.h"

#include "lora/lora_registers.h"

void setup()
{

  Serial.begin(115200);

  if (lora_init())
    Serial.printf("Initialization successfull!\nSPI OK!\n");
  else
    Serial.printf("Initialization failed!\nSPI not working!!\n");

  printAllRegisters();
  set_Mode(SLEEP_MODE);
}

void loop()
{
}
