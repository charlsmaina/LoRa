#ifndef TESTS_H
#define TESTS_H
#include <stdint.h>
void writeRegister(uint8_t addr, uint8_t value);
uint8_t readRegister(uint8_t addr);
void lora_hardware_reset(void);
bool lora_init(void);
#endif