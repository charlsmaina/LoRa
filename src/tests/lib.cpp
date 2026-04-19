
#include <Arduino.h>
#include <SPI.h>
#include "../include/lora/lora_config.h"
#include "../include/lora/tests.h"
#include "../include/lora/lora_registers.h"
#include "../include/lora/pin_config.h"

uint8_t readRegister(uint8_t addr)
{
    digitalWrite(PIN_NSS, LOW);
    SPI.transfer(0x7F & addr); /*0x7F = 0111111, hence MSB will always be 0*/
    uint8_t register_value = SPI.transfer(0x00);
    digitalWrite(PIN_NSS, HIGH);
    return register_value;
}
void writeRegister(uint8_t addr, uint8_t value)
{
    digitalWrite(PIN_NSS, LOW);
    SPI.transfer(addr | 0x80); /*0x80 = 1000000: hence MSB will always be 1*/
    SPI.transfer(value);
    digitalWrite(PIN_NSS, HIGH);
}
void lora_hardware_reset(void) // reset is active low
{

    digitalWrite(PIN_RESET, LOW);
    delay(10);
    digitalWrite(PIN_RESET, HIGH);
    delay(10);
}
bool lora_init(void)
{
    pinMode(PIN_RESET, OUTPUT);
    lora_hardware_reset();

    pinMode(PIN_NSS, OUTPUT);
    digitalWrite(PIN_NSS, HIGH);
    SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_NSS);
    SPI.setFrequency(1000000);

    if (readRegister(REG_REG_VERSION) == 0x12)

        return true;
    else
        return false;
}
void printAllRegisters(void)
{
    Serial.println("Address values");
    for (uint8_t addr = 0x00; addr <= 0x20; addr++)
    {
        uint8_t val = readRegister(addr);
        Serial.printf("Address:0x%02X :: Default value : 0x%02X\n", addr, val);
    }
}

void set_Mode(lora_mode_t mode)
{
    uint8_t reg_value = readRegister(REG_OP_MODE);
    /*Clear last 3 bits and then set them to desired mode*/
    uint8_t value = ~(((1 << 3) - 1)) & reg_value | mode;
    writeRegister(REG_OP_MODE, value);
}

void set_lora_mode(void)
{
    set_Mode(SLEEP_MODE);
    uint8_t reg_value = readRegister(REG_OP_MODE);
    uint8_t lora_mode = (1 << 7) | reg_value;
    writeRegister(REG_OP_MODE, lora_mode);
}
void set_op_frequency(void)
{
    set_Mode(STDBY_MODE);
    writeRegister(REG_FR_MSB, LORA_FR_MSB);
    writeRegister(REG_FR_MID, LORA_FR_MID);
    writeRegister(REG_FR_LSB, LORA_FR_LSB);
}
void set_pa_config(void)
{
    set_Mode(STDBY_MODE);
    writeRegister(REG_PA_CONFIG, LORA_PA_CONFIG);
}
void set_lna(void)
{
    set_Mode(STDBY_MODE);
    writeRegister(REG_LNA, LORA_LNA);
}
void set_ocp(void)
{
    set_Mode(STDBY_MODE);
    writeRegister(REG_OCP, LORA_OCP);
}
void lora_reg_config(uint8_t addr, uint8_t value)
{
    set_Mode(STDBY_MODE);
    writeRegister(addr, value);
}

void reg_group_init(void)
{
    lora_reg_config(REG_FIFO_ADDR_PTR, LORA_FIFO_ADDR_PTR);
    lora_reg_config(REG_FIFO_TX_BASE_ADDR, LORA_FIFO_TX_BASE_ADDR);
    lora_reg_config(REG_FIFO_RX_BASE_ADDR, LORA_FIFO_RX_BASE_ADDR);
    lora_reg_config(REG_IRQ__FLAGS_MASK, LORA_IRQ_FLAGS_MASK);
    lora_reg_config(REG_MODEM_CONFIG1, LORA_MODEM_CONFIG1);
    lora_reg_config(REG_MODEM_CONFIG2, LORA_MODEM_CONFIG2);
    lora_reg_config(REG_SYMB_TIMEOUT_LSB, LORA_SYMB_TIMEOUT_LSB);
    lora_reg_config(REG_PREAMBLE_MSB, LORA_PREAMBLE_MSB);
    lora_reg_config(REG_PREAMBLE_LSB, LORA_PREAMBLE_LSB);
    lora_reg_config(REG_SYNC_WORD, LORA_SYNC_WORD);
}

void transmit(uint8_t data_buff[])
{
    set_Mode(STDBY_MODE);
    uint8_t fifo_tx_base_pointer = readRegister(REG_FIFO_TX_BASE_ADDR);
    writeRegister(REG_FIFO_ADDR_PTR, fifo_tx_base_pointer);
    for (uint8_t i = 0; i < 15; i++)
    {
        writeRegister(REG_FIFO, data_buff[i]);
    }
    writeRegister(REG_LORA_PAYLOAD_LENGTH, 15);
    set_Mode(TX_MODE);

    uint32_t start_time = millis();
    uint32_t timeout = 5000;

    while (1)
    {
        uint8_t tx_done_reg = readRegister(REG_IRQ_FLAGS);
        if (tx_done_reg & (1 << 3))
        {
            Serial.printf("\nTransmission completed!\n");
            writeRegister(REG_IRQ_FLAGS, (1 << 3));
            break;
        }
        else if (millis() - start_time >= timeout)
        {
            Serial.printf("\nPayload transmission failed!\n");
            break;
        }
    }
}
void receive(void)
{
    set_Mode(STDBY_MODE);
    uint8_t fifo_rx_base_pointer = readRegister(REG_FIFO_RX_BASE_ADDR);
    writeRegister(REG_FIFO_ADDR_PTR, fifo_rx_base_pointer);
    set_Mode(RX_CONT);

    uint32_t start_time = millis();
    uint32_t timeout = 20000;
    Serial.printf("Listening...");

    while (1)
    {
        uint8_t rx_done_reg = readRegister(REG_IRQ_FLAGS);
        if (rx_done_reg & (1 << 6))
        {
            set_Mode(STDBY_MODE);
            Serial.printf("Reception done!\n");
            writeRegister(REG_IRQ_FLAGS, (1 << 6));
            break;
        }
        else
        {
            if (millis() - start_time >= timeout)
            {
                Serial.printf("Timeout exceeded:exiting listening \n");
                set_Mode(STDBY_MODE);
                break;
            }
        }
    }
}
void extract_fifo_payload(uint8_t rx_buffer[])
{
    uint8_t no_bytes = readRegister(REG_RX_NB_BYTES);
    uint8_t rx_current_address = readRegister(REG_FIFO_RX_CURRENT_ADDR);
    writeRegister(REG_FIFO_ADDR_PTR, rx_current_address);
    for (uint8_t i = 0; i < no_bytes; i++)
    {
        rx_buffer[i] = readRegister(REG_FIFO);
    }
    Serial.printf("Extraction done:\n");
    for (uint8_t i = 0; i < no_bytes; i++)
    {
        Serial.printf("Sent payload:  %c\n", rx_buffer[i]);
    }
}
