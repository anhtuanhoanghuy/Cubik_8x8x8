import 'package:flutter/material.dart';
import 'package:flutter_native_splash/flutter_native_splash.dart';
import 'dart:math' as math;

void main() {
  runApp(const CubikLEDApp());
}

class CubikLEDApp extends StatelessWidget {
  const CubikLEDApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Cubik LED',
      theme: ThemeData(
        brightness: Brightness.dark,
        scaffoldBackgroundColor: const Color(0xFF0A0E1A),
        primaryColor: const Color(0xFF5B6EF5),
        fontFamily: 'SF Pro Display',
      ),
      home: const MainScreen(),
      debugShowCheckedModeBanner: false,
    );
  }
}

class MainScreen extends StatefulWidget {
  const MainScreen({super.key});

  @override
  State<MainScreen> createState() => _MainScreenState();
}

class _MainScreenState extends State<MainScreen> {
  int _currentIndex = 0;

  void _showAddMenu(BuildContext context) {
    showMenu(
      context: context,
      position: const RelativeRect.fromLTRB(1000, 140, 20, 0),
      color: const Color(0xFF1E293B),
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
      items: [
        PopupMenuItem(
          child: const Text('Tìm thiết bị', style: TextStyle(color: Colors.white, fontSize: 15)),
          onTap: () {},
        ),
        PopupMenuItem(
          child: const Text('Quét QR', style: TextStyle(color: Colors.white, fontSize: 15)),
          onTap: () {},
        ),
      ],
    );
  }

  void _showNotifications(BuildContext context) {
    showMenu(
      context: context,
      position: const RelativeRect.fromLTRB(1000, 140, 80, 0),
      color: const Color(0xFF1E293B),
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
      items: [
        PopupMenuItem(
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Row(
                children: [
                  Container(
                    width: 8,
                    height: 8,
                    decoration: const BoxDecoration(color: Color(0xFF5B6EF5), shape: BoxShape.circle),
                  ),
                  const SizedBox(width: 12),
                  const Expanded(
                    child: Text('Bạn lỡ 1 tin nhắn từ Tuấn', style: TextStyle(color: Colors.white, fontSize: 14)),
                  ),
                ],
              ),
              const SizedBox(height: 4),
              Padding(
                padding: const EdgeInsets.only(left: 20),
                child: Text('5 phút trước', style: TextStyle(color: Colors.grey[400], fontSize: 12)),
              ),
            ],
          ),
        ),
      ],
    );
  }

  Widget _buildScrollContent() {
    switch (_currentIndex) {
      case 0:
        return const ModeContent();
      case 1:
        return const ChatContent();
      case 2:
        return const SettingContent();
      default:
        return const ModeContent();
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SafeArea(
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Padding(
              padding: const EdgeInsets.all(20.0),
              child: Row(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: [
                  Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      const Text('Cubik LED', style: TextStyle(fontSize: 32, fontWeight: FontWeight.bold, color: Colors.white)),
                      const SizedBox(height: 4),
                      Text('IoT Controller', style: TextStyle(fontSize: 16, color: Colors.grey[400])),
                    ],
                  ),
                  Row(
                    children: [
                      Stack(
                        children: [
                          IconButton(
                            icon: const Icon(Icons.notifications_outlined, size: 28),
                            color: Colors.white,
                            onPressed: () => _showNotifications(context),
                          ),
                          Positioned(
                            right: 8,
                            top: 8,
                            child: Container(
                              padding: const EdgeInsets.all(4),
                              decoration: const BoxDecoration(color: Colors.red, shape: BoxShape.circle),
                              constraints: const BoxConstraints(minWidth: 18, minHeight: 18),
                              child: const Text('3', style: TextStyle(color: Colors.white, fontSize: 10, fontWeight: FontWeight.bold), textAlign: TextAlign.center),
                            ),
                          ),
                        ],
                      ),
                      IconButton(
                        icon: const Icon(Icons.add, size: 28),
                        color: Colors.white,
                        onPressed: () => _showAddMenu(context),
                      ),
                    ],
                  ),
                ],
              ),
            ),
            Padding(
              padding: const EdgeInsets.symmetric(horizontal: 20.0),
              child: Container(
                padding: const EdgeInsets.all(20),
                decoration: BoxDecoration(
                  color: const Color(0xFF1A1F2E),
                  borderRadius: BorderRadius.circular(16),
                  border: Border.all(color: Colors.grey.withOpacity(0.2), width: 1),
                ),
                child: Row(
                  children: [
                    Container(
                      width: 12,
                      height: 12,
                      decoration: const BoxDecoration(color: Color(0xFF4ADE80), shape: BoxShape.circle),
                    ),
                    const SizedBox(width: 12),
                    Expanded(
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: [
                          const Text('Cubik LED #001', style: TextStyle(fontSize: 18, fontWeight: FontWeight.w600, color: Colors.white)),
                          const SizedBox(height: 4),
                          Text('Đã kết nối', style: TextStyle(fontSize: 14, color: Colors.grey[400])),
                        ],
                      ),
                    ),
                    Text('8x8x8 LEDs', style: TextStyle(fontSize: 14, color: Colors.grey[400])),
                  ],
                ),
              ),
            ),
            const SizedBox(height: 8),
            Expanded(child: _buildScrollContent()),
          ],
        ),
      ),
      bottomNavigationBar: BottomNavigationBar(
        currentIndex: _currentIndex,
        onTap: (index) => setState(() => _currentIndex = index),
        backgroundColor: const Color(0xFF0A0E1A),
        selectedItemColor: const Color(0xFF5B6EF5),
        unselectedItemColor: Colors.grey[600],
        type: BottomNavigationBarType.fixed,
        items: const [
          BottomNavigationBarItem(icon: Icon(Icons.flash_on), label: 'Mode'),
          BottomNavigationBarItem(icon: Icon(Icons.chat_bubble_outline), label: 'Chat'),
          BottomNavigationBarItem(icon: Icon(Icons.settings_outlined), label: 'Setting'),
        ],
      ),
    );
  }
}

class ModeContent extends StatefulWidget {
  const ModeContent({super.key});

  @override
  State<ModeContent> createState() => _ModeContentState();
}

class _ModeContentState extends State<ModeContent> {
  double _speedValue = 50;
  double _brightnessValue = 75;

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
            const Text('Bạn bè', style: TextStyle(fontSize: 28, fontWeight: FontWeight.bold, color: Colors.white)),
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

class SettingContent extends StatelessWidget {
  const SettingContent({super.key});

  @override
  Widget build(BuildContext context) {
    return Center(
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          Icon(Icons.settings, size: 80, color: Colors.grey[600]),
          const SizedBox(height: 16),
          Text('Setting Screen', style: TextStyle(fontSize: 24, color: Colors.grey[400])),
        ],
      ),
    );
  }
}

class EffectCard extends StatelessWidget {
  final String title;
  final String subtitle;
  final IconData icon;
  final Gradient? gradient;
  final Color? backgroundColor;
  final Color? iconColor;
  final bool isLarge;

  const EffectCard({
    super.key,
    required this.title,
    required this.subtitle,
    required this.icon,
    this.gradient,
    this.backgroundColor,
    this.iconColor,
    this.isLarge = false,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      decoration: BoxDecoration(
        gradient: gradient,
        color: backgroundColor,
        borderRadius: BorderRadius.circular(16),
        border: backgroundColor != null ? Border.all(color: Colors.grey.withOpacity(0.2), width: 1) : null,
      ),
      child: Material(
        color: Colors.transparent,
        child: InkWell(
          borderRadius: BorderRadius.circular(16),
          onTap: () {},
          child: Padding(
            padding: const EdgeInsets.all(20.0),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Container(
                  width: 56,
                  height: 56,
                  decoration: BoxDecoration(
                    color: gradient != null ? Colors.white.withOpacity(0.2) : iconColor?.withOpacity(0.2),
                    borderRadius: BorderRadius.circular(12),
                  ),
                  child: Icon(icon, color: gradient != null ? Colors.white : iconColor, size: 28),
                ),
                const Spacer(),
                Text(title, style: const TextStyle(fontSize: 18, fontWeight: FontWeight.bold, color: Colors.white)),
                const SizedBox(height: 4),
                Text(subtitle, style: TextStyle(fontSize: 13, color: gradient != null ? Colors.white.withOpacity(0.8) : Colors.grey[400])),
              ],
            ),
          ),
        ),
      ),
    );
  }
}

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

class ChatDetailScreen extends StatefulWidget {
  final String name;
  final String emoji;
  final bool isOnline;

  const ChatDetailScreen({super.key, required this.name, required this.emoji, required this.isOnline});

  @override
  State<ChatDetailScreen> createState() => _ChatDetailScreenState();
}

class _ChatDetailScreenState extends State<ChatDetailScreen> {
  final TextEditingController _textController = TextEditingController();
  bool _isTextFilled = false;
  bool _isDropdownExpanded = false;
  Color _selectedColor = Colors.white;
  double _brightness = 50;
  double _speed = 50;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: const Color(0xFF0A0E1A),
      body: SafeArea(
        child: Column(
          children: [
            Padding(
              padding: const EdgeInsets.symmetric(vertical: 20.0),
              child: Row(
                mainAxisAlignment: MainAxisAlignment.start,
                children: [
                  IconButton(icon: const Icon(Icons.arrow_back, color: Colors.white), onPressed: () => Navigator.pop(context)),
                  const SizedBox(width: 12),
                  Container(
                    width: 48,
                    height: 48,
                    decoration: const BoxDecoration(color: Color(0xFF5B6EF5), shape: BoxShape.circle),
                    child: Center(child: Text(widget.emoji, style: const TextStyle(fontSize: 24))),
                  ),
                  const SizedBox(width: 12),
                  Expanded(
                    child: Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: [
                        Text(widget.name, style: const TextStyle(fontSize: 18, fontWeight: FontWeight.w600, color: Colors.white)),
                        const SizedBox(height: 2),
                        widget.isOnline ?
                        Row(
                          children: [
                            Container(width: 8, height: 8, decoration: const BoxDecoration(color: Color(0xFF4ADE80), shape: BoxShape.circle)),
                            const SizedBox(width: 6),
                            Text('Đang online', style: TextStyle(fontSize: 13, color: Colors.grey[400])),
                          ],
                        ) :
                        Text("2 giờ trước"),
                      ],
                    ),
                  ),
                ],
              ),
            ),
            Expanded(
              child: SingleChildScrollView(
                padding: const EdgeInsets.symmetric(horizontal: 20.0),
                child: Column(
                  children: [
                    GestureDetector(
                      onTap: () => setState(() => _isDropdownExpanded = !_isDropdownExpanded),
                      child: Container(
                        padding: const EdgeInsets.all(20),
                        decoration: BoxDecoration(
                          color: const Color(0xFF1A1F2E),
                          borderRadius: BorderRadius.circular(16),
                          border: Border.all(color: Colors.grey.withOpacity(0.2), width: 1),
                        ),
                        child: Row(
                          mainAxisAlignment: MainAxisAlignment.spaceBetween,
                          children: [
                            const Text('Khối LED 8x8x8', style: TextStyle(fontSize: 18, fontWeight: FontWeight.w600, color: Colors.white)),
                            Icon(_isDropdownExpanded ? Icons.keyboard_arrow_up : Icons.keyboard_arrow_down, color: Colors.white),
                          ],
                        ),
                      ),
                    ),
                    if (_isDropdownExpanded) ...[
                      const SizedBox(height: 16),
                      Container(
                        height: 200,
                        decoration: BoxDecoration(
                          color: const Color(0xFF1A1F2E),
                          borderRadius: BorderRadius.circular(16),
                          border: Border.all(color: Colors.grey.withOpacity(0.2), width: 1),
                        ),
                        child: Center(child: Icon(Icons.view_in_ar, size: 80, color: Colors.grey[600])),
                      ),
                    ],
                    const SizedBox(height: 24),
                    Container(
                      padding: const EdgeInsets.all(16),
                      decoration: BoxDecoration(
                        color: const Color(0xFF1A1F2E),
                        borderRadius: BorderRadius.circular(16),
                        border: Border.all(color: Colors.grey.withOpacity(0.2), width: 1),
                      ),
                      child: Row(
                        children: [
                          Container(
                            width: 56,
                            height: 56,
                            decoration: BoxDecoration(color: Colors.red.withOpacity(0.2), borderRadius: BorderRadius.circular(12)),
                            child: const Icon(Icons.mic, color: Colors.red, size: 28),
                          ),
                          const SizedBox(width: 12),
                          Expanded(
                            child: TextField(
                              onChanged: (value) {
                                setState(() {
                                  _isTextFilled = value.trim().isNotEmpty;
                                });
                              },
                              controller: _textController,
                              style: TextStyle(color: _selectedColor, fontSize: 16),
                              decoration: InputDecoration(
                                hintText: 'Nhập text hiện thị lên LED',
                                hintStyle: TextStyle(color: Colors.grey[600], fontSize: 16),
                                border: InputBorder.none,
                              ),
                            ),
                          ),
                          Container(
                            width: 56,
                            height: 56,
                            decoration: BoxDecoration(color: _isTextFilled ? Colors.blue.withOpacity(0.2) : Colors.grey[800], borderRadius: BorderRadius.circular(12)),
                            child: Icon(Icons.send, color: _isTextFilled ? Colors.blue : Colors.grey[600], size: 24),
                          ),
                        ],
                      ),
                    ),
                    const SizedBox(height: 24),
                    // Color Picker Section
                    Container(
                      padding: const EdgeInsets.all(20),
                      decoration: BoxDecoration(
                        color: const Color(0xFF1A1F2E),
                        borderRadius: BorderRadius.circular(16),
                        border: Border.all(
                          color: Colors.grey.withOpacity(0.2),
                          width: 1,
                        ),
                      ),
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: [
                          const Text(
                            'Chọn màu text',
                            style: TextStyle(
                              fontSize: 20,
                              fontWeight: FontWeight.bold,
                              color: Colors.white,
                            ),
                          ),
                          const SizedBox(height: 24),
                          // Color Wheel
                          Center(
                            child: ColorWheelPicker(
                              selectedColor: _selectedColor,
                              onColorChanged: (color) {
                                setState(() {
                                  _selectedColor = color;
                                });
                              },
                            ),
                          ),
                          const SizedBox(height: 24),
                          // Selected Color Display
                          Row(
                            children: [
                              Container(
                                width: 100,
                                height: 50,
                                decoration: BoxDecoration(
                                  color: _selectedColor,
                                  borderRadius: BorderRadius.circular(16),
                                ),
                              ),
                              const SizedBox(width: 20),
                              Column(
                                crossAxisAlignment: CrossAxisAlignment.start,
                                children: [
                                  Text(
                                    'Màu đã chọn',
                                    style: TextStyle(
                                      fontSize: 15,
                                      color: Colors.grey[400],
                                    ),
                                  ),
                                  const SizedBox(height: 6),
                                  Text(
                                    '#${_selectedColor.value.toRadixString(16).substring(2).toUpperCase()}',
                                    style: const TextStyle(
                                      fontSize: 18,
                                      fontWeight: FontWeight.bold,
                                      color: Colors.white,
                                      letterSpacing: 1,
                                    ),
                                  ),
                                ],
                              ),
                            ],
                          ),
                        ],
                      ),
                    ),
                    const SizedBox(height: 24),
                    Container(
                      padding: const EdgeInsets.all(20),
                      decoration: BoxDecoration(
                        color: const Color(0xFF1A1F2E),
                        borderRadius: BorderRadius.circular(16),
                        border: Border.all(color: Colors.grey.withOpacity(0.2), width: 1),
                      ),
                      child: Column(
                        children: [
                          Row(mainAxisAlignment: MainAxisAlignment.spaceBetween, children: [
                            const Text('Độ sáng', style: TextStyle(fontSize: 16, color: Colors.white)),
                            Text('${_brightness.round()}%', style: const TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: Colors.white)),
                          ]),
                          Slider(value: _brightness, min: 0, max: 100, activeColor: const Color(0xFF5B6EF5), inactiveColor: Colors.grey[800], onChanged: (value) => setState(() => _brightness = value)),
                          const SizedBox(height: 16),
                          Row(mainAxisAlignment: MainAxisAlignment.spaceBetween, children: [
                            const Text('Tốc độ hiển chữ', style: TextStyle(fontSize: 16, color: Colors.white)),
                            Text('${_speed.round()}%', style: const TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: Colors.white)),
                          ]),
                          Slider(value: _speed, min: 0, max: 100, activeColor: const Color(0xFF5B6EF5), inactiveColor: Colors.grey[800], onChanged: (value) => setState(() => _speed = value)),
                        ],
                      ),
                    ),
                    const SizedBox(height: 24),
                  ],
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }

  @override
  void dispose() {
    _textController.dispose();
    super.dispose();
  }
}
// Custom Color Wheel Picker Widget
class ColorWheelPicker extends StatefulWidget {
  final Color selectedColor;
  final ValueChanged<Color> onColorChanged;

  const ColorWheelPicker({
    super.key,
    required this.selectedColor,
    required this.onColorChanged,
  });

  @override
  State<ColorWheelPicker> createState() => _ColorWheelPickerState();
}

class _ColorWheelPickerState extends State<ColorWheelPicker> {
  Offset? _indicatorPosition;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onPanDown: _handleTouch,
      onPanUpdate: _handleTouch,
      child: CustomPaint(
        size: const Size(250, 250),
        painter: ColorWheelPainter(
          selectedColor: widget.selectedColor,
          indicatorPosition: _indicatorPosition,
        ),
      ),
    );
  }

  void _handleTouch(details) {
    RenderBox box = context.findRenderObject() as RenderBox;
    Offset localPosition = box.globalToLocal(details.globalPosition);

    final center = Offset(box.size.width / 2, box.size.height / 2);
    final distance = (localPosition - center).distance;
    final radius = box.size.width / 2;

    if (distance <= radius) {
      setState(() {
        _indicatorPosition = localPosition;
      });

      // Calculate color based on position
      final angle = math.atan2(
        localPosition.dy - center.dy,
        localPosition.dx - center.dx,
      );

      final hue = (angle * 180 / math.pi + 360) % 360;
      final saturation = (distance / radius).clamp(0.0, 1.0);
      final value = 1.0;

      widget.onColorChanged(
        HSVColor.fromAHSV(1.0, hue, saturation, value).toColor(),
      );
    }
  }
}

class ColorWheelPainter extends CustomPainter {
  final Color selectedColor;
  final Offset? indicatorPosition;

  ColorWheelPainter({
    required this.selectedColor,
    this.indicatorPosition,
  });

  @override
  void paint(Canvas canvas, Size size) {
    final center = Offset(size.width / 2, size.height / 2);
    final radius = size.width / 2;

    // Draw color wheel with sweep and radial gradient combination
    for (double i = 0; i < 360; i += 1) {
      final hue = i;

      final paint = Paint()
        ..shader = RadialGradient(
          colors: [
            Colors.white,
            HSVColor.fromAHSV(1.0, hue, 0.5, 1.0).toColor(),
            HSVColor.fromAHSV(1.0, hue, 1.0, 1.0).toColor(),
          ],
          stops: const [0.0, 0.5, 1.0],
        ).createShader(Rect.fromCircle(center: center, radius: radius));

      final startAngle = (i - 0.5) * math.pi / 180;
      final sweepAngle = 1.0 * math.pi / 180;

      canvas.drawArc(
        Rect.fromCircle(center: center, radius: radius),
        startAngle,
        sweepAngle,
        true,
        paint,
      );
    }

    // Draw indicator at selected position or center
    final indicatorPos = indicatorPosition ?? center;

    final indicatorPaint = Paint()
      ..color = selectedColor
      ..style = PaintingStyle.fill;

    final indicatorBorderPaint = Paint()
      ..color = Colors.white
      ..style = PaintingStyle.stroke
      ..strokeWidth = 4;

    canvas.drawCircle(indicatorPos, 15, indicatorBorderPaint);
    canvas.drawCircle(indicatorPos, 15, indicatorPaint);
  }

  @override
  bool shouldRepaint(ColorWheelPainter oldDelegate) {
    return oldDelegate.selectedColor != selectedColor ||
        oldDelegate.indicatorPosition != indicatorPosition;
  }
}