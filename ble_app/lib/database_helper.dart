import 'package:sqflite/sqflite.dart';
import 'package:path/path.dart';

class DatabaseHelper {
  static final DatabaseHelper instance = DatabaseHelper._internal();
  static Database? _database;

  DatabaseHelper._internal();

  Future<Database> get database async {
    if (_database != null) return _database!;
    _database = await _initDatabase();
    return _database!;
  }

  Future<Database> _initDatabase() async {
    final path = join(await getDatabasesPath(), 'mesh_chat.db');
    return await openDatabase(
      path,
      version: 1,
      onCreate: (db, version) async {
        await db.execute('''
          CREATE TABLE messages (
            id      INTEGER PRIMARY KEY AUTOINCREMENT,
            node_id TEXT    NOT NULL,
            text    TEXT    NOT NULL,
            is_me   INTEGER NOT NULL,
            time    INTEGER NOT NULL
          )
        ''');
      },
    );
  }

  // Insert one message for a given node
  Future<void> insertMessage({
    required String nodeId,
    required String text,
    required bool isMe,
    required DateTime time,
  }) async {
    final db = await database;
    await db.insert('messages', {
      'node_id': nodeId,
      'text': text,
      'is_me': isMe ? 1 : 0, // SQLite has no bool — use 1/0
      'time': time.millisecondsSinceEpoch,
    });
  }

  // Load all messages for a node, oldest first
  Future<List<Map<String, dynamic>>> loadMessages(String nodeId) async {
    final db = await database;
    return await db.query(
      'messages',
      where: 'node_id = ?',
      whereArgs: [nodeId],
      orderBy: 'time ASC',
    );
  }

  // Clear chat history for a node
  Future<void> clearMessages(String nodeId) async {
    final db = await database;
    await db.delete('messages', where: 'node_id = ?', whereArgs: [nodeId]);
  }
}
