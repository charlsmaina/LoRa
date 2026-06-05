import 'package:flutter/material.dart';
import 'ble_service.dart';

class ChatScreen extends StatefulWidget {
  const ChatScreen({super.key});

  @override
  State<ChatScreen> createState() => _ChatScreenState();
}

class _ChatScreenState extends State<ChatScreen> {
  final BleService _ble = BleService();
  final TextEditingController _controller = TextEditingController();
  final List<String> _messages = [];
  bool _connected = false;

  @override
  void initState() {
    super.initState();
    _ble.onMessageReceived = (message) {
      setState(() {
        _messages.add("mesh: $message");
      });
    };
  }

  Future<void> _connect() async {
    await _ble.connect();
    setState(() {
      _connected = true;
    });
  }

  Future<void> _send() async {
    String text = _controller.text.trim();
    if (text.isEmpty) return;
    await _ble.sendMessage(text);
    setState(() {
      _messages.add("me: $text");
      _controller.clear();
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(_connected ? "LoRa Mesh" : "Not Connected"),
        actions: [IconButton(icon: Icon(Icons.bluetooth), onPressed: _connect)],
      ),
      body: Column(
        children: [
          Expanded(
            child: ListView.builder(
              itemCount: _messages.length,
              itemBuilder: (context, index) {
                return ListTile(title: Text(_messages[index]));
              },
            ),
          ),
          Padding(
            padding: EdgeInsets.all(8.0),
            child: Row(
              children: [
                Expanded(
                  child: TextField(
                    controller: _controller,
                    decoration: InputDecoration(
                      hintText: "Type a message",
                      border: OutlineInputBorder(),
                    ),
                  ),
                ),
                SizedBox(width: 8),
                ElevatedButton(
                  onPressed: _connected ? _send : null,
                  child: Text("Send"),
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}
