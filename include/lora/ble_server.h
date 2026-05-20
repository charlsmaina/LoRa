
#pragma once
#include <stdint.h>
typedef void (*OnMessageReceied)(const char *msg, uint8_t len);

void ble_init(OnMessageReceied cb);