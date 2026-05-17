#include <Arduino.h>
#include <SPI.h>
#include "../include/lora/aodv_layer.h"
#include "ble_gatt/ble_gatt.h"

// ─── BLE receive handler ──────────────────────────────────────────────────────
// Fires when Flutter app writes to RX characteristic.
// For now: just print what arrived. Later: call aodv_sendpayload() here.
void on_ble_rx(uint8_t dest_addr, const uint8_t *payload, size_t len)
{
  Serial.printf("[BLE→MESH] dest=0x%02X  payload(%d bytes): ", dest_addr, len);
  for (size_t i = 0; i < len; i++)
  {
    Serial.printf("%c", payload[i]); // print as text if you're sending strings
  }
  Serial.println();
}

void setup()
{
  Serial.begin(115200);

  aodv_init();
  ble_gatt_init("LoRa_Mesh_Node0", on_ble_rx);
}

void loop()
{
  aodv_control_tick();
  ble_gatt_loop(); // handles reconnect advertising restart
}