#ifndef TESTS_H
#define TESTS_H
#include <stdint.h>
#include "lora_config.h"
#define QUEU_SIZE 8

typedef enum
{
    RX_COMPLETE,
    TX_READY,

} EVENT_TYPE_t;

typedef struct
{
    EVENT_TYPE_t type;

} Event;
void writeRegister(uint8_t addr, uint8_t value);
uint8_t readRegister(uint8_t addr);
void lora_hardware_reset(void);
bool lora_spi_init(void);
void printAllRegisters();
void set_Mode(lora_mode_t mode);
void set_lora_mode(void);
void set_op_frequency(void);
void set_pa_config(void);
void set_lna(void);
void lora_reg_config(uint8_t addr, uint8_t value);
void reg_group_init(void);
void transmit(uint8_t data_buf[]);
void receive(void);
void extract_fifo_payload(uint8_t rx_buffer[]);
void interrupts_pins_setup();

void on_dio0_rise();
void there_is_payload_to_send();
void enqueu(Event event_to_queu);
Event dequeu(void);

bool queu_is_empty();
#endif