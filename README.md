# LoRa Based mesh Network For Communiaction by Mountain climbers In Areas With Low Network Coverage

- This project implements a
decentralized ad hoc LoRa Mesh network that is used to provide a communication system that supports short messages sending between end users. Users can send a direct message to each other if they are reachable with a  single hop  OR are reachable via other intermediary nodes(supports routing). A node can dynamically search for a route to destination or help another node search for a route to destination. A BLE based messaging app serves as the user interface allowing users to type in messages.

- The repo contains bare-metal firmware and a flutter-based android app source code. The firmware stack consits of a SX1276 driver to handle LORA, a custom ad-hoc routing protocol and a BLE GATT server for BLE support.
  
- The app sends the message to an ESP32 microcontroller via BLE which connects to a RFM95W (has a sx1276 LoRa transceiver) via SPI, the RFM95W then sends the message via LoRa modulation. Below is the realized final product

<table align="center">
  <tr>
    <td align="center" valign="bottom">
      <a href="../LoRa/assets/chat_screen1.png">
        <img src="../LoRa/assets/chat_screen1.png" width="300" alt="chat screen1">
      <br>
      <sub><b>Chat screen of two nodes communicating</b></sub>
        </a>
    </td>

<td align="center" valign="bottom">
  <a href="../LoRa/assets/3_node_system.png">
    <img src="../LoRa/assets/3_node_system.png" width="300" alt="3 node system">
  <br>
  <sub><b>3-node system setup</b></sub></a>
</td>
</tr>
</table>

> Built as my final-year capstone project, Bsc Electronic & Computer Engineering, JKUAT (2026).

- The software architecture uses a layered approach that consists of five layers.
  
<table align="center">
  <tr>
    <td align="center" valign="bottom">
      <a href="../LoRa/assets/layered_architecture.png">
        <img src="../LoRa/assets/layered_architecture.png" width="300" alt="chat screen1">
      </a><br>
      <sub><b>System software architcture</b></sub>
    </td>

<td align="center" valign="bottom">
  <a href="../LoRa/assets/system_blockD.png">
    <img src="../LoRa/assets/system_blockD.png" width="300" alt="System Operation Overview">
  </a><br>
  <sub><b>System operation overview</b></sub>
</td>
</tr>
</table>

## Firmware architecture
  
1.**Physical layer** - Consists of the sx1276 driver that configures the LoRa chip via **direct register writing via SPI** and provides high level functions to upper layers to interact with the radio chip. The driver is written without reliance on third party LoRa libraries and follows the [Semtech sx1276 data sheet](https://www.mouser.com/datasheet/2/761/sx1276-1278113.pdf). The code for the driver is found in [sx1276 driver](src/SX1276/sx1276.cpp).

 The radio control layer implemented in [radio_layer](src/RADIO_CONTROL/radio_control) provides high level functions like sending payload, extracting payload from the lora chip and listening for incoming traffic.
  
2.**MAC layer** - Receives incoming messages from the radio layer and calls the necessary handler. Every message used in the system has a unique tag that indentifies and it is the function of the MAC layer to attach tags and indentify received payloads based on their tags so as to decide the appropriate handler for that type of message. For example, before  a RREQ(Route Request) message is sent, the MAC layer is the layer that appends a type to it so that other nodes that receive that RREQ can intelligently identify the message for subsequent processing. In the receiving scenario, it checks for the type of the message to know which handler to call depending on the message type that arrived. For detailed dive into the internal mechanics of the mac layer, click the link below.
 [MAC_layer.cpp](src/MAC_LAYER/mac_layer.cpp)

 3.**Network Layer** - Handles the routing mechanism. A routing protocol that is derived from AODV(Ad hoc On Demand Distance Vector) is used. On top of it a collision minimization mechanism by use of a handshake mechanism is implemented. RREQ,RREP,RTS,CTS and ACK are the routing control messges used.

- A RREQ message type is used to initiate route discovery by a node. Each node has a route table and on receiving a RREQ message checks its route table to verify if it has a route to the destination cached in its route table. If a route is found, a RREP(Rout Reply) is sent to the destination.

- A RTS(Request To Send) message is a message type sent by a node that has important payload to send. The purpose of this  network control message is to  reduce collisions by ensuring each node requests to use the channel before whenever it needs to perform transmission of payload.
  >It works by each node sending a RTS message that carries a destination address. Only the destination address can reply to the message. The destination address replies by sendng a CTS(Clear To Send) message to the sender of the RTS. The CTS messge is a command to other nodes that are not the senders of the RTS to backoff for a given time interval to give room for the RTS sender to transmit its payload to the node that replied with the CTS. This therefore reserves the channel to the two nodes to communicate without interference.

- [rfc3561](https://archive.org/details/rfc3561)- Was used as the refence manual for AODV - AODV is not implemented in its totality but rather, its routing contol messages are used with the exception of Route Error message type which was not used. For more details on the routing mechanism , click [aodv_layer.cpp](src/AODV_LORA/aodv_lora.cpp)

4.**Application layer** - This is divided into the BLE GATT client and BLE GATT servers and the main file which bundles everything together. BLE bridges communication between the app and the ESP32. BLE uses a GATT architecture to facilitate comunication between two devices using bluetooth. Think of the  GATT model as how bluetooth communication is conducted on a high level. The GATT model involves a GATT server and a GATT client. The GATT server is implemnted by a peripheral device that a central device can connect to. Think of a peripheral device as a device that advertises itself for other devices to connect to it , but it cannot initiate the connection. The GATT client makes the connection. The GATT model relies on services and characteristics. Services are a unique way to bundle related data or functionalities together. Within each service, there can be multiple characteristics which can be thought of as the actual data. Each service and characteristic has a unique indentifier. The GATT client can write , read or subscribe(receive notifications) to the characteristics defined by the GATT server. For more details visit [ble_server.cpp](src/BLE_GATT/ble_server.cpp).
  
- More details on the flutter app will be added soon.

## Hardware system

RF95W module. -  The RFM95W module comes packed with a SX1276 LoRa tranceiver. The chip operates at 868MHz ISM band for LoRa communication and supports other modes like OOK and FSK. The chip pads have a width of 2mm width and hence populary available 2.54 width  cannot be used directly. The chip was soldered on an Esp8266 expansion board to enable soldering of 2.54 header pins.

<table>
<tr>
<td><a  href = "assets/rfm95_front.png"><img src = "assets/rfm95_front.png" width = "200" alt =  "rfm95w front page"><br>
<sub><b>RFM95W front-face</b></sub></a></td>
<td><a href = "assets/rfm_back.png"><img src = "assets/rfm_back.png" width = "200" alt = "rfm95w back page"><br>
<sub><b>RFM95W back-face</b></sub></a> </td>
<td><a href = "assets/8266_adapter.png" ><img src = "assets/8266_adapter.png" width = "200" alt = "8266"><br>
<sub><b>ESP8266 expansion board</b></sub></a></td>
</tr>
</table>

- I used a simple quaterwave monopole antenna made from copper wire. For 868Mhz the length of the antenna was calculated to 8.6 cm .  This approach has drawback in that it is not optimized for range due to lack of proper impedance matching and inefficient ground plane reference. Howerver it was sufficient to demostrate the system functionality.
- An ESP32S-EU model was used as the microcontroler to run the Bluetooth and routing functionalities.
- The power supply system consisted of two 4.5 Lithium ion batteries  and an LM2596 Buck converter to provide a regulated 3.3V voltage which provides both power supply to the RFM95W and the ESp32. SPI connection, interrupt pin and power supply connections weremade using a perfboard for faster prototyping.


<table>
<tr>
<td><a  href = "assets/2nodesOnperfboard.png"><img src = "assets/2nodesOnperfboard.png" width = "500" alt =  "rfm95w front page"><br>
<sub><b>ESP32 + RFM95W </b></sub></td></a>
<td><a href = "assets/singleimage.png"><img src = "assets/singleimage.png" width = "300" alt = "rfm95w back page"><br>
<sub><b>Single node </b></sub></a> </td>

</tr>
</table>


## Author
Charles Maina 
Interested in embedded systems, and core low-level engineering(From semiconductor physics to the OS layer).
