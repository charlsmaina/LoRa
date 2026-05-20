import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return const MaterialApp(home: ScanPage());
  }
}

class ScanPage extends StatefulWidget {
  const ScanPage({super.key});

  @override
  State<ScanPage> createState() => _ScanPageState();
}

class _ScanPageState extends State<ScanPage> {
  // holds the scan results we find
  final List<ScanResult> _results = [];

  void _startScan() {
    _results.clear();

    // listen to scan results as they arrive
    FlutterBluePlus.scanResults.listen((results) {
      setState(() {
        _results.clear();
        _results.addAll(results);
      });
    });

    // scan for 5 seconds
    FlutterBluePlus.startScan(timeout: const Duration(seconds: 5));
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('BLE Scan')),
      body: ListView.builder(
        itemCount: _results.length,
        itemBuilder: (context, index) {
          final device = _results[index].device;
          return ListTile(
            title: Text(
              device.platformName.isEmpty ? '(no name)' : device.platformName,
            ),
            subtitle: Text(device.remoteId.toString()),
          );
        },
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _startScan,
        child: const Icon(Icons.search),
      ),
    );
  }
}
