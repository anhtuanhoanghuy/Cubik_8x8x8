import 'package:flutter/material.dart';
import 'model/effect_card.dart';
import 'controller/bluetooth.dart';

class ModeContent extends StatefulWidget {
  const ModeContent({super.key});

  @override
  State<ModeContent> createState() => _ModeContentState();
}

class _ModeContentState extends State<ModeContent> {
  double _speedValue = 50;
  double _brightnessValue = 50;

  @override
  Widget build(BuildContext context) {
    return SingleChildScrollView(
      child: Column(
        children: [
          const SizedBox(height: 20),
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: 20.0),
            child: Align(
              alignment: Alignment.centerLeft,
              child: Text('Chế độ hiệu ứng', style: TextStyle(fontSize: 24, fontWeight: FontWeight.bold, color: Colors.white)),
            ),
          ),
          const SizedBox(height: 8),
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: 20.0),
            child: Align(
              alignment: Alignment.centerLeft,
              child: Text('Chọn hiệu ứng ánh sáng cho LED của bạn', style: TextStyle(fontSize: 14, color: Colors.grey[400])),
            ),
          ),
          const SizedBox(height: 24),
          GridView.count(
            crossAxisCount: 2,
            padding: const EdgeInsets.symmetric(horizontal: 20.0),
            mainAxisSpacing: 16,
            crossAxisSpacing: 16,
            childAspectRatio: 1.1,
            shrinkWrap: true,
            physics: const NeverScrollableScrollPhysics(),
            children: [
              EffectCard(
                title: 'Custom LED',
                subtitle: 'Tự thiết kế mẫu',
                icon: Icons.auto_fix_high,
                gradient: const LinearGradient(colors: [Color(0xFF7C3AED), Color(0xFF5B6EF5)], begin: Alignment.topLeft, end: Alignment.bottomRight),
                isLarge: true,
              ),
              EffectCard(title: 'Rainbow', subtitle: 'Hiệu ứng cầu vồng', icon: Icons.star_rounded, backgroundColor: const Color(0xFF1A1F2E), iconColor: const Color(0xFF8ff2a9)),
              EffectCard(title: 'Wave', subtitle: 'Sóng ánh sáng', icon: Icons.waves, backgroundColor: const Color(0xFF1A1F2E), iconColor: const Color(0xFF5B6EF5)),
              EffectCard(title: 'Pulse', subtitle: 'Nhấp nháy', icon: Icons.flash_on, backgroundColor: const Color(0xFF1A1F2E), iconColor: const Color(0xFFFB923C)),
              EffectCard(title: 'Music', subtitle: 'Theo nhạc', icon: Icons.music_note, backgroundColor: const Color(0xFF1A1F2E), iconColor: const Color(0xFFA855F7)),
              EffectCard(title: 'Heart', subtitle: 'Trái tim', icon: Icons.favorite, backgroundColor: const Color(0xFF1A1F2E), iconColor: const Color(0xFFEC4899)),
              EffectCard(title: 'Stars', subtitle: 'Ngôi sao', icon: Icons.star_outline, backgroundColor: const Color(0xFF1A1F2E), iconColor: const Color(0xFF8B5CF6)),
            ],
          ),
          const SizedBox(height: 24),
          Container(
            margin: const EdgeInsets.symmetric(horizontal: 20.0),
            padding: const EdgeInsets.all(20),
            decoration: BoxDecoration(
              color: const Color(0xFF1A1F2E),
              borderRadius: BorderRadius.circular(16),
              border: Border.all(color: Colors.grey.withOpacity(0.2), width: 1),
            ),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                const Text('Cài đặt hiệu ứng', style: TextStyle(fontSize: 20, fontWeight: FontWeight.bold, color: Colors.white)),
                const SizedBox(height: 14),
                Row(mainAxisAlignment: MainAxisAlignment.spaceBetween, children: [
                  const Text('Tốc độ', style: TextStyle(fontSize: 16, color: Colors.white)),
                  Text('${_speedValue.round()}%', style: const TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: Colors.white)),
                ]),
                Slider(value: _speedValue, min: 0, max: 100, activeColor: const Color(0xFF5B6EF5), inactiveColor: Colors.grey[800], onChanged: (value) => setState(() => _speedValue = value)),
                Row(mainAxisAlignment: MainAxisAlignment.spaceBetween, children: [
                  const Text('Độ sáng', style: TextStyle(fontSize: 16, color: Colors.white)),
                  Text('${_brightnessValue.round()}%', style: const TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: Colors.white)),
                ]),
                Slider(value: _brightnessValue, min: 0, max: 100, activeColor: const Color(0xFF5B6EF5), inactiveColor: Colors.grey[800], onChanged: (value) => setState(() => _brightnessValue = value)),
              ],
            ),
          ),
          const SizedBox(height: 24),
        ],
      ),
    );
  }
}