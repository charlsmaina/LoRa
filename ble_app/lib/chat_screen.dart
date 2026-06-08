import 'package:flutter/material.dart';
import 'ble_service.dart';
import 'database_helper.dart';

class _Message {
  final String text;
  final bool isMe;
  final DateTime time;
  _Message({required this.text, required this.isMe, required this.time});
}

class ChatScreen extends StatefulWidget {
  final String nodeName;
  final String nodeId;
  final BleService ble;

  const ChatScreen({
    super.key,
    required this.nodeName,
    required this.nodeId,
    required this.ble,
  });

  @override
  State<ChatScreen> createState() => _ChatScreenState();
}

class _ChatScreenState extends State<ChatScreen> {
  final TextEditingController _controller = TextEditingController();
  final ScrollController _scrollController = ScrollController();
  final List<_Message> _messages = [];

  @override
  @override
  void initState() {
    super.initState();
    _loadHistory();

    // register this screen for its node id
    // BleService will route incoming messages here when this screen is open
    widget.ble.registerHandler(widget.nodeId, (message) {
      final msg = _Message(text: message, isMe: false, time: DateTime.now());
      DatabaseHelper.instance.insertMessage(
        nodeId: widget.nodeId,
        text: message,
        isMe: false,
        time: msg.time,
      );
      setState(() {
        _messages.add(msg);
      });
      _scrollToBottom();
    });
  }

  Future<void> _loadHistory() async {
    final rows = await DatabaseHelper.instance.loadMessages(widget.nodeId);
    setState(() {
      _messages.addAll(
        rows.map(
          (row) => _Message(
            text: row['text'] as String,
            isMe: (row['is_me'] as int) == 1,
            time: DateTime.fromMillisecondsSinceEpoch(row['time'] as int),
          ),
        ),
      );
    });
    _scrollToBottom();
  }

  @override
  @override
  void dispose() {
    widget.ble.unregisterHandler(widget.nodeId);
    _controller.dispose();
    _scrollController.dispose();
    super.dispose();
  }

  Future<void> _send() async {
    final text = _controller.text.trim();
    if (text.isEmpty) return;

    final msg = _Message(text: text, isMe: true, time: DateTime.now());

    await DatabaseHelper.instance.insertMessage(
      nodeId: widget.nodeId,
      text: text,
      isMe: true,
      time: msg.time,
    );

    // destination implicit from which chat screen we are in
    // user never types it — appended silently as "0x21:hello"
    await widget.ble.sendMessage(widget.nodeId, text);

    setState(() {
      _messages.add(msg);
      _controller.clear();
    });
    _scrollToBottom();
  }

  void _scrollToBottom() {
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (_scrollController.hasClients) {
        _scrollController.animateTo(
          _scrollController.position.maxScrollExtent,
          duration: const Duration(milliseconds: 250),
          curve: Curves.easeOut,
        );
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: const Color(0xFF1A1A2E),
      appBar: AppBar(
        backgroundColor: const Color(0xFF16213E),
        title: Row(
          children: [
            CircleAvatar(
              radius: 16,
              backgroundColor: Colors.teal.shade700,
              child: Text(
                widget.nodeName[0],
                style: const TextStyle(
                  fontSize: 13,
                  fontWeight: FontWeight.w600,
                  color: Colors.white,
                ),
              ),
            ),
            const SizedBox(width: 10),
            Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  widget.nodeName,
                  style: const TextStyle(fontSize: 15, color: Colors.white),
                ),
                Row(
                  children: [
                    Container(
                      width: 6,
                      height: 6,
                      decoration: const BoxDecoration(
                        color: Colors.tealAccent,
                        shape: BoxShape.circle,
                      ),
                    ),
                    const SizedBox(width: 4),
                    Text(
                      widget.nodeId,
                      style: const TextStyle(
                        fontSize: 11,
                        color: Colors.tealAccent,
                      ),
                    ),
                  ],
                ),
              ],
            ),
          ],
        ),
        actions: [
          IconButton(
            icon: const Icon(Icons.delete_outline, color: Colors.white54),
            onPressed: () async {
              await DatabaseHelper.instance.clearMessages(widget.nodeId);
              setState(() {
                _messages.clear();
              });
            },
          ),
        ],
      ),
      body: Column(
        children: [
          Expanded(
            child: ListView.builder(
              controller: _scrollController,
              padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
              itemCount: _messages.length,
              itemBuilder: (context, index) => _buildBubble(_messages[index]),
            ),
          ),
          _buildInputRow(),
        ],
      ),
    );
  }

  Widget _buildBubble(_Message msg) {
    final timeStr =
        '${msg.time.hour.toString().padLeft(2, '0')}:'
        '${msg.time.minute.toString().padLeft(2, '0')}';

    return Align(
      alignment: msg.isMe ? Alignment.centerRight : Alignment.centerLeft,
      child: Container(
        margin: const EdgeInsets.symmetric(vertical: 4),
        padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
        constraints: BoxConstraints(
          maxWidth: MediaQuery.of(context).size.width * 0.72,
        ),
        decoration: BoxDecoration(
          color: msg.isMe ? const Color(0xFF1D9E75) : const Color(0xFF2A2A3E),
          borderRadius: BorderRadius.only(
            topLeft: const Radius.circular(16),
            topRight: const Radius.circular(16),
            bottomLeft: Radius.circular(msg.isMe ? 16 : 4),
            bottomRight: Radius.circular(msg.isMe ? 4 : 16),
          ),
        ),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.end,
          children: [
            Text(
              msg.text,
              style: TextStyle(
                color: msg.isMe ? Colors.white : Colors.white70,
                fontSize: 14,
                height: 1.4,
              ),
            ),
            const SizedBox(height: 3),
            Text(
              timeStr,
              style: TextStyle(
                fontSize: 10,
                color: msg.isMe ? Colors.white54 : Colors.white38,
              ),
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildInputRow() {
    return Container(
      padding: const EdgeInsets.all(10),
      color: const Color(0xFF16213E),
      child: Row(
        children: [
          Expanded(
            child: TextField(
              controller: _controller,
              style: const TextStyle(color: Colors.white),
              textCapitalization: TextCapitalization.sentences,
              decoration: InputDecoration(
                hintText: 'Type a message…',
                hintStyle: const TextStyle(color: Colors.white38),
                filled: true,
                fillColor: const Color(0xFF2A2A3E),
                border: OutlineInputBorder(
                  borderRadius: BorderRadius.circular(24),
                  borderSide: BorderSide.none,
                ),
                contentPadding: const EdgeInsets.symmetric(
                  horizontal: 16,
                  vertical: 10,
                ),
              ),
              onSubmitted: (_) => _send(),
            ),
          ),
          const SizedBox(width: 8),
          GestureDetector(
            onTap: _send,
            child: Container(
              width: 44,
              height: 44,
              decoration: const BoxDecoration(
                color: Color(0xFF1D9E75),
                shape: BoxShape.circle,
              ),
              child: const Icon(
                Icons.send_rounded,
                color: Colors.white,
                size: 20,
              ),
            ),
          ),
        ],
      ),
    );
  }
}
