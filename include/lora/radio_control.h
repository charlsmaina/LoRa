#ifndef RADIO_CONTROL_H

#define RADIO_CONTROL_H

typedef enum
{
    SLEEP_MODE = 0x00,
    STDBY_MODE = 0x01,
    TX_MODE = 0x03,
    RX_CONT = 0x05

} lora_mode_t;

void set_Mode(lora_mode_t mode);
void set_lora_mode(void);
void set_op_frequency(void);
void set_pa_config(void);
void set_lna(void);
void set_ocp(void);
void lora_reg_config(uint8_t addr, uint8_t value);
void reg_group_init(void);

#endif