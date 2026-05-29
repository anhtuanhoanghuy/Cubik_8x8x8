import 'package:flutter/material.dart';

import '../defines.dart';

class Monitoring {
  int isOnline = Status.OFFLINE.value;
  int isConnectedBLE = Status.OFFLINE.value;
  String device_name =  '';
  String device_tag = '';
  int wifiConnected = Status.OFF.value;
  String wifiInfo = 'Chưa được kết nối';
  int LED = Status.OFF.value;
  int voiceMessage = Status.OFF.value;
  int autoOff = Status.OFF.value;
  TimeOfDay sleepStartTime = TimeOfDay(hour: 22, minute: 00);
  TimeOfDay sleepEndTime = TimeOfDay(hour: 6, minute: 00);
  int ledMode = LedMode.RAINBOW.value;
  int brightness = 50;
  int speed = 50;
}