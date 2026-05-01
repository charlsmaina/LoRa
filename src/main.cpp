

#include <Arduino.h>
#include "../include/lora/tests.h"
#include "../include/lora/lora_config.h"
#include "../include/lora/lora_registers.h"
#include <SPI.h>
#include "tests/AODV_LORA/aodv_control.h"
#include "../include/lora/pin_config.h"
uint8_t extraction_buffer[100];
uint8_t trasmitter_buffer[100];

void IRAM_ATTR on_dio0_rise()
{
  Event e;
  e.type = RX_COMPLETE;
  enqueu(e);
}
void IRAM_ATTR there_is_payload_to_send()
{
  Event e;
  e.type = TX_READY;
  enqueu(e);
}

void setup()
{
  /*senders only for now*/
  uint8_t x = 77;
  for (uint8_t i = 0; i < 100; i++)
  {
    trasmitter_buffer[i] = x;
  }

  // digitalWrite(PIN_BLE_PROXY, HIGH); /*Sender*/

  Serial.begin(115200);

  if (lora_spi_init())
    Serial.printf("Initialization successfull!\nSPI OK!\n");

  else
    Serial.printf("Initialization failed!\nSPI not working!!\n");

  set_lora_mode();
  set_op_frequency();
  set_pa_config();
  reg_group_init();

  interrupts_pins_setup();
  set_Mode(RX_CONT);
  delay(1000);
  receive();

  /*
  ---------------------TX simulation-------------------------
   pinMode(PIN_BLE_PROXY, INPUT);
    digitalWrite(PIN_BLE_PROXY, LOW);
  */
}

void loop()
{

  if (!queu_is_empty())
  {
    Event e = dequeu();
    switch (e.type)
    {
    case RX_COMPLETE:
      printf("Extraction requirement detected:\n");
      extract_fifo_payload(extraction_buffer);
      set_Mode(RX_CONT);
      receive();

      break;
    case TX_READY:
      transmit(trasmitter_buffer);
      printf("Transmission requirement detected:\n");

      break;
    }
  }
  delay(1000);
}
