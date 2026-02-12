import 'package:flutter/material.dart';
import 'model/color_picker.dart';

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