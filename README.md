# Overview

- This repo oulines the implemntation process of a decentralized ad hoc LoRa Mesh network that is used to provide a communication system that supports short messages sending between end users. Users can send a direct message to each other if they are reachable with a  single one hop direct link OR are reachable via other intermediary nodes. A node can dynamically search for a route to destination or help another node search for a route to destination. A BLE based messaging app serves as the user interface allowing users to type in messages.
  
- The app sends the message to an ESP32 microcontroller via BLE which connects to a RFM95W (has a sx1276 LoRa tranceiver) via SPI, the RFM95W then sends the message via LoRa modulation.
  
- The software architecture uses a layered approach that consists of five layers namely:
  
 1. **Physical layer** - Consists of the sx1276 driver that configures the LoRa chip and provides high level functions to upper layer to interact with the radio chip. The driver is written without reliance on third party LoRa libralies and follows the [Semtech sx1276 data sheet](https://www.mouser.com/datasheet/2/761/sx1276-1278113.pdf). The code for the driver is found in src/SX1276/sx1276.cpp . The radio control layer found in src/RADIO_CONTROL/radio_control.cpp implements high level functions like sending payload, extracting payload from the lora chip and listening for incoming traffic. For more detailed dive into the working mechanics of each file. Click the links below.
     - [sx1276.cpp](google.com)
     - [radio_control.cpp](www.gooogle.com)
  
2. **MAC LAYER** - Receives incoming messages from the radio layer and calls the necessary handler. It also frames message to different types before sending. For example, before  a RREQ(Route Request) message is sent, the MAC layer is the layer that appends a type to it so that other nodes that receive that RREQ can intelligently indentify the message for subsequent processing. In the receiving scenario, it checks for the type of the message to know which handler to call depending on the message type that arrived. For detailed dive into the internal mechanics of the mac layer, click the link below.
3. **NETWORK_LAYER** - Im