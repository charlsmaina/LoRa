#ifndef LORA_CONFIG_H
#define LORA_CONFIG_H
typedef enum
{
    SLEEP_MODE = 0x00,
    STDBY_MODE = 0x01,
    TX_MODE = 0x03,
    RX_CONT = 0x05

} lora_mode_t;

#define LORA_FR_MSB 0xD9
#define LORA_FR_MID 0x00
#define LORA_FR_LSB 0x00
#define LORA_PA_CONFIG 0x5F
#define LORA_OCP 0x2B
#define LORA_LNA 0x20

/*Use same function :ie: lora_reg_Config*/
#define LORA_FIFO_ADDR_PTR 0x08
#define LORA_FIFO_TX_BASE_ADDR 0x80
#define LORA_FIFO_RX_BASE_ADDR 0x01
#define LORA_IRQ_FLAGS_MASK 0x00
#define LORA_MODEM_CONFIG1 0x72
#define LORA_MODEM_CONFIG2 0xC4
#define LORA_SYMB_TIMEOUT_LSB 0x64
#define LORA_PREAMBLE_MSB 0x00
#define LORA_PREAMBLE_LSB 0x08
#define LORA_SYNC_WORD 0x12

#endif