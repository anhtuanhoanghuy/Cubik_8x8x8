import 'package:flutter/material.dart';

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