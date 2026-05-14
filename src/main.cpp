

#include <Arduino.h>
#include <SPI.h>
#include "../include/lora/aodv_layer.h"

void setup()
{

  Serial.begin(115200);

  /*
  char data[] = "Hello , this is LoRa communiacation; a radio technology cabable of detecting signals degraded to magnitudes of up to 10^15\n";

    aodv_init();
    aodv_sendpayload(NODE_C, (uint8_t *)data, strlen(data));
  */

  aodv_init();
}

void loop()
{
  aodv_control_tick();
}
