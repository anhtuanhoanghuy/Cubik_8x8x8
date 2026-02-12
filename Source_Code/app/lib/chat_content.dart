import 'package:flutter/material.dart';
import 'chat_detail_screen.dart';
import 'model/friend_card.dart';


class ChatContent extends StatelessWidget {
  const ChatContent({super.key});

  @override
  Widget build(BuildContext context) {
    final friends = [
      {'name': 'Nguyễn Văn A', 'emoji': '👨', 'isOnline': true},
      {'name': 'Trần Thị B', 'emoji': '👩', 'isOnline': true},
      {'name': 'Lê Văn C', 'emoji': '👨‍💼', 'isOnline': false},
      {'name': 'Phạm Thị D', 'emoji': '👩‍🦱', 'isOnline': true},
      {'name': 'Hoàng Văn E', 'emoji': '🧑‍💻', 'isOnline': false},
    ];

    return SingleChildScrollView(
      child: Padding(
        padding: const EdgeInsets.all(20.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Text('Bạn bè', style: TextStyle(fontSize: 24, fontWeight: FontWeight.bold, color: Colors.white)),
            const SizedBox(height: 8),
            Text('Chọn bạn để gửi thiết kế LED', style: TextStyle(fontSize: 14, color: Colors.grey[400])),
            const SizedBox(height: 24),
            ...friends.map((friend) => Padding(
              padding: const EdgeInsets.only(bottom: 12),
              child: FriendCard(
                name: friend['name'] as String,
                emoji: friend['emoji'] as String,
                isOnline: friend['isOnline'] as bool,
                onTap: () => Navigator.push(context, MaterialPageRoute(builder: (context) => ChatDetailScreen(name: friend['name'] as String, emoji: friend['emoji'] as String, isOnline: friend['isOnline'] as bool))),
              ),
            )),
          ],
        ),
      ),
    );
  }
}