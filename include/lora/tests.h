#ifndef TESTS_H
#define TESTS_H
#include <stdint.h>
#include "lora_config.h"

void writeRegister(uint8_t addr, uint8_t value);
uint8_t readRegister(uint8_t addr);
void lora_hardware_reset(void);
bool lora_init(void);
void printAllRegisters();
void set_Mode(lora_mode_t mode);
void set_lora_mode(void);
void set_op_frequency(void);
void set_pa_config(void);
void set_lna(void);
void lora_reg_config(uint8_t addr, uint8_t value);
void reg_group_init(void);
#endif