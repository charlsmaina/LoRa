#include <Arduino.h>
#include <SPI.h>
#include "../include/lora/aodv_layer.h"
#include "../include/lora/ble_server.h"

void on_ble_message(uint8_t dest, const char *msg, uint8_t len)
{
  Serial.printf("Message arrived in the network: to be sent\n");
  Serial.printf("Message: %s\nLength:%d\nDestination:0x%02X\n", msg, len, dest);
  aodv_sendpayload(dest, (uint8_t *)msg, len);
}

void pass_to_app(uint8_t *msg, uint8_t dest)
{
  char framed[256];
  sniprintf(framed, sizeof(framed), "0x%02X:%s", dest, (const char *)msg);

  ble_notify(framed, strlen(framed));
}

void setup()
{
  Serial.begin(115200);
  ble_init(on_ble_message);
  aodv_init(pass_to_app);
}

void loop()
{
  aodv_control_tick();
}