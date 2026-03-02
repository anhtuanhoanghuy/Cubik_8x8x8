import 'package:app/controller/mqtt_controller.dart';
import 'package:app/defines.dart';
import 'package:flutter/material.dart';
import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/src/extension_instance.dart';
import '../model/monitoring.dart';
import 'monitoring_controller.dart';

final monitoring = Get.find<Monitoring>();
final MonitoringController monitoringController = Get.find<MonitoringController>();
void handleTopic(String topic, List<int> bytes, ) {
  if (topic == topicStatus) {
    monitoring.isOnline = bytes[0];
    monitoringController.updateOnlineStatusFromModel(monitoring);
    print('online: ${monitoring.isOnline}');
  } else if (topic == topicMonitoring) {
    Map<int, List<int>>? decoded_monitoring = parseMonitoringTopic(bytes);
    syncMonitoring(decoded_monitoring);
    monitoringController.updateMonitoringFromModel(monitoring);
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

void syncMonitoring(Map<int, List<int>>? data) {
  final timeData = data?[CommonCommand.AUTO_OFF_SET_TIME.value];
  final wifi = data?[BleCommand.WIFI_CONNECT.value];
  monitoring.isConnectedBLE = data?[BleCommand.WIFI_ON_OFF.value]?.first ?? Status.OFF.value;
  monitoring.device_name =  String.fromCharCodes(data?[CommonCommand.NAME_CHANGE.value] ?? []);
  monitoring.device_tag = String.fromCharCodes(data?[CommonCommand.TAG_CHANGE.value] ?? []);
  monitoring.wifiConnected = wifi?[0] ?? Status.OFF.value;
  monitoring.wifiInfo = String.fromCharCodes(wifi?.sublist(1) ?? []);
  monitoring.LED = data?[CommonCommand.LED_ON_OFF.value]?.first ?? Status.OFF.value;
  monitoring.voiceMessage = data?[CommonCommand.VOICE_ON_OFF.value]?.first ?? Status.OFF.value;
  monitoring.autoOff = data?[CommonCommand.AUTO_OFF.value]?.first ?? Status.OFF.value;
  monitoring.sleepStartTime = TimeOfDay(hour: timeData?[0] ?? 0, minute: timeData?[1] ?? 0);
  monitoring.sleepEndTime = TimeOfDay(hour: timeData?[2] ?? 0, minute: timeData?[3] ?? 0);
  monitoring.ledMode = data?[CommonCommand.LED_MODE.value]?.first ?? LedMode.RAINBOW.value;
  monitoring.brightness = data?[CommonCommand.BRIGHTNESS.value]?.first ?? 50;
  monitoring.speed = data?[CommonCommand.SPEED.value]?.first ?? 50;
}