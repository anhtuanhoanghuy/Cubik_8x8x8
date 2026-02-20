import 'dart:collection';
import 'package:app/defines.dart';
import 'package:get/get.dart';

import 'mqtt_controller.dart';

class QueueProcessor extends GetxController {
  final Queue<Map<String, dynamic>> _queue = Queue();

  var queueLength = 0.obs;
  bool isProcessing = false;
  bool _isRunning = false; //create a task

  // Callbacks
  Function(List<int>)? sendByInternet;
  Function(List<int>)? sendByBLE;
  Function()? checkInternet;
  Function()? checkBle;



  // Init
  void init({
    required Function(List<int>) onWifi,
    required Function(List<int>) onBLE,
    required Function() internetCheck,
    required Function() bleCheck,
  }) {
    sendByInternet = onWifi;
    sendByBLE = onBLE;
    checkInternet = internetCheck;
    checkBle = bleCheck;
    _isRunning = true;
    _process();
  }

  //| Header (0xAA)| Cmd_ID (1byte)| Length (1byte) | Data (n byte) | Checksum |
  List<int> encodeCommand(int cmdId, List<int> cmdValue) {
    // Constants
    const int HEADER = 0xAA;

    // Calculate length
    int length = cmdValue.length;

    // Build packet (without checksum first)
    List<int> packet = [];
    packet.add(HEADER);           // Header: 0xAA
    packet.add(cmdId);            // Command ID
    packet.add(length);           // Length of data
    packet.addAll(cmdValue);      // Command value (data)

    // Calculate checksum (XOR of all bytes)
    int checksum = 0;
    for (int byte in packet) {
      checksum ^= byte;
    }

    // Add checksum to packet
    packet.add(checksum);

    return packet;
  }

  // Add command
  void addCommand({required int command_id, required List<int> command_value, CommunicationMethod method = CommunicationMethod.SEND_BY_AUTO}) {
    List<int> command = encodeCommand(command_id, command_value);
    _queue.add({
      'command': command,
      'method': method,  // 0=WiFi, 1=BLE, 2=Auto
    });
    queueLength.value = _queue.length;
    print('Added: ${toHexPretty(command)}, method=$method');
  }

  // Process queue
  Future<void> _process() async {
    while (_isRunning) {
      if (_queue.isNotEmpty && !isProcessing) {
        isProcessing = true;

        var cmdToSend = _queue.removeFirst();
        queueLength.value = _queue.length;

        await _send(cmdToSend);

        isProcessing = false;
        await Future.delayed(Duration(milliseconds: 100));
      }
      await Future.delayed(Duration(milliseconds: 50));
    }
  }

  // Send command
  Future<void> _send(Map<String, dynamic> cmd) async {
    List<int> command = cmd['command'];
    CommunicationMethod method = cmd['method'];

    try {
      switch (method) {
        case CommunicationMethod.SEND_BY_WIFI:
        // WiFi only
          await sendByInternet?.call(command);
          break;
        case CommunicationMethod.SEND_BY_BLE:
          await sendByBLE?.call(command);
          break;
        case CommunicationMethod.SEND_BY_AUTO:
        // Auto: WiFi → BLE
          bool internetOK = await checkInternet?.call();

          if (internetOK) {
            await sendByInternet?.call(command);
            print('Internet is selected');
          } else {
            await sendByBLE?.call(command);
            print('BLE is selected');
          }
          break;
        default:
          break;
      }
    } catch (e) {
      print('❌ Error: $e');
    }
  }

  void clear() {
    _queue.clear();
    queueLength.value = 0;
  }

  String toHexPretty(List<int> bytes) {
    return bytes
        .map((b) => b.toRadixString(16).padLeft(2, '0'))
        .join(' ')
        .toUpperCase();
  }
}