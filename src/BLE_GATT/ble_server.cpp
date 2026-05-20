
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "../../include/lora/ble_server.h"

#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc" /*indentifies a unique service*/
#define CHAR_TX_UUID "12345678-1234-1234-1234-123456789abd" /*phone writes*/
#define CHAR_RX_UUID "12345678-1234-1234-1234-123456789abe" /*phone reads*/

static BLECharacteristic *rxCharacteristic = nullptr;
static bool deviceConnected = false;

/*Define a write characteristic callback typedef*/

static OnMessageReceied _onMessageReceived = nullptr;

class TXcallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *c) override
    {
        std::string val = c->getValue();
        if (_onMessageReceived && val.length() > 0)
        {
            _onMessageReceived(val.c_str(), val.length());
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
        s->startAdvertising();
    }
};

void ble_init(OnMessageReceied cb)
{
    _onMessageReceived = cb;
    BLEDevice::init("LORA_NODE_A");
    BLEServer *server = BLEDevice::createServer();

    BLEService *service = server->createService(SERVICE_UUID);
    BLECharacteristic *tchar = service->createCharacteristic(
        CHAR_TX_UUID,
        BLECharacteristic::PROPERTY_WRITE

    );

    rxCharacteristic = service->createCharacteristic(
        CHAR_TX_UUID,
        BLECharacteristic::PROPERTY_NOTIFY);
    rxCharacteristic->addDescriptor(new BLE2902);
    service->start();
    BLEDevice::startAdvertising();
}

/*Send data to the flutter app*/

void ble_notify(const char *msg, uint8_t len)
{
    if (deviceConnected && rxCharacteristic)
    {
        rxCharacteristic->setValue((uint8_t *)msg, len);
        rxCharacteristic->notify();
    }
}
