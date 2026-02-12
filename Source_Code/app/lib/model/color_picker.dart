import 'package:flutter/material.dart';
import 'dart:math' as math;

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