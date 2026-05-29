import 'package:flutter/material.dart';

class FriendCard extends StatelessWidget {
  final String name;
  final String emoji;
  final bool isOnline;
  final VoidCallback onTap;

  const FriendCard({
    super.key,
    required this.name,
    required this.emoji,
    required this.onTap,
    required this.isOnline
  });

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: Container(
        padding: const EdgeInsets.all(16),
        decoration: BoxDecoration(
          color: const Color(0xFF1A1F2E),
          borderRadius: BorderRadius.circular(16),
          border: Border.all(color: Colors.grey.withOpacity(0.2), width: 1),
        ),
        child: Row(
          children: [
            Stack(
              children: [
                Container(
                  width: 56,
                  height: 56,
                  decoration: const BoxDecoration(color: Color(0xFF5B6EF5), shape: BoxShape.circle),
                  child: Center(child: Text(emoji, style: const TextStyle(fontSize: 28))),
                ),
                if (isOnline)
                  Positioned(
                    right: 0,
                    bottom: 0,
                    child: Container(
                      width: 16,
                      height: 16,
                      decoration: BoxDecoration(
                        color: const Color(0xFF4ADE80),
                        shape: BoxShape.circle,
                        border: Border.all(color: const Color(0xFF1A1F2E), width: 2),
                      ),
                    ),
                  ),
              ],
            ),
            const SizedBox(width: 16),
            Expanded(child: Text(name, style: const TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: Colors.white))),
          ],
        ),
      ),
    );
  }
}