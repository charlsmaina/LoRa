
Tasks:
1. Hardware validation 
	- Solder and inspect 3 ESP32 + RFM95W
	- Flash basic LoRa ping-pong sketch on each pair
	- Confirm TX/RX on all three nodes - measure RSSI , verify range is reasonable
	- Confirm BLE advertisement is visible from a phone
	- Ensure all nodes receive and send LoRa packets reliably
	- Metric: All three nodes can send and receive LoRa packets reliably
  2. AODV Mesh firmware
    - Define the packet structure : RREQ,RREP,RERR
    - Do flooding fast
    - Implement route discovery through an intermediate node
    - Implement a routing table and RERR + route repair
    - Metric: A message typed at node A, reliably reaches Node C via node B, with rerouting when B goes down.
    
   3. BLE GATT server 
	   - Each node exposes a a GATT service to the flutter app to send or receive messages over BLE
	   - Set up a NimBLE GATT server on each ESP32
	   - Define characteristics, one for TX(app-node), one for RX notifications (node -app)
	   - Test with generic BLE scanner(nRf Connect)
	   - Metric : Write a string on the TX characteristic and read it back on the RX characteristic
4. Flutter BLE client
    - Scan and connect to a nearby node
    - Send a text message via the TX characteristic
    - Display an incoming message form the RX notification
    - Basic UI : message list + text input field
    Metric : App connects, sends hello message, message transverses the mesh, arrives at destination node's BLE client
5. End to end integration + testing
	- Run all three nodes simultaneously with 2 phones connected to node A and node C
	- Send messages both way, verify delivery
	- Stress test : verify RERR and rerouting
	- Measure and record deliverable success rate, latency , RSSI values
	- Document results
