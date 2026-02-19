import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/src/extension_instance.dart';
import 'package:get/get_state_manager/src/rx_flutter/rx_obx_widget.dart';
import 'chat_content.dart';
import 'controller/bluetooth_controller.dart';
import 'controller/command_controller.dart';
import 'controller/mqtt_controller.dart';
import 'mode_content.dart';
import 'setting_content.dart';

late MQTTClientWrapper newclient;
late BluetoothController bleController;
late QueueProcessor queue;
bool wifiConnected = false;
bool bleConnected = false;

void main() {
  runApp(const CubikLEDApp());
  bleController = BluetoothController();
  newclient = MQTTClientWrapper();
  newclient.prepareMqttClient();
  queue = Get.put(QueueProcessor());
  queue.init(
      onWifi: newclient.publishBytes,
      onBLE: bleController.sendCommand,
      wifiCheck: () => wifiConnected,
      bleCheck: () => bleConnected
  );
}

class CubikLEDApp extends StatelessWidget {
  const CubikLEDApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Cubik LED',
      theme: ThemeData(
        brightness: Brightness.dark,
        scaffoldBackgroundColor: const Color(0xFF0A0E1A),
        primaryColor: const Color(0xFF5B6EF5),
        fontFamily: 'SF Pro Display',
      ),
      home: const MainScreen(),
      debugShowCheckedModeBanner: false,
    );
  }
}

class MainScreen extends StatefulWidget {
  const MainScreen({super.key});

  @override
  State<MainScreen> createState() => _MainScreenState();
}

class _MainScreenState extends State<MainScreen> {
  int _currentIndex = 0;
  void _showBluetoothDialog() {
    final BluetoothController btController = Get.put(BluetoothController());

    // Bắt đầu scan ngay khi mở dialog
    btController.scanDevices();

    showDialog(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          backgroundColor: const Color(0xFF1A1F2E),
          shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
          title: const Text(
              'Thiết bị gần đây',
              style: TextStyle(
                  color: Colors.white,
                  fontSize: 20,
                  fontWeight: FontWeight.bold)),
          content: SizedBox(
            width: double.maxFinite,
            height: 340,
            child: Column(
              children: [

                // 🔹 Status message
                Obx(() => Padding(
                  padding: const EdgeInsets.only(bottom: 8),
                  child: Text(
                    btController.statusMessage.value,
                    style: TextStyle(
                      color: Colors.white.withOpacity(0.7),
                      fontSize: 12,
                    ),
                  ),
                )),

                // 🔹 Device list
                Expanded(
                  child: StreamBuilder<List<ScanResult>>(
                    stream: btController.scanResults,
                    builder: (context, snapshot) {
                      // Loading state
                      if (btController.isScanning.value &&
                          (!snapshot.hasData || snapshot.data!.isEmpty)) {
                        return const Center(
                          child: Column(
                            mainAxisAlignment: MainAxisAlignment.center,
                            children: [
                              CircularProgressIndicator(color: Color(0xFF5B6EF5)),
                              SizedBox(height: 16),
                              Text(
                                'Đang quét thiết bị...',
                                style: TextStyle(color: Colors.white70),
                              ),
                            ],
                          ),
                        );
                      }

                      // Empty state
                      if (!snapshot.hasData || snapshot.data!.isEmpty) {
                        return Center(
                          child: Column(
                            mainAxisAlignment: MainAxisAlignment.center,
                            children: [
                              const Icon(
                                Icons.bluetooth_disabled,
                                size: 64,
                                color: Colors.white30,
                              ),
                              const SizedBox(height: 16),
                              const Text(
                                'Không tìm thấy thiết bị',
                                style: TextStyle(color: Colors.white70),
                              ),
                              const SizedBox(height: 8),
                              TextButton.icon(
                                icon: const Icon(Icons.refresh, color: Color(0xFF5B6EF5)),
                                label: const Text(
                                  'Quét lại',
                                  style: TextStyle(color: Color(0xFF5B6EF5)),
                                ),
                                onPressed: () => btController.scanDevices(),
                              ),
                            ],
                          ),
                        );
                      }

                      // Device list
                      return ListView.builder(
                        shrinkWrap: true,
                        itemCount: snapshot.data!.length,
                        itemBuilder: (context, index) {
                          final result = snapshot.data![index];
                          final device = result.device;

                          // Get device name
                          String deviceName = device.platformName.isNotEmpty
                              ? device.platformName
                              : 'Thiết bị không tên';
                          return Card(
                            color: const Color(0xFF252B3D),
                            margin: const EdgeInsets.symmetric(vertical: 4),
                            shape: RoundedRectangleBorder(
                              borderRadius: BorderRadius.circular(8),
                            ),
                            child: ListTile(
                              contentPadding: const EdgeInsets.symmetric(
                                horizontal: 12,
                                vertical: 4,
                              ),

                              // Bluetooth icon
                              leading: Container(
                                padding: const EdgeInsets.all(8),
                                decoration: BoxDecoration(
                                  color: const Color(0xFF5B6EF5).withOpacity(0.2),
                                  borderRadius: BorderRadius.circular(8),
                                ),
                                child: const Icon(
                                  Icons.bluetooth,
                                  color: Color(0xFF5B6EF5),
                                  size: 24,
                                ),
                              ),

                              // Device name and MAC
                              title: Text(
                                deviceName,
                                style: const TextStyle(
                                  color: Colors.white,
                                  fontSize: 15,
                                  fontWeight: FontWeight.w500,
                                ),
                              ),
                              subtitle: Text(
                                device.remoteId.toString(),
                                style: TextStyle(
                                  color: Colors.white.withOpacity(0.5),
                                  fontSize: 12,
                                ),
                              ),

                              // Status
                              trailing: Obx(() {
                                String deviceId = device.remoteId.toString();

                                // Đang kết nối
                                if (btController.connectingDeviceId.value ==
                                    deviceId) {
                                  return Container(
                                    padding: const EdgeInsets.symmetric(
                                        horizontal: 12, vertical: 6),
                                    decoration: BoxDecoration(
                                      color: const Color(0xFF5B6EF5)
                                          .withOpacity(0.2),
                                      borderRadius: BorderRadius.circular(12),
                                    ),
                                    child: Row(
                                      mainAxisSize: MainAxisSize.min,
                                      children: [
                                        const SizedBox(
                                          width: 14,
                                          height: 14,
                                          child: CircularProgressIndicator(
                                            strokeWidth: 2,
                                            color: Color(0xFF5B6EF5),
                                          ),
                                        ),
                                        const SizedBox(width: 8),
                                        const Text(
                                          'Đang kết nối',
                                          style: TextStyle(
                                            color: Color(0xFF5B6EF5),
                                            fontSize: 11,
                                            fontWeight: FontWeight.w600,
                                          ),
                                        ),
                                      ],
                                    ),
                                  );
                                }

                                // Đã kết nối
                                if (btController.isConnected.value &&
                                    btController.connectedDevice.value?.remoteId
                                        .toString() == deviceId) {
                                  return Container(
                                    padding: const EdgeInsets.symmetric(
                                        horizontal: 12, vertical: 6),
                                    decoration: BoxDecoration(
                                      color: Colors.green.withOpacity(0.2),
                                      borderRadius: BorderRadius.circular(12),
                                    ),
                                    child: Row(
                                      mainAxisSize: MainAxisSize.min,
                                      children: [
                                        const Icon(Icons.check_circle,
                                            color: Colors.green, size: 14),
                                        const SizedBox(width: 6),
                                        const Text(
                                          'Đã kết nối',
                                          style: TextStyle(
                                            color: Colors.green,
                                            fontSize: 11,
                                            fontWeight: FontWeight.w600,
                                          ),
                                        ),
                                      ],
                                    ),
                                  );
                                }
                                // Chưa kết nối - hiện signal strength
                                return Text(''
                                );
                              }),

                              // On tap
                              onTap: () async {
                                String deviceId = device.remoteId.toString();

                                // Nếu đang kết nối → return
                                if (btController.connectingDeviceId.value == deviceId) {
                                  return;
                                }

                                // Nếu đã kết nối → ngắt kết nối
                                if (btController.isConnected.value &&
                                    btController.connectedDevice.value?.remoteId.toString() == deviceId) {
                                  await btController.disconnect();
                                  return;
                                }

                                // Connect
                                try {
                                  await btController.connectToDevice(device);
                                } catch (e) {}
                              },
                            ),
                          );
                        },
                      );
                    },
                  ),
                ),
              ],
            ),
          ),

          actions: [
            TextButton(
              onPressed: () {
                btController.isScanning.value = false;
                FlutterBluePlus.stopScan();
                Navigator.pop(context);
              },
              child: const Text(
                'Đóng',
                style: TextStyle(
                  color: Color(0xFF5B6EF5),
                  fontSize: 16,
                ),
              ),
            ),
          ],
        );
      },
    ).then((_) {
      // Stop scan khi đóng dialog
      btController.isScanning.value = false;
      FlutterBluePlus.stopScan();
    });
  }
  void _showAddMenu(BuildContext context) {
    showMenu(
      context: context,
      position: const RelativeRect.fromLTRB(1000, 140, 20, 0),
      color: const Color(0xFF1E293B),
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
      items: [
        PopupMenuItem(
          child: const Text('Tìm thiết bị', style: TextStyle(color: Colors.white, fontSize: 15)),
          onTap: () {
            _showBluetoothDialog();
          },
        ),
        PopupMenuItem(
          child: const Text('Quét QR', style: TextStyle(color: Colors.white, fontSize: 15)),
          onTap: () {},
        ),
      ],
    );
  }

  void _showNotifications(BuildContext context) {
    showMenu(
      context: context,
      position: const RelativeRect.fromLTRB(1000, 140, 80, 0),
      color: const Color(0xFF1E293B),
      shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(12)),
      items: [
        PopupMenuItem(
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Row(
                children: [
                  Container(
                    width: 8,
                    height: 8,
                    decoration: const BoxDecoration(color: Color(0xFF5B6EF5), shape: BoxShape.circle),
                  ),
                  const SizedBox(width: 12),
                  const Expanded(
                    child: Text('Bạn lỡ 1 tin nhắn từ Tuấn', style: TextStyle(color: Colors.white, fontSize: 14)),
                  ),
                ],
              ),
              const SizedBox(height: 4),
              Padding(
                padding: const EdgeInsets.only(left: 20),
                child: Text('5 phút trước', style: TextStyle(color: Colors.grey[400], fontSize: 12)),
              ),
            ],
          ),
        ),
      ],
    );
  }

  Widget _buildScrollContent() {
    switch (_currentIndex) {
      case 0:
        return const ModeContent();
      case 1:
        return const ChatContent();
      case 2:
        return const SettingContent();
      default:
        return const ModeContent();
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SafeArea(
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Padding(
              padding: const EdgeInsets.all(20.0),
              child: Row(
                mainAxisAlignment: MainAxisAlignment.spaceBetween,
                children: [
                  Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      const Text('Cubik LED', style: TextStyle(fontSize: 32, fontWeight: FontWeight.bold, color: Colors.white)),
                      const SizedBox(height: 4),
                      Text('IoT Controller', style: TextStyle(fontSize: 16, color: Colors.grey[400])),
                    ],
                  ),
                  Row(
                    children: [
                      Stack(
                        children: [
                          IconButton(
                            icon: const Icon(Icons.notifications_outlined, size: 28),
                            color: Colors.white,
                            onPressed: () => _showNotifications(context),
                          ),
                          Positioned(
                            right: 8,
                            top: 8,
                            child: Container(
                              padding: const EdgeInsets.all(4),
                              decoration: const BoxDecoration(color: Colors.red, shape: BoxShape.circle),
                              constraints: const BoxConstraints(minWidth: 18, minHeight: 18),
                              child: const Text('3', style: TextStyle(color: Colors.white, fontSize: 10, fontWeight: FontWeight.bold), textAlign: TextAlign.center),
                            ),
                          ),
                        ],
                      ),
                      IconButton(
                        icon: const Icon(Icons.add, size: 28),
                        color: Colors.white,
                        onPressed: () => _showAddMenu(context),
                      ),
                    ],
                  ),
                ],
              ),
            ),
            Padding(
              padding: const EdgeInsets.symmetric(horizontal: 20.0),
              child: Container(
                padding: const EdgeInsets.all(20),
                decoration: BoxDecoration(
                  color: const Color(0xFF1A1F2E),
                  borderRadius: BorderRadius.circular(16),
                  border: Border.all(color: Colors.grey.withOpacity(0.2), width: 1),
                ),
                child: Row(
                  children: [
                    Container(
                      width: 12,
                      height: 12,
                      decoration: const BoxDecoration(color: Color(0xFF4ADE80), shape: BoxShape.circle),
                    ),
                    const SizedBox(width: 12),
                    Expanded(
                      child: Column(
                        crossAxisAlignment: CrossAxisAlignment.start,
                        children: [
                          const Text('Cubik LED #001', style: TextStyle(fontSize: 18, fontWeight: FontWeight.w600, color: Colors.white)),
                          const SizedBox(height: 4),
                          Text('Đã kết nối', style: TextStyle(fontSize: 14, color: Colors.grey[400])),
                        ],
                      ),
                    ),
                    Text('8x8x8 LEDs', style: TextStyle(fontSize: 14, color: Colors.grey[400])),
                  ],
                ),
              ),
            ),
            const SizedBox(height: 8),
            Expanded(child: _buildScrollContent()),
          ],
        ),
      ),
      bottomNavigationBar: BottomNavigationBar(
        currentIndex: _currentIndex,
        onTap: (index) => setState(() => _currentIndex = index),
        backgroundColor: const Color(0xFF0A0E1A),
        selectedItemColor: const Color(0xFF5B6EF5),
        unselectedItemColor: Colors.grey[600],
        type: BottomNavigationBarType.fixed,
        items: const [
          BottomNavigationBarItem(icon: Icon(Icons.flash_on), label: 'Mode'),
          BottomNavigationBarItem(icon: Icon(Icons.chat_bubble_outline), label: 'Chat'),
          BottomNavigationBarItem(icon: Icon(Icons.settings_outlined), label: 'Setting'),
        ],
      ),
    );
  }
}