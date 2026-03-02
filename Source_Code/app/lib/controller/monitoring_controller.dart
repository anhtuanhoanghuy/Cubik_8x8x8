import 'package:flutter/material.dart';
import 'package:get/get.dart';
import '../defines.dart';
import '../model/monitoring.dart';

class MonitoringController extends GetxController {
  var isOnline = Status.OFFLINE.value.obs;
  var isConnectedBLE = Status.OFFLINE.value.obs;
  var device_name = ''.obs;
  var device_tag = ''.obs;
  var wifiConnected = Status.OFF.value.obs;
  var wifiInfo = ''.obs;
  var LED = Status.OFF.value.obs;
  var voiceMessage = Status.OFF.value.obs;
  var autoOff = Status.OFF.value.obs;
  var sleepStartTime = TimeOfDay(hour: 22, minute: 00).obs;
  var sleepEndTime = TimeOfDay(hour: 6, minute: 00).obs;
  var ledMode = LedMode.RAINBOW.value.obs;
  var brightness = 50.obs;
  var speed = 50.obs;

  // ✅ Update từ Model

  void updateOnlineStatusFromModel(Monitoring model){
    isOnline.value = model.isOnline;
  }
  void updateMonitoringFromModel(Monitoring model) {
    isConnectedBLE.value = model.isConnectedBLE;
    device_name.value = model.device_name;
    device_tag.value = model.device_tag;
    wifiConnected.value = model.wifiConnected;
    wifiInfo.value = model.wifiInfo;
    LED.value = model.LED;
    voiceMessage.value = model.voiceMessage;
    autoOff.value = model.autoOff;
    sleepStartTime.value = model.sleepStartTime;
    sleepEndTime.value = model.sleepEndTime;
    ledMode.value = model.ledMode;
    brightness.value = model.brightness;
    speed.value = model.speed;
  }
}
