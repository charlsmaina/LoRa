import 'dart:async';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';

class BleService {
  BluetoothDevice? _device;
  BluetoothCharacteristic? _txChar;
  BluetoothCharacteristic? _rxChar;

  final String targetName = "LORA_NODE_A";
  final String serviceUuid = "12345678-1234-1234-1234-123456789abc";
  final String txUuid = "12345678-1234-1234-1234-123456789abd";
  final String rxUuid = "12345678-1234-1234-1234-123456789abe";

  Function()? onDisconnected;
  Function(String nodeId)? onConnected;

  // Map of callbacks keyed by node id — each chat screen registers itself
  // "0x21" → Base Camp handler, "0x22" → Climber 1 handler etc.
  final Map<String, Function(String message)> _messageHandlers = {};

  StreamSubscription? _disconnectSub;

  // Chat screen calls this when it opens
  void registerHandler(String nodeId, Function(String message) handler) {
    _messageHandlers[nodeId] = handler;
  }

  // Chat screen calls this when it closes
  void unregisterHandler(String nodeId) {
    _messageHandlers.remove(nodeId);
  }

  Future<void> connect() async {
    final completer = Completer<void>();

    await FlutterBluePlus.startScan(timeout: const Duration(seconds: 5));

    final scanSub = FlutterBluePlus.scanResults.listen((results) async {
      for (ScanResult r in results) {
        if (r.device.platformName == targetName && !completer.isCompleted) {
          await FlutterBluePlus.stopScan();
          _device = r.device;
          await _device!.connect(autoConnect: false);
          await _discoverServices();
          _listenForDisconnect();
          onConnected?.call(targetName);
          completer.complete();
          break;
        }
      }
    });

    await completer.future.timeout(
      const Duration(seconds: 8),
      onTimeout: () {
        scanSub.cancel();
        throw Exception("Device not found — is the node powered on?");
      },
    );

    await scanSub.cancel();
  }

  Future<void> _discoverServices() async {
    final services = await _device!.discoverServices();
    for (BluetoothService service in services) {
      if (service.serviceUuid.toString() == serviceUuid) {
        for (BluetoothCharacteristic c in service.characteristics) {
          if (c.characteristicUuid.toString() == txUuid) _txChar = c;
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
        // incoming bytes are "0x21:Hello"
        // convert to string and parse the prefix
        final raw = String.fromCharCodes(value);
        _routeIncoming(raw);
      }
    });
  }

  void _routeIncoming(String raw) {
    // find the first colon — same parsing as firmware does on send path
    final colonIndex = raw.indexOf(':');
    if (colonIndex == -1) return; // malformed, drop

    final nodeId = raw.substring(0, colonIndex); // "0x21"
    final message = raw.substring(colonIndex + 1); // "Hello"

    if (message.isEmpty) return;

    // look up the registered handler for this node id
    final handler = _messageHandlers[nodeId];

    if (handler != null) {
      // chat screen is open — deliver directly
      handler(message);
    } else {
      // chat screen is not open — message still gets persisted
      // by the home screen's background handler
      _messageHandlers['background']?.call(raw);
    }
  }

  void _listenForDisconnect() {
    _disconnectSub = _device!.connectionState.listen((state) {
      if (state == BluetoothConnectionState.disconnected) {
        _txChar = null;
        _rxChar = null;
        onDisconnected?.call();
      }
    });
  }

  // destination implicit from which chat screen sends
  // format: "0x21:Hello" — firmware splits on first colon
  Future<void> sendMessage(String destination, String message) async {
    if (_txChar == null) return;
    final payload = '$destination:$message';
    await _txChar!.write(payload.codeUnits, withoutResponse: false);
  }

  void dispose() {
    _disconnectSub?.cancel();
    _device?.disconnect();
  }
}
