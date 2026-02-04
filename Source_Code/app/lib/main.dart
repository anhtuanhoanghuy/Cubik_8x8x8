import 'package:flutter/material.dart';

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
      home: const ModeScreen(),
      debugShowCheckedModeBanner: false,
    );
  }
}

class ModeScreen extends StatefulWidget {
  const ModeScreen({super.key});

  @override
  State<ModeScreen> createState() => _ModeScreenState();
}

class _ModeScreenState extends State<ModeScreen> {
  int _currentIndex = 0; // Mode tab is selected
  double _speedValue = 50;
  double _brightnessValue = 75;

  void _showAddMenu(BuildContext context) {
    showMenu(
      context: context,
      position: const RelativeRect.fromLTRB(1000, 140, 20, 0),
      color: const Color(0xFF1E293B),
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
      items: [
        PopupMenuItem(
          child: const Text(
                'Tìm thiết bị',
                style: TextStyle(color: Colors.white, fontSize: 15),
          ),
          onTap: () {
            // Handle device search
          },
        ),
        PopupMenuItem(
          child: const Text(
                'Quét QR',
                style: TextStyle(color: Colors.white, fontSize: 15),
          ),
          onTap: () {
            // Handle QR scan
          },
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
                    decoration: const BoxDecoration(
                      color: Color(0xFF5B6EF5),
                      shape: BoxShape.circle,
                    ),
                  ),
                  const SizedBox(width: 12),
                  const Expanded(
                    child: Text(
                      'Bạn lỡ 1 tin nhắn từ Tuấn',
                      style: TextStyle(color: Colors.white, fontSize: 14),
                    ),
                  ),
                ],
              ),
              const SizedBox(height: 4),
              Padding(
                padding: const EdgeInsets.only(left: 20),
                child: Text(
                  '5 phút trước',
                  style: TextStyle(
                    color: Colors.grey[400],
                    fontSize: 12,
                  ),
                ),
              ),
            ],
          ),
        ),
        PopupMenuItem(
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Row(
                children: [
                  Container(
                    width: 8,
                    height: 8,
                    decoration: const BoxDecoration(
                      color: Color(0xFF5B6EF5),
                      shape: BoxShape.circle,
                    ),
                  ),
                  const SizedBox(width: 12),
                  const Expanded(
                    child: Text(
                      'Thiết bị LED #001 đã kết nối',
                      style: TextStyle(color: Colors.white, fontSize: 14),
                    ),
                  ),
                ],
              ),
              const SizedBox(height: 4),
              Padding(
                padding: const EdgeInsets.only(left: 20),
                child: Text(
                  '1 giờ trước',
                  style: TextStyle(
                    color: Colors.grey[400],
                    fontSize: 12,
                  ),
                ),
              ),
            ],
          ),
        ),
        PopupMenuItem(
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Row(
                children: [
                  Container(
                    width: 8,
                    height: 8,
                    decoration: const BoxDecoration(
                      color: Color(0xFF5B6EF5),
                      shape: BoxShape.circle,
                    ),
                  ),
                  const SizedBox(width: 12),
                  const Expanded(
                    child: Text(
                      'Bạn có 1 cập nhật mới',
                      style: TextStyle(color: Colors.white, fontSize: 14),
                    ),
                  ),
                ],
              ),
              const SizedBox(height: 4),
              Padding(
                padding: const EdgeInsets.only(left: 20),
                child: Text(
                  '2 giờ trước',
                  style: TextStyle(
                    color: Colors.grey[400],
                    fontSize: 12,
                  ),
                ),
              ),
            ],
          ),
        ),
      ],
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SafeArea(
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            // Header
            Padding(
              padding: const EdgeInsets.all(20.0),
              child: Row(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: [
                  Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      const Text(
                        'Cubik LED',
                        style: TextStyle(
                          fontSize: 32,
                          fontWeight: FontWeight.bold,
                          color: Colors.white,
                        ),
                      ),
                      const SizedBox(height: 4),
                      Text(
                        'IoT Controller',
                        style: TextStyle(
                          fontSize: 16,
                          color: Colors.grey[400],
                        ),
                      ),
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
                              decoration: const BoxDecoration(
                                color: Colors.red,
                                shape: BoxShape.circle,
                              ),
                              constraints: const BoxConstraints(
                                minWidth: 18,
                                minHeight: 18,
                              ),
                              child: const Text(
                                '3',
                                style: TextStyle(
                                  color: Colors.white,
                                  fontSize: 10,
                                  fontWeight: FontWeight.bold,
                                ),
                                textAlign: TextAlign.center,
                              ),
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

            // Device Card
            Padding(
              padding: const EdgeInsets.symmetric(horizontal: 20.0),
              child: Container(
                padding: const EdgeInsets.all(20),
                decoration: BoxDecoration(
                  color: const Color(0xFF1A1F2E),
                  borderRadius: BorderRadius.circular(16),
                  border: Border.all(
                    color: Colors.grey.withOpacity(0.2),
                    width: 1,
                  ),
                ),
                child: Row(
                  children: [
                    Container(
                      width: 12,
                      height: 12,
                      decoration: const BoxDecoration(
                        color: Color(0xFF4ADE80),
                        shape: BoxShape.circle,
                      ),
                    ),
                    const SizedBox(width: 12),
                    Expanded(
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: [
                          const Text(
                            'Cubik LED #001',
                            style: TextStyle(
                              fontSize: 18,
                              fontWeight: FontWeight.w600,
                              color: Colors.white,
                            ),
                          ),
                          const SizedBox(height: 4),
                          Text(
                            'Đã kết nối',
                            style: TextStyle(
                              fontSize: 14,
                              color: Colors.grey[400],
                            ),
                          ),
                        ],
                      ),
                    ),
                    Text(
                      '8x8x8 LEDs',
                      style: TextStyle(
                        fontSize: 14,
                        color: Colors.grey[400],
                      ),
                    ),
                  ],
                ),
              ),
            ),
            const SizedBox(height: 8),


            // Effects Grid
            Expanded(
              child: SingleChildScrollView(
                child: Column(
                  children: [
                    const SizedBox(height: 20),
                    // Title
                    Padding(
                      padding: const EdgeInsets.symmetric(horizontal: 20.0),
                      child: Align(
                        alignment: Alignment.centerLeft,
                        child: Text(
                          'Chế độ hiệu ứng',
                          style: TextStyle(
                            fontSize: 24,
                            fontWeight: FontWeight.bold,
                            color: Colors.white,
                          ),
                        ),
                      ),
                    ),
                    const SizedBox(height: 8),
                    Padding(
                      padding: const EdgeInsets.symmetric(horizontal: 20.0),
                      child: Align(
                        alignment: Alignment.centerLeft,
                        child: Text(
                          'Chọn hiệu ứng ánh sáng cho LED của bạn',
                          style: TextStyle(
                            fontSize: 14,
                            color: Colors.grey[400],
                          ),
                        ),
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
                          gradient: const LinearGradient(
                            colors: [Color(0xFF7C3AED), Color(0xFF5B6EF5)],
                            begin: Alignment.topLeft,
                            end: Alignment.bottomRight,
                          ),
                          isLarge: true,
                        ),
                        EffectCard(
                          title: 'Rainbow',
                          subtitle: 'Hiệu ứng cầu vồng',
                          icon: Icons.star_rounded,
                          backgroundColor: const Color(0xFF1A1F2E),
                          iconColor: const Color(0xFF8ff2a9),
                        ),
                        EffectCard(
                          title: 'Wave',
                          subtitle: 'Sóng ánh sáng',
                          icon: Icons.waves,
                          backgroundColor: const Color(0xFF1A1F2E),
                          iconColor: const Color(0xFF5B6EF5),
                        ),
                        EffectCard(
                          title: 'Pulse',
                          subtitle: 'Nhấp nháy',
                          icon: Icons.flash_on,
                          backgroundColor: const Color(0xFF1A1F2E),
                          iconColor: const Color(0xFFFB923C),
                        ),
                        EffectCard(
                          title: 'Music',
                          subtitle: 'Theo nhạc',
                          icon: Icons.music_note,
                          backgroundColor: const Color(0xFF1A1F2E),
                          iconColor: const Color(0xFFA855F7),
                        ),
                        EffectCard(
                          title: 'Heart',
                          subtitle: 'Trái tim',
                          icon: Icons.favorite,
                          backgroundColor: const Color(0xFF1A1F2E),
                          iconColor: const Color(0xFFEC4899),
                        ),
                        EffectCard(
                          title: 'Stars',
                          subtitle: 'Ngôi sao',
                          icon: Icons.star_outline,
                          backgroundColor: const Color(0xFF1A1F2E),
                          iconColor: const Color(0xFF8B5CF6),
                        ),
                      ],
                    ),

                    // Settings Section
                    const SizedBox(height: 24),
                    Container(
                      margin: const EdgeInsets.symmetric(horizontal: 20.0),
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
                            'Cài đặt hiệu ứng',
                            style: TextStyle(
                              fontSize: 20,
                              fontWeight: FontWeight.bold,
                              color: Colors.white,
                            ),
                          ),
                          const SizedBox(height: 14),

                          // Speed Slider
                          Row(
                            mainAxisAlignment: MainAxisAlignment.spaceBetween,
                            children: [
                              const Text(
                                'Tốc độ',
                                style: TextStyle(
                                  fontSize: 16,
                                  color: Colors.white,
                                ),
                              ),
                              Text(
                                '${_speedValue.round()}%',
                                style: const TextStyle(
                                  fontSize: 16,
                                  fontWeight: FontWeight.w600,
                                  color: Colors.white,
                                ),
                              ),
                            ],
                          ),
                          SliderTheme(
                            data: SliderThemeData(
                              activeTrackColor: const Color(0xFF5B6EF5),
                              inactiveTrackColor: Colors.grey[800],
                              thumbColor: const Color(0xFF5B6EF5),
                              overlayColor: const Color(0xFF5B6EF5),
                              trackHeight: 3,
                              thumbShape: const RoundSliderThumbShape(
                                enabledThumbRadius: 10,
                              ),
                            ),
                            child: Slider(
                              value: _speedValue,
                              min: 0,
                              max: 100,
                              onChanged: (value) {
                                setState(() {
                                  _speedValue = value;
                                });
                              },
                            ),
                          ),
                          // Brightness Slider
                          Row(
                            mainAxisAlignment: MainAxisAlignment.spaceBetween,
                            children: [
                              const Text(
                                'Độ sáng',
                                style: TextStyle(
                                  fontSize: 16,
                                  color: Colors.white,
                                ),
                              ),
                              Text(
                                '${_brightnessValue.round()}%',
                                style: const TextStyle(
                                  fontSize: 16,
                                  fontWeight: FontWeight.w600,
                                  color: Colors.white,
                                ),
                              ),
                            ],
                          ),
                          SliderTheme(
                            data: SliderThemeData(
                              activeTrackColor: const Color(0xFF5B6EF5),
                              inactiveTrackColor: Colors.grey[800],
                              thumbColor: const Color(0xFF5B6EF5),
                              overlayColor: const Color(0xFF5B6EF5),
                              trackHeight: 3,
                              thumbShape: const RoundSliderThumbShape(
                                enabledThumbRadius: 10,
                              ),
                            ),
                            child: Slider(
                              value: _brightnessValue,
                              min: 0,
                              max: 100,
                              onChanged: (value) {
                                setState(() {
                                  _brightnessValue = value;
                                });
                              },
                            ),
                          ),
                        ],
                      ),
                    ),
                  ],
                ),
              ),
            ),
          ],
        ),
      ),
      bottomNavigationBar: BottomNavigationBar(
        currentIndex: _currentIndex,
        onTap: (index) {
          setState(() {
            _currentIndex = index;
          });
        },
        backgroundColor: const Color(0xFF0A0E1A),
        selectedItemColor: const Color(0xFF5B6EF5),
        unselectedItemColor: Colors.grey[600],
        type: BottomNavigationBarType.fixed,
        items: const [
          BottomNavigationBarItem(
            icon: Icon(Icons.flash_on),
            label: 'Mode',
          ),
          BottomNavigationBarItem(
            icon: Icon(Icons.chat_bubble_outline),
            label: 'Chat',
          ),
          BottomNavigationBarItem(
            icon: Icon(Icons.settings_outlined),
            label: 'Setting',
          ),
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
        border: backgroundColor != null
            ? Border.all(
          color: Colors.grey.withOpacity(0.2),
          width: 1,
        )
            : null,
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
                    color: gradient != null
                        ? Colors.white.withOpacity(0.2)
                        : iconColor?.withOpacity(0.2),
                    borderRadius: BorderRadius.circular(12),
                  ),
                  child: Icon(
                    icon,
                    color: gradient != null ? Colors.white : iconColor,
                    size: 28,
                  ),
                ),
                const Spacer(),
                Text(
                  title,
                  style: const TextStyle(
                    fontSize: 18,
                    fontWeight: FontWeight.bold,
                    color: Colors.white,
                  ),
                ),
                const SizedBox(height: 4),
                Text(
                  subtitle,
                  style: TextStyle(
                    fontSize: 13,
                    color: gradient != null
                        ? Colors.white.withOpacity(0.8)
                        : Colors.grey[400],
                  ),
                ),
              ],
            ),
          ),
        ),
      ),
    );
  }
}