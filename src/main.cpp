#include <Arduino.h>
#include <SPI.h>
#include "../include/lora/aodv_layer.h"
#include "../include/lora/ble_server.h"

void on_ble_message(const char *msg, uint8_t len)
{
  Serial.printf("[BLE-MESH] %s", msg);
}

void setup()
{
  ble_init(on_ble_message);
  Serial.begin(115200);

  aodv_init();
}

void loop()
{
  /*aodv_control_tick();*/
}