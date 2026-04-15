#ifndef LORA_CONFIG_H
#define LORA_CONFIG_H
typedef enum
{
    SLEEP_MODE = 0x00,
    STDBY_MODE = 0x01,
    TX_MODE = 0x03,
    RX_CONT = 0x05

} lora_mode_t;
#endif