import 'dart:io';
import 'package:get/get.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:permission_handler/permission_handler.dart';

class BluetoothController extends GetxController {
  // Observable states
  var isScanning = false.obs;
  var isConnected = false.obs;
  var statusMessage = 'Ready'.obs;
  var connectingDeviceId = ''.obs;
  // Connected device
  Rx<BluetoothDevice?> connectedDevice = Rx<BluetoothDevice?>(null);

  // GATT Service & Characteristic UUIDs (từ ESP32)
  final String serviceUUID = "12345678-1234-5678-1234-56789abcdef0";
  final String characteristicUUID = "12345678-1234-5678-1234-56789abcdef1";

  BluetoothCharacteristic? targetCharacteristic;

  // ============= REQUEST PERMISSIONS =============
  Future<bool> requestPermissions() async {
    if (Platform.isAndroid) {
      // Android 12+ (API 31+)
      Map<Permission, PermissionStatus> statuses = await [
        Permission.bluetoothScan,
        Permission.bluetoothConnect,
        Permission.location,  // Cần cho Android < 12
      ].request();

      // Check if all granted
      bool allGranted = statuses.values.every(
              (status) => status.isGranted
      );

      if (!allGranted) {
        statusMessage.value = 'Permissions denied';
        print('Permissions: $statuses');
        return false;
      }

      return true;
    }

    // iOS
    return true;
  }

  // ============= SCAN DEVICES =============
  Future<void> scanDevices() async {
    try {
      // Request permissions first
      bool hasPermissions = await requestPermissions();
      if (!hasPermissions) {
        statusMessage.value = 'Need Bluetooth permissions';
        return;
      }

      isScanning.value = true;
      statusMessage.value = 'Scanning...';
      // logScanResults();
      // Start scan với timeout
      await FlutterBluePlus.startScan(
        timeout: const Duration(seconds: 5),
      );

      // ✅ ĐỢI scan hoàn thành (5 giây)
      await Future.delayed(const Duration(seconds: 5));

      // Dừng scan (đề phòng)
      await FlutterBluePlus.stopScan();

      isScanning.value = false;
      statusMessage.value = 'Scan completed';

    } catch (e) {
      isScanning.value = false;
      statusMessage.value = 'Scan failed: $e';
      print('Error scanning: $e');
    }
  }

  Stream<List<ScanResult>> get scanResults {
    return FlutterBluePlus.scanResults.map((results) {
      return results.where((result) {

        // ✅ CHỈ CHECK SERVICE UUID
        bool hasValidService = result.advertisementData.serviceUuids.any(
                (uuid) => uuid.toString().toLowerCase() == serviceUUID.toLowerCase()
        );

        if (hasValidService) {
          print('✅ Valid Cubik device: ${result.device.platformName}');
        }

        return hasValidService;

      }).toList();
    });
  }
  // ============= LOG SCAN RESULTS =============
  void logScanResults() {
    FlutterBluePlus.scanResults.listen((results) {
      if (results.isEmpty) {
        print('📱 No devices found');
        return;
      }

      print('\n' + '=' * 70);
      print('📱 BLE SCAN RESULTS (${results.length} devices found)');
      print('=' * 70);

      // Sort by RSSI (strongest first)
      results.sort((a, b) => b.rssi.compareTo(a.rssi));

      for (int i = 0; i < results.length; i++) {
        var result = results[i];
        var device = result.device;

        print('\n[$i] ${device.platformName.isNotEmpty ? device.platformName : "Unnamed Device"}');
        print('    MAC Address: ${device.remoteId}');

        // Advertised services (if any)
        if (result.advertisementData.serviceUuids.isNotEmpty) {
          print('    Services: ${result.advertisementData.serviceUuids}');
        }

        // Manufacturer data (if any)
        if (result.advertisementData.manufacturerData.isNotEmpty) {
          print('    Manufacturer Data: ${result.advertisementData.manufacturerData}');
        }
      }

      print('\n' + '=' * 70 + '\n');
    });
  }

  // ============= CONNECT TO DEVICE =============
  Future<void> connectToDevice(BluetoothDevice device) async {
    try {
      statusMessage.value = 'Connecting to ${device.platformName}...';
      connectingDeviceId.value = device.remoteId.toString();
      // Connect
      await device.connect(
        timeout: const Duration(seconds: 15),
        autoConnect: false,
      );

      connectedDevice.value = device;
      isConnected.value = true;
      connectingDeviceId.value = '';
      statusMessage.value = 'Connected to ${device.platformName}';

      // Discover services
      await discoverServices(device);

      // Listen to connection state
      device.connectionState.listen((state) {
        if (state == BluetoothConnectionState.disconnected) {
          isConnected.value = false;
          connectedDevice.value = null;
          targetCharacteristic = null;
          connectingDeviceId.value = '';
          statusMessage.value = 'Disconnected';
        }
      });

    } catch (e) {
      connectingDeviceId.value = '';
      statusMessage.value = 'Connection failed: $e';
      print('Error connecting: $e');
    }
  }

  // ============= DISCOVER SERVICES =============
  Future<void> discoverServices(BluetoothDevice device) async {
    try {
      List<BluetoothService> services = await device.discoverServices();

      for (var service in services) {
        print('Service UUID: ${service.uuid}');

        // Tìm service của ESP32
        if (service.uuid.toString().toLowerCase() ==
            serviceUUID.toLowerCase()) {
          print('Found target service!');

          // Tìm characteristic
          for (var characteristic in service.characteristics) {
            print('  Characteristic UUID: ${characteristic.uuid}');

            if (characteristic.uuid.toString().toLowerCase() ==
                characteristicUUID.toLowerCase()) {
              print('  Found target characteristic!');
              targetCharacteristic = characteristic;

              // Enable notifications (optional)
              if (characteristic.properties.notify) {
                await characteristic.setNotifyValue(true);

                // Listen to notifications
                characteristic.lastValueStream.listen((value) {
                  String response = String.fromCharCodes(value);
                  print('Notification: $response');
                  statusMessage.value = 'Response: $response';
                });
              }
            }
          }
        }
      }

    } catch (e) {
      print('Error discovering services: $e');
    }
  }

  // ============= SEND COMMAND (WRITE) =============
  Future<void> sendCommand(List<int> data) async {
    if (targetCharacteristic == null) {
      statusMessage.value = 'Not connected to characteristic';
      return;
    }

    try {
      await targetCharacteristic!.write(
        data,
        withoutResponse: false,
      );

      print('Sent: $data');
      statusMessage.value = 'Command sent';

    } catch (e) {
      statusMessage.value = 'Send failed: $e';
      print('Error sending: $e');
    }
  }

  // ============= READ RESPONSE =============
  Future<String> readResponse() async {
    if (targetCharacteristic == null) {
      return 'Not connected';
    }

    try {
      List<int> value = await targetCharacteristic!.read();
      String response = String.fromCharCodes(value);

      print('Read: $response');
      statusMessage.value = 'Response: $response';

      return response;

    } catch (e) {
      print('Error reading: $e');
      return 'Read failed';
    }
  }

  // ============= DISCONNECT =============
  Future<void> disconnect() async {
    if (connectedDevice.value != null) {
      await connectedDevice.value!.disconnect();
      connectedDevice.value = null;
      isConnected.value = false;
      targetCharacteristic = null;
      statusMessage.value = 'Disconnected';
    }
  }

  // ============= HELPER: SEND WIFI CONFIG =============
  Future<void> sendWiFiConfig(String ssid, String password) async {
    // Format: 0x01 [length] [SSID]\0[Password]\0
    List<int> data = [];

    data.add(0x01);  // CMD ID: WiFi Config

    List<int> ssidBytes = ssid.codeUnits;
    List<int> passBytes = password.codeUnits;

    int dataLen = ssidBytes.length + 1 + passBytes.length + 1;
    data.add(dataLen);  // Data length

    data.addAll(ssidBytes);
    data.add(0x00);  // Null terminator

    data.addAll(passBytes);
    data.add(0x00);  // Null terminator

    print('Sending WiFi config: CMD=0x01, SSID=$ssid, Pass=$password');

    await sendCommand(data);

    // Đợi 2s rồi đọc response
    await Future.delayed(const Duration(seconds: 2));
    String response = await readResponse();
    print('WiFi config response: $response');
  }

  // ============= HELPER: SEND ON/OFF =============
  Future<void> sendOnOff(bool state) async {
    // Format: 0x02 0x01 [state]
    List<int> data = [
      0x02,              // CMD ID: ON/OFF
      0x01,              // Data length
      state ? 0x01 : 0x00  // State
    ];

    print('Sending ON/OFF: ${state ? "ON" : "OFF"}');

    await sendCommand(data);

    await Future.delayed(const Duration(milliseconds: 500));
    await readResponse();
  }

  // ============= HELPER: SEND TEST MODE =============
  Future<void> sendTestMode(int mode) async {
    // Format: 0x03 0x01 [mode]
    List<int> data = [
      0x03,    // CMD ID: Test Mode
      0x01,    // Data length
      mode     // Mode number
    ];

    print('Sending Test Mode: $mode');

    await sendCommand(data);

    await Future.delayed(const Duration(milliseconds: 500));
    await readResponse();
  }
}