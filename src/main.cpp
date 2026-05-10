

#include <Arduino.h>
#include <SPI.h>
#include "../include/lora/aodv_layer.h"

void setup()
{

  Serial.begin(115200);
  /*
  char data[] = "Hello Radio world, This is LoRa communiacation:\n: a Long range radio technology developed by SEMTECH limited:";

  aodv_init();
  aodv_sendpayload(NODE_B, (uint8_t *)data, strlen(data));
   */
  aodv_init();
}

void loop()
{
  aodv_control_tick();
}
