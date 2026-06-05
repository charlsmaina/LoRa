
#pragma once
#include <stdint.h>
typedef void (*OnMessageReceived)(const char *msg, uint8_t len);

void ble_init(OnMessageReceived cb);
void ble_notify(const char *msg, uint8_t len);