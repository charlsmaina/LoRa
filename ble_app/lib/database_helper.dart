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
      version: 2, // bumped from 1 to 2 — added status column
      onCreate: (db, version) async {
        await db.execute('''
          CREATE TABLE messages (
            id      INTEGER PRIMARY KEY AUTOINCREMENT,
            node_id TEXT    NOT NULL,
            text    TEXT    NOT NULL,
            is_me   INTEGER NOT NULL,
            time    INTEGER NOT NULL,
            status  INTEGER NOT NULL DEFAULT 0
          )
        ''');
        // status: 0 = sent, 1 = delivered
      },
      onUpgrade: (db, oldVersion, newVersion) async {
        if (oldVersion < 2) {
          // existing installs get the new column with default 0
          await db.execute(
            'ALTER TABLE messages ADD COLUMN status INTEGER NOT NULL DEFAULT 0',
          );
        }
      },
    );
  }

  Future<int> insertMessage({
    required String nodeId,
    required String text,
    required bool isMe,
    required DateTime time,
  }) async {
    final db = await database;
    // returns the row id — used to mark delivered later
    return await db.insert('messages', {
      'node_id': nodeId,
      'text': text,
      'is_me': isMe ? 1 : 0,
      'time': time.millisecondsSinceEpoch,
      'status': 0,
    });
  }

  // Mark the most recently sent message to a node as delivered
  Future<void> markLastDelivered(String nodeId) async {
    final db = await database;

    // find the most recent outgoing message to this node
    final rows = await db.query(
      'messages',
      where: 'node_id = ? AND is_me = 1',
      whereArgs: [nodeId],
      orderBy: 'time DESC',
      limit: 1,
    );

    if (rows.isEmpty) return;

    final id = rows.first['id'] as int;

    await db.update(
      'messages',
      {'status': 1},
      where: 'id = ?',
      whereArgs: [id],
    );
  }

  Future<List<Map<String, dynamic>>> loadMessages(String nodeId) async {
    final db = await database;
    return await db.query(
      'messages',
      where: 'node_id = ?',
      whereArgs: [nodeId],
      orderBy: 'time ASC',
    );
  }

  Future<void> clearMessages(String nodeId) async {
    final db = await database;
    await db.delete('messages', where: 'node_id = ?', whereArgs: [nodeId]);
  }
}
