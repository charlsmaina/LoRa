#include <Arduino.h>
#include "../include/lora/aodv_lora.h"
#include "../include/lora/mac_layer.h"
#include "../include/lora/messages_types.h"

static void handle_payload();
static void handle_rreq();
static void handle_rrep();
