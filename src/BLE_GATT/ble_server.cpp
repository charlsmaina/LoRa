#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string.h>
#include <stdlib.h>
#include "../../include/lora/ble_server.h"

#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"
#define CHAR_TX_UUID "12345678-1234-1234-1234-123456789abd"
#define CHAR_RX_UUID "12345678-1234-1234-1234-123456789abe"

static BLECharacteristic *rxCharacteristic = nullptr;
static bool deviceConnected = false;
static OnMessageReceived _onMessageReceived = nullptr;

class TXcallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *c) override
    {
        std::string val = c->getValue();
        if (val.length() == 0)
            return;

        // Flutter sends "0x21:hello"
        // Find the first colon — everything before is dst, after is payload
        const char *raw = val.c_str();
        const char *colon = strchr(raw, ':');

        if (colon == NULL)
        {
            // No colon — malformed packet, drop it
            return;
        }

        // Extract destination address string e.g. "0x21"
        // colon - raw = number of characters before the colon
        char dst_str[8] = {0};
        size_t dst_len = colon - raw;

        if (dst_len == 0 || dst_len >= sizeof(dst_str))
        {
            // destination field empty or too long, drop
            return;
        }

        memcpy(dst_str, raw, dst_len);
        dst_str[dst_len] = '\0';

        // "0x21" → 0x21 as uint8_t
        // strtol with base 16 handles the 0x prefix automatically
        uint8_t dst_addr = (uint8_t)strtol(dst_str, NULL, 16);

        // Everything after the colon is the message payload
        const char *message = colon + 1;
        size_t msg_len = val.length() - dst_len - 1; // subtract "0x21:"

        if (msg_len == 0)
            return; // empty message, drop

        // Fire the callback with parsed destination and payload
        if (_onMessageReceived)
        {
            _onMessageReceived(dst_addr, message, msg_len);
        }
    }
};

class ServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *s) override
    {
        deviceConnected = true;
    }
    void onDisconnect(BLEServer *s) override
    {
        deviceConnected = false;
        s->startAdvertising(); // restart advertising so phone can reconnect
    }
};

void ble_init(OnMessageReceived cb)
{
    _onMessageReceived = cb;
    BLEDevice::init("LORA_NODE_A");

    BLEServer *server = BLEDevice::createServer();
    server->setCallbacks(new ServerCallbacks());

    BLEService *service = server->createService(SERVICE_UUID);

    BLECharacteristic *txchar = service->createCharacteristic(
        CHAR_TX_UUID,
        BLECharacteristic::PROPERTY_WRITE);
    txchar->setCallbacks(new TXcallbacks());

    rxCharacteristic = service->createCharacteristic(
        CHAR_RX_UUID,
        BLECharacteristic::PROPERTY_NOTIFY);

    rxCharacteristic->addDescriptor(new BLE2902());
    service->start();
    BLEDevice::startAdvertising();
}

void ble_notify(const char *msg, uint8_t len)
{
    if (deviceConnected && rxCharacteristic)
    {
        rxCharacteristic->setValue((uint8_t *)msg, len);
        rxCharacteristic->notify();
    }
}