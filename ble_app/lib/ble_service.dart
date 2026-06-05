import 'package:flutter_blue_plus/flutter_blue_plus.dart'; /* Include flutter blue plus libraly*/

class BleService {
  BluetoothDevice? _device; /*ESP*/
  BluetoothCharacteristic? _txChar;
  BluetoothCharacteristic? _rxChar;

  final String targetName = "LORA_NODE_A";
  final String serviceUuid = "12345678-1234-1234-1234-123456789abc";
  final String txUuid = "12345678-1234-1234-1234-123456789abd";
  final String rxUuid = "12345678-1234-1234-1234-123456789abe";

  // called when a message arrives from the mesh
  Function(String message)? onMessageReceived; /* Declares a function type* */

  Future<void> connect() async {
    print("Starting scan...");
    await FlutterBluePlus.startScan(timeout: Duration(seconds: 5));

    FlutterBluePlus.scanResults.listen((results) async {
      print("Scan results: ${results.length} devices found");
      for (ScanResult r in results) {
        print("Found device: ${r.device.platformName}");
        if (r.device.platformName == targetName) {
          print("Target found — connecting...");
          await FlutterBluePlus.stopScan();
          _device = r.device;
          await _device!.connect(autoConnect: false);
          await _discoverServices();
          break;
        }
      }
    });
  }

  Future<void> _discoverServices() async {
    List<BluetoothService> services = await _device!.discoverServices();

    for (BluetoothService service in services) {
      if (service.serviceUuid.toString() == serviceUuid) {
        for (BluetoothCharacteristic c in service.characteristics) {
          if (c.characteristicUuid.toString() == txUuid) {
            _txChar = c;
          }
          if (c.characteristicUuid.toString() == rxUuid) {
            _rxChar = c;
            await _subscribeToRx();
          }
        }
      }
    }
  }

  Future<void> _subscribeToRx() async {
    await _rxChar!.setNotifyValue(true);
    _rxChar!.lastValueStream.listen((value) {
      if (value.isNotEmpty) {
        String message = String.fromCharCodes(value);
        if (onMessageReceived != null) {
          onMessageReceived!(message);
        }
      }
    });
  }

  Future<void> sendMessage(String message) async {
    if (_txChar == null) return;
    List<int> bytes = message.codeUnits;
    await _txChar!.write(bytes, withoutResponse: false);
  }
}
