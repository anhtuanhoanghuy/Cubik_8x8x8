import 'package:app/controller/mqtt_controller.dart';
import 'package:app/defines.dart';
import '../main.dart';
import '../model/monitoring.dart';

Monitoring monitoring = Monitoring();
void handleTopic(String topic, List<int> bytes, ) {
  if (topic == topicStatus) {
    monitoring.isOnline = bytes[0];
  } else if (topic == topicMonitoring) {
    parseMonitoringTopic(bytes);
  }
}

Map<int, List<int>>? parseMonitoringTopic(List<int> bytes) {
  // ----- Basic validation -----
  if (bytes.isEmpty || bytes[0] != 0xAA) {
    print('Invalid package');
    return null;
  }

  final int length = bytes[1];

  if (length < 4 || length + 3 != bytes.length) {
    print('Invalid package size');
    return null;
  }

  // ----- Checksum validation -----
  int checksum = 0;
  for (int i = 0; i < bytes.length - 1; i++) {
    checksum ^= bytes[i];
  }

  if (checksum != bytes.last) {
    print('Mistake package');
    return null;
  }

  // ----- Extract payload (không modify list gốc) -----
  final payload = bytes.sublist(2, bytes.length - 1);

  // ----- Parse key:value,key:value -----
  final Map<int, List<int>> result = {};

  List<int> currentKey = [];
  List<int> currentValue = [];
  bool readingValue = false;

  for (final byte in payload) {
    if (byte == 0x3A) { // ':'
      readingValue = true;
      continue;
    }

    if (byte == 0x2C) { // ','
      if (currentKey.isNotEmpty) {
        result[currentKey.first] = List.from(currentValue);
      }
      currentKey.clear();
      currentValue.clear();
      readingValue = false;
      continue;
    }

    readingValue ? currentValue.add(byte) : currentKey.add(byte);
  }

  // Lưu cặp cuối
  if (currentKey.isNotEmpty) {
    result[currentKey.first] = List.from(currentValue);
  }
  // In kết quả
  result.forEach((k, v) {
    print('Key: 0x${k.toRadixString(16).padLeft(2, '0')}, Value: $v');
  });
  return result;
}