import 'package:flutter/material.dart';
import 'ble_service.dart';
import 'database_helper.dart';
import 'chat_screen.dart';

const List<Map<String, String>> kNodes = [
  {'name': 'Base Camp', 'id': '0x21'},
  {'name': 'Climber 1', 'id': '0x22'},
  {'name': 'Climber 2', 'id': '0x23'},
];

class HomeScreen extends StatefulWidget {
  const HomeScreen({super.key});

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  final BleService _ble = BleService();
  bool _connected = false;
  bool _connecting = false;
  String _connectedId = '';

  // unread count per node id — increments when message arrives
  // and that chat screen is not open
  final Map<String, int> _unread = {'0x21': 0, '0x22': 0, '0x23': 0};

  @override
  void initState() {
    super.initState();

    _ble.onConnected = (nodeId) {
      setState(() {
        _connected = true;
        _connecting = false;
        _connectedId = nodeId;
      });
    };

    _ble.onDisconnected = () {
      setState(() {
        _connected = false;
        _connectedId = '';
      });
    };

    // Background handler — fires when a message arrives for a node
    // whose chat screen is not currently open
    // Persists to SQLite and increments unread badge
    _ble.registerHandler('background', (raw) async {
      final colonIndex = raw.indexOf(':');
      if (colonIndex == -1) return;

      final nodeId = raw.substring(0, colonIndex);
      final message = raw.substring(colonIndex + 1);

      // persist even though screen is closed
      await DatabaseHelper.instance.insertMessage(
        nodeId: nodeId,
        text: message,
        isMe: false,
        time: DateTime.now(),
      );

      // increment unread badge for this node
      if (_unread.containsKey(nodeId)) {
        setState(() {
          _unread[nodeId] = (_unread[nodeId] ?? 0) + 1;
        });
      }
    });
  }

  @override
  void dispose() {
    _ble.unregisterHandler('background');
    _ble.dispose();
    super.dispose();
  }

  Future<void> _connect() async {
    setState(() {
      _connecting = true;
    });
    try {
      await _ble.connect();
    } catch (e) {
      setState(() {
        _connecting = false;
      });
      if (mounted) {
        ScaffoldMessenger.of(
          context,
        ).showSnackBar(SnackBar(content: Text('$e')));
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: const Color(0xFF1A1A2E),
      appBar: AppBar(
        backgroundColor: const Color(0xFF16213E),
        title: const Text(
          'LoRa Mesh',
          style: TextStyle(color: Colors.white, fontWeight: FontWeight.w600),
        ),
        actions: [
          if (_connecting)
            const Padding(
              padding: EdgeInsets.symmetric(horizontal: 16, vertical: 14),
              child: SizedBox(
                width: 20,
                height: 20,
                child: CircularProgressIndicator(
                  strokeWidth: 2,
                  color: Colors.tealAccent,
                ),
              ),
            )
          else if (_connected)
            Padding(
              padding: const EdgeInsets.only(right: 12),
              child: Chip(
                label: Text(
                  _connectedId,
                  style: const TextStyle(fontSize: 11, color: Colors.white),
                ),
                backgroundColor: Colors.teal.shade700,
                padding: EdgeInsets.zero,
              ),
            )
          else
            IconButton(
              icon: const Icon(Icons.bluetooth, color: Colors.white70),
              tooltip: 'Connect to node',
              onPressed: _connect,
            ),
        ],
      ),
      body: ListView.separated(
        padding: const EdgeInsets.all(16),
        itemCount: kNodes.length,
        separatorBuilder: (_, __) => const SizedBox(height: 8),
        itemBuilder: (context, index) {
          final node = kNodes[index];
          return _buildNodeTile(node['name']!, node['id']!);
        },
      ),
    );
  }

  Widget _buildNodeTile(String name, String id) {
    final unreadCount = _unread[id] ?? 0;

    return GestureDetector(
      onTap: () async {
        // clear unread badge when opening the chat
        setState(() {
          _unread[id] = 0;
        });

        await Navigator.push(
          context,
          MaterialPageRoute(
            builder: (_) =>
                _ChatScreenWrapper(nodeName: name, nodeId: id, ble: _ble),
          ),
        );
      },
      child: Container(
        padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 14),
        decoration: BoxDecoration(
          color: const Color(0xFF16213E),
          borderRadius: BorderRadius.circular(12),
        ),
        child: Row(
          children: [
            Stack(
              children: [
                CircleAvatar(
                  radius: 22,
                  backgroundColor: Colors.teal.shade700,
                  child: Text(
                    name[0],
                    style: const TextStyle(
                      color: Colors.white,
                      fontWeight: FontWeight.w600,
                    ),
                  ),
                ),
                // unread badge — only visible when count > 0
                if (unreadCount > 0)
                  Positioned(
                    right: 0,
                    top: 0,
                    child: Container(
                      padding: const EdgeInsets.all(4),
                      decoration: const BoxDecoration(
                        color: Colors.tealAccent,
                        shape: BoxShape.circle,
                      ),
                      child: Text(
                        '$unreadCount',
                        style: const TextStyle(
                          fontSize: 10,
                          color: Colors.black,
                          fontWeight: FontWeight.bold,
                        ),
                      ),
                    ),
                  ),
              ],
            ),
            const SizedBox(width: 14),
            Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  name,
                  style: const TextStyle(
                    color: Colors.white,
                    fontSize: 15,
                    fontWeight: FontWeight.w500,
                  ),
                ),
                const SizedBox(height: 2),
                Text(
                  id,
                  style: const TextStyle(color: Colors.white38, fontSize: 12),
                ),
              ],
            ),
            const Spacer(),
            const Icon(Icons.chevron_right, color: Colors.white24),
          ],
        ),
      ),
    );
  }
}

// Thin wrapper that registers/unregisters the chat screen handler
// with BleService when the screen opens and closes
class _ChatScreenWrapper extends StatefulWidget {
  final String nodeName;
  final String nodeId;
  final BleService ble;

  const _ChatScreenWrapper({
    required this.nodeName,
    required this.nodeId,
    required this.ble,
  });

  @override
  State<_ChatScreenWrapper> createState() => _ChatScreenWrapperState();
}

class _ChatScreenWrapperState extends State<_ChatScreenWrapper> {
  @override
  void dispose() {
    // unregister when screen closes — background handler takes over
    widget.ble.unregisterHandler(widget.nodeId);
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return ChatScreen(
      nodeName: widget.nodeName,
      nodeId: widget.nodeId,
      ble: widget.ble,
    );
  }
}
