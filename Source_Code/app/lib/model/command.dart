// lib/models/command_model.dart

enum TransportMethod {
  wifiOnly,    // 0 - Chỉ WiFi (MQTT)
  bleOnly,     // 1 - Chỉ BLE
  auto,        // 2 - Tự động chọn (WiFi ưu tiên, fallback BLE)
}

class Command {
  final String topic_uuid;          // MQTT topic hoặc BLE characteristic UUID
  final dynamic data;           // String hoặc List<int>
  final TransportMethod method;
  final DateTime timestamp;
  final String id;

  Command({
    required this.topic_uuid,
    required this.data,
    this.method = TransportMethod.auto,
  }) : timestamp = DateTime.now(),
        id = DateTime.now().millisecondsSinceEpoch.toString();

  @override
  String toString() {
    String methodStr = method == TransportMethod.wifiOnly
        ? 'WiFi'
        : method == TransportMethod.bleOnly
        ? 'BLE'
        : 'Auto';
    return '[$methodStr] $topic_uuid: $data';
  }
}