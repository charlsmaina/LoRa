#include <Arduino.h>
#include "BLEDevice.h"
#include "../include/lora/ble_gatt.h"

void ble_gatt_init(const char *device_name)
{
    BLEDevice::init(device_name);                      /*Starts the BLE stack on esp32 and sets device name*/
    BLEServer *server = BLEDevice::createServer();     /*Creates a GATT server, can now host charactristics*/
    BLEAdvertising *adv = BLEDevice::getAdvertising(); /*Gets a pointer to the advertising manager*/
    adv->setScanResponse(true);                        /*Tells the advertiser to include extra data in its response when a scanner actively queries it*/
    BLEDevice::startAdvertising();

    Serial.printf("[BLE] advertising as %s/n", device_name);
}