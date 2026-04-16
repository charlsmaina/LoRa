/*
----------------------------------------------------------------------
This file contain a collection of sx1276 registers addresses
----------------------------------------------------------------------
*/
#ifndef LORA_REGISTERS_H
#define LORA_REGISTERS_H
#define REG_FIFO 0x00                 /*FIFO read/write access*/
#define REG_OP_MODE 0x01              /*Operating mode selection: LoRa or FSK*/
#define REG_FR_MSB 0x06               /*RF carrier frequency most significant byte*/
#define REG_FR_MID 0x07               /*RF carrier frequency intermidiate byte*/
#define REG_FR_LSB 0x08               /*RF carrier least significant byte*/
#define REG_PA_CONFIG 0x09            /*Power Aplifier selection and Output power control*/
#define REG_LNA 0x0C                  /*LNA settings*/
#define REG_FIFO_ADDR_PTR 0x0D        /*FiFo SPI pointer*/
#define REG_FIFO_TX_BASE_ADDR 0x0E    /*Start of TX data*/
#define REG_FIFO_RX_BASE_ADDR 0x0F    /*Start of RX data */
#define REG_FIFO_RX_CURRENT_ADDR 0x10 /*Start address of last packet received*/
#define REG_IRQ__FLAGS_MASK 0x11      /*Optional IRQ flag mask*/
#define REG_IRQ_FLAGS 0x12            /*Interrupt requests flags*/
#define REG_RX_NB_BYTES 0x13          /*Number of received bytes*/
#define REG_PKT_SNR_VALUE 0x1A        /*Approximation of last packet SNR*/
#define REG_PKT_RSSI_VALUE 0x1B       /*Current RSSI*/
#define REG_MODEM_CONFIG1 0x1D        /*Modem PHY config 1*/
#define REG_MODEM_CONFIG2 0x1E        /*Modem PHY config 2*/
#define REG_PREAMBLE_MSB 0x20         /*Size of preamble*/
#define REG_PREAMBLE_LSB 0x21
#define REG_LORA_PAYLOAD_LENGTH 0x22    /*LoRa payload length */
#define REG_MAXIMUM_PAYLOAD_LENGTH 0x23 /*MAximum allowed payload*/
#define REG_FIFO_TX_BYTE_ADDR 0x25      /*address of last byte written into FIFO*/
#define REG_MODEM_CONFIG3 0x26          /*Modem PHY config 3*/
#define REG_SYNC_WORD 0x39              /*LoRa Sync Word*/
#define REG_DIO_MAPPING1 0x40           /*Mappings of DI00 to DI003*/
#define REG_DIO_MAPPING2 0x41           /*Mappings of DIO4 to DIO5,CLOCK frequency*/
#define REG_REG_VERSION 0x42            /*Semtech ID relating the silicon revision*/
#define REG_PA_DAC 0x4D
#define REG_OCP 0x0B
#define REG_SYMB_TIMEOUT_LSB 0x1F
#endif
