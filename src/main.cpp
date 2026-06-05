#include <Arduino.h>
#include <SPI.h>
#include "../include/lora/aodv_layer.h"
#include "../include/lora/ble_server.h"

void on_ble_message(const char *msg, uint8_t len)
{
  Serial.printf("Message arrived in the network: to be sent\n");
  Serial.printf("[BLE-MESH] %s", msg);
  /*Message format from BLE , 0xNN : MESSAGE, MESSAGE_DEST:MESSGE*/
  const char *colon = strchr(msg, ':');
  /*Search for : pattern in msg and return a pointer to it*/
  if (colon == nullptr)
  {
    Serial.printf("Invalid BLE mesh message format:\n");
    return;
  }
  else
  {
    uint8_t dest = (uint8_t)(strtol(msg, nullptr, 16));
  }
}

void setup()
{
  Serial.begin(115200);
  ble_init(on_ble_message);

  aodv_init();
}

void loop()
{
  aodv_control_tick();
}