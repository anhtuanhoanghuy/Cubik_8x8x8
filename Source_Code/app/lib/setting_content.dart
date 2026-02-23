import 'dart:typed_data';
import 'dart:convert';
import 'package:app/defines.dart';
import 'package:flutter/material.dart';
import './controller/command_controller.dart';
import 'package:get/get_core/src/get_main.dart';
import 'package:get/get_instance/src/extension_instance.dart';
import './controller/bluetooth_controller.dart';
import 'package:get/get_state_manager/src/rx_flutter/rx_obx_widget.dart';

class SettingContent extends StatefulWidget {
  const SettingContent({super.key});
  @override
  State<SettingContent> createState() => _SettingContentState();
}

class _SettingContentState extends State<SettingContent> {
  final bleController = Get.find<BluetoothController>();
  final queue = Get.find<QueueProcessor>();
  String deviceName = 'Hoàng Huy Anh Tuấn';
  String tagID = 'hoanghuyanhtuan';
  bool _ledPowerOn = true;
  bool _voiceNotify = true;
  bool _sleepTimerEnabled = false;
  String _wifiNetwork = 'My Network';
  bool _isWifiConnected = true;
  bool _isCheckUpdateDone = true;
  bool _haveNewFirmware = true;

  TimeOfDay _sleepStartTime = const TimeOfDay(hour: 22, minute: 0);
  TimeOfDay _sleepEndTime = const TimeOfDay(hour: 6, minute: 0);

  void _showSleepTimerDialog() {
    TimeOfDay tempStart = _sleepStartTime;
    TimeOfDay tempEnd = _sleepEndTime;

    showDialog(
      context: context,
      builder: (BuildContext context) {
        return StatefulBuilder(
          builder: (context, setDialogState) {
            return AlertDialog(
              backgroundColor: const Color(0xFF1A1F2E),
              shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
              title: const Text('Hẹn giờ ngủ', style: TextStyle(color: Colors.white, fontSize: 20, fontWeight: FontWeight.bold)),
              content: Column(
                mainAxisSize: MainAxisSize.min,
                children: [
                  // Start Time
                  Container(
                    padding: const EdgeInsets.all(16),
                    decoration: BoxDecoration(
                      color: const Color(0xFF0A0E1A),
                      borderRadius: BorderRadius.circular(12),
                    ),
                    child: Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        const Text('Thời gian bắt đầu', style: TextStyle(color: Colors.white, fontSize: 15)),
                        TextButton(
                          onPressed: () async {
                            final TimeOfDay? picked = await showTimePicker(
                              context: context,
                              initialTime: tempStart,
                              builder: (context, child) {
                                return Theme(
                                  data: ThemeData.dark().copyWith(
                                    colorScheme: const ColorScheme.dark(
                                      primary: Color(0xFF5B6EF5),
                                      surface: Color(0xFF1A1F2E),
                                    ),
                                  ),
                                  child: child!,
                                );
                              },
                            );
                            if (picked != null) {
                              setDialogState(() {
                                tempStart = picked;
                              });
                            }
                          },
                          child: Text(
                            '${tempStart.hour.toString().padLeft(2, '0')}:${tempStart.minute.toString().padLeft(2, '0')}',
                            style: const TextStyle(color: Color(0xFF5B6EF5), fontSize: 18, fontWeight: FontWeight.bold),
                          ),
                        ),
                      ],
                    ),
                  ),
                  const SizedBox(height: 16),
                  // End Time
                  Container(
                    padding: const EdgeInsets.all(16),
                    decoration: BoxDecoration(
                      color: const Color(0xFF0A0E1A),
                      borderRadius: BorderRadius.circular(12),
                    ),
                    child: Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        const Text('Thời gian kết thúc', style: TextStyle(color: Colors.white, fontSize: 15)),
                        TextButton(
                          onPressed: () async {
                            final TimeOfDay? picked = await showTimePicker(
                              context: context,
                              initialTime: tempEnd,
                              builder: (context, child) {
                                return Theme(
                                  data: ThemeData.dark().copyWith(
                                    colorScheme: const ColorScheme.dark(
                                      primary: Color(0xFF5B6EF5),
                                      surface: Color(0xFF1A1F2E),
                                    ),
                                  ),
                                  child: child!,
                                );
                              },
                            );
                            if (picked != null) {
                              setDialogState(() {
                                tempEnd = picked;
                              });
                            }
                          },
                          child: Text(
                            '${tempEnd.hour.toString().padLeft(2, '0')}:${tempEnd.minute.toString().padLeft(2, '0')}',
                            style: const TextStyle(color: Color(0xFF5B6EF5), fontSize: 18, fontWeight: FontWeight.bold),
                          ),
                        ),
                      ],
                    ),
                  ),
                ],
              ),
              actions: [
                TextButton(
                  onPressed: () => Navigator.pop(context),
                  child: Text('Hủy bỏ', style: TextStyle(color: Colors.grey[400], fontSize: 16)),
                ),
                TextButton(
                  onPressed: () {
                    queue.addCommand(command_id: CommonCommand.AUTO_OFF_SET_TIME.value, command_value: [tempStart.hour,tempStart.minute,tempEnd.hour,tempEnd.minute], method: CommunicationMethod.SEND_BY_AUTO);
                    setState(() {
                      _sleepStartTime = tempStart;
                      _sleepEndTime = tempEnd;
                    });
                    Navigator.pop(context);
                  },
                  child: const Text('OK', style: TextStyle(color: Color(0xFF5B6EF5), fontSize: 16, fontWeight: FontWeight.bold)),
                ),
              ],
            );
          },
        );
      },
    );
  }

  void _showWifiDialog() {
    queue.addCommand(command_id: BleCommand.WIFI_SCAN.value, command_value: [Status.ON.value], method: CommunicationMethod.SEND_BY_BLE);
    final List<Map<String, dynamic>> wifiNetworks = [
      {'name': 'My Network', 'signal': 4, 'secured': true},
      {'name': 'Office WiFi', 'signal': 3, 'secured': true},
      {'name': 'Guest Network', 'signal': 2, 'secured': false},
      {'name': 'Home 5G', 'signal': 4, 'secured': true},
      {'name': 'Public WiFi', 'signal': 1, 'secured': false},
      {'name': 'ANH TUAN_2G', 'signal': 1, 'secured': true},
    ];

    showDialog(
      context: context,
      builder: (BuildContext context) {
        return AlertDialog(
          backgroundColor: const Color(0xFF1A1F2E),
          shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
          title: const Text('Chọn mạng WiFi', style: TextStyle(color: Colors.white, fontSize: 20, fontWeight: FontWeight.bold)),
          content: SizedBox(
            width: double.maxFinite,
            child: ListView.builder(
              shrinkWrap: true,
              itemCount: wifiNetworks.length,
              itemBuilder: (context, index) {
                final network = wifiNetworks[index];
                return ListTile(
                  contentPadding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
                  leading: Icon(
                    network['signal'] >= 3 ? Icons.wifi : Icons.wifi_2_bar,
                    color: Colors.white,
                  ),
                  title: Text(network['name'], style: const TextStyle(color: Colors.white, fontSize: 15)),
                  trailing: network['secured'] ? Icon(Icons.lock, color: Colors.grey[600], size: 18) : null,
                  onTap: () {
                    Navigator.pop(context);
                    if (network['secured']) {
                      _showPasswordDialog(network['name']);
                    } else {
                      queue.addCommand(command_id: BleCommand.WIFI_CONNECT.value, command_value: List<int>.from(Uint8List.fromList(utf8.encode(network['name'])) as Iterable<dynamic>), method: CommunicationMethod.SEND_BY_BLE);
                      setState(() {
                        _wifiNetwork = network['name'];
                        _isWifiConnected = true;
                      });
                    }
                  },
                );
              },
            ),
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.pop(context),
              child: const Text('Đóng', style: TextStyle(color: Color(0xFF5B6EF5), fontSize: 16)),
            ),
          ],
        );
      },
    );
  }

  void _showPasswordDialog(String networkName) {
    final TextEditingController passwordController = TextEditingController();
    bool obscurePassword = true;

    showDialog(
      context: context,
      builder: (BuildContext context) {
        return StatefulBuilder(
          builder: (context, setDialogState) {
            return AlertDialog(
              backgroundColor: const Color(0xFF1A1F2E),
              shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
              title: Text('Kết nối $networkName', style: const TextStyle(color: Colors.white, fontSize: 18, fontWeight: FontWeight.bold)),
              content: Column(
                mainAxisSize: MainAxisSize.min,
                children: [
                  TextField(
                    controller: passwordController,
                    obscureText: obscurePassword,
                    style: const TextStyle(color: Colors.white),
                    decoration: InputDecoration(
                      hintText: 'Nhập mật khẩu',
                      hintStyle: TextStyle(color: Colors.grey[600]),
                      filled: true,
                      fillColor: const Color(0xFF0A0E1A),
                      border: OutlineInputBorder(
                        borderRadius: BorderRadius.circular(12),
                        borderSide: BorderSide.none,
                      ),
                      suffixIcon: IconButton(
                        icon: Icon(
                          obscurePassword ? Icons.visibility_off : Icons.visibility,
                          color: Colors.grey[600],
                        ),
                        onPressed: () {
                          setDialogState(() {
                            obscurePassword = !obscurePassword;
                          });
                        },
                      ),
                    ),
                  ),
                ],
              ),
              actions: [
                TextButton(
                  onPressed: () => Navigator.pop(context),
                  child: Text('Hủy bỏ', style: TextStyle(color: Colors.grey[400], fontSize: 16)),
                ),
                TextButton(
                  onPressed: () {
                    List<int> wifiConnect = [];
                    wifiConnect.add(networkName.length);
                    wifiConnect.add(passwordController.text.length);
                    wifiConnect.addAll(List<int>.from(Uint8List.fromList(utf8.encode(networkName)) as Iterable<dynamic>));
                    wifiConnect.addAll(List<int>.from(Uint8List.fromList(utf8.encode(passwordController.text)) as Iterable<dynamic>));
                    
                    queue.addCommand(command_id: BleCommand.WIFI_CONNECT.value, command_value: wifiConnect, method: CommunicationMethod.SEND_BY_BLE);
                    setState(() {
                      _wifiNetwork = networkName;
                      _isWifiConnected = true;
                    });
                    Navigator.pop(context);
                  },
                  child: const Text('Kết nối', style: TextStyle(color: Color(0xFF5B6EF5), fontSize: 16, fontWeight: FontWeight.bold)),
                ),
              ],
            );
          },
        );
      },
    );
  }
  void _changeInfoDialog(String info,String cmd) {
    final TextEditingController infoController = TextEditingController(text: info);

    showDialog(
      context: context,
      builder: (BuildContext context) {
        return StatefulBuilder(
          builder: (context, setDialogState) {
            return AlertDialog(
              backgroundColor: const Color(0xFF1A1F2E),
              shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
              content: Column(
                mainAxisSize: MainAxisSize.min,
                children: [
                  TextField(
                    autofocus: true,
                    maxLength: 30,
                    controller: infoController,
                    style: const TextStyle(color: Colors.white),
                    decoration: InputDecoration(
                      hintStyle: TextStyle(color: Colors.grey[600]),
                      filled: true,
                      fillColor: const Color(0xFF0A0E1A),
                      border: OutlineInputBorder(
                        borderRadius: BorderRadius.circular(12),
                        borderSide: BorderSide.none,
                      ),
                    ),
                  ),
                ],
              ),
              actions: [
                TextButton(
                  onPressed: () => Navigator.pop(context),
                  child: Text('Hủy bỏ', style: TextStyle(color: Colors.grey[400], fontSize: 16)),
                ),
                TextButton(
                  onPressed: () {
                    if (cmd == 'deviceName') {
                      queue.addCommand(command_id: CommonCommand.NAME_CHANGE.value, command_value: List<int>.from(Uint8List.fromList(utf8.encode(infoController.text)) as Iterable<dynamic>), method: CommunicationMethod.SEND_BY_AUTO);
                      setState(() {
                        deviceName = infoController.text;
                      });
                    } else if (cmd == 'tagID') {
                      queue.addCommand(command_id: CommonCommand.TAG_CHANGE.value, command_value: List<int>.from(Uint8List.fromList(utf8.encode(infoController.text)) as Iterable<dynamic>), method: CommunicationMethod.SEND_BY_AUTO);
                      setState(() {
                        tagID =  infoController.text;
                      });
                    }
                    Navigator.pop(context);
                  },
                  child: const Text('Đồng ý', style: TextStyle(color: Color(0xFF5B6EF5), fontSize: 16, fontWeight: FontWeight.bold)),
                ),
              ],
            );
          },
        );
      },
    );
  }
  void _showConfirmDialog(String text, String cmd) {
      showDialog(
        context: context,
        builder: (BuildContext context) {
          return StatefulBuilder(
            builder: (context, setDialogState) {
              return AlertDialog(
                backgroundColor: const Color(0xFF1A1F2E),
                shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
                title: Text('Xác nhận $text ?', style: const TextStyle(color: Colors.white, fontSize: 18, fontWeight: FontWeight.bold)),
                actions: [
                  TextButton(
                    onPressed: () => Navigator.pop(context),
                    child: Text('Hủy bỏ', style: TextStyle(color: Colors.grey[400], fontSize: 16)),
                  ),
                  TextButton(
                    onPressed: () {
                      Navigator.pop(context);
                      switch (cmd) {
                        case 'restart':
                          queue.addCommand(command_id: CommonCommand.RESTART.value, command_value: [Status.ON.value], method: CommunicationMethod.SEND_BY_AUTO);
                          break;
                        case 'reset':
                          queue.addCommand(command_id: CommonCommand.RESET.value, command_value: [Status.ON.value], method: CommunicationMethod.SEND_BY_AUTO);
                          break;
                        case 'delete':
                          queue.addCommand(command_id: CommonCommand.DELETE.value, command_value: [Status.ON.value], method: CommunicationMethod.SEND_BY_AUTO);
                          break;
                        default:
                          break;
                      }
                    },
                    child: const Text('Đồng ý', style: TextStyle(color: Color(0xFF5B6EF5), fontSize: 16, fontWeight: FontWeight.bold)),
                  ),
                ],
              );
            },
          );
        },
      );

  }
  void _checkUpdateDialog() {
    showDialog(
      context: context,
      builder: (BuildContext context) {
        return StatefulBuilder(
          builder: (context, setDialogState) {
            return AlertDialog(
              backgroundColor: const Color(0xFF1A1F2E),
              shape: RoundedRectangleBorder(borderRadius: BorderRadius.circular(16)),
              title: _isCheckUpdateDone ?
                Text(_haveNewFirmware ? 'Đã có bản cập nhật mới' : 'Đây là phiên bản mới nhất', style: const TextStyle(color: Colors.white, fontSize: 18, fontWeight: FontWeight.bold))
                :
                Row(
                  children: [
                    Text('Đang kiểm tra cập nhật', style: const TextStyle(color: Colors.white, fontSize: 18, fontWeight: FontWeight.bold)),
                    SizedBox(width: 10),
                    CircularProgressIndicator()
                  ]
                ),
              content: Text("Version 1.0.0"),
              actions: [
                if(_isCheckUpdateDone)
                  TextButton(
                    onPressed: () => Navigator.pop(context),
                    child: Text(
                      'Hủy bỏ',
                      style: TextStyle(color: Colors.grey[400], fontSize: 16),
                    ),
                  ),
                  if (_haveNewFirmware)
                    TextButton(
                      onPressed: () {
                        Navigator.pop(context);
                      },
                      child: const Text(
                        'Cập nhật',
                        style: TextStyle(
                          color: Color(0xFF5B6EF5),
                          fontSize: 16,
                          fontWeight: FontWeight.bold,
                        ),
                      ),
                    ),
              ],
            );
          },
        );
      },
    );
  }
  @override
  Widget build(BuildContext context) {
    return SingleChildScrollView(
      child: Padding(
        padding: const EdgeInsets.all(20.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            const Text('Cài đặt', style: TextStyle(fontSize: 24, fontWeight: FontWeight.bold, color: Colors.white)),
            const SizedBox(height: 8),
            Text('Quản lý thiết bị và kết nối', style: TextStyle(fontSize: 14, color: Colors.grey[400])),
            const SizedBox(height: 24),

            // Device Info
            Container(
              padding: const EdgeInsets.all(20),
              decoration: BoxDecoration(
                color: const Color(0xFF1A1F2E),
                borderRadius: BorderRadius.circular(16),
                border: Border.all(color: Colors.grey.withOpacity(0.2), width: 1),
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  const Row(
                    children: [
                      Icon(Icons.info_outline, color: Colors.white, size: 20),
                      SizedBox(width: 12),
                      Text('Thông tin thiết bị', style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold, color: Colors.white)),
                    ],
                  ),
                  const SizedBox(height: 16),
                  Padding(
                    padding: const EdgeInsets.only(bottom: 12),
                    child: Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        Text('Tên thiết bị', style: TextStyle(fontSize: 14, color: Colors.grey[400])),
                        Expanded(
                            child: InkWell(
                              onTap: () => _changeInfoDialog(deviceName,'deviceName'),
                              child: Row(
                                mainAxisAlignment: MainAxisAlignment.end,
                                children: [
                                  Flexible(
                                    child: Text(
                                      deviceName,
                                      style: const TextStyle(fontSize: 14, color: Colors.white, fontWeight: FontWeight.w600),
                                      textAlign: TextAlign.right,
                                      overflow: TextOverflow.ellipsis,
                                    ),
                                  ),
                                  const Icon(Icons.chevron_right, color: Colors.white),
                                ],
                              ),
                            ),
                          ),
                      ],
                    ),
                  ),
                  Padding(
                    padding: const EdgeInsets.only(bottom: 12),
                    child: Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        Text('Tag ID', style: TextStyle(fontSize: 14, color: Colors.grey[400])),
                        Expanded(
                          child: InkWell(
                            onTap: () => _changeInfoDialog(tagID,'tagID'),
                            child: Row(
                              mainAxisAlignment: MainAxisAlignment.end,
                              children: [
                                Flexible(
                                  child: Text(
                                    '#${tagID}',
                                    style: const TextStyle(fontSize: 14, color: Colors.white, fontWeight: FontWeight.w600),
                                    textAlign: TextAlign.right,
                                    overflow: TextOverflow.ellipsis,
                                  ),
                                ),
                                const Icon(Icons.chevron_right, color: Colors.white),
                              ],
                            ),
                          ),
                        ),
                      ],
                    ),
                  ),
                  Padding(
                    padding: const EdgeInsets.only(bottom: 12),
                    child: Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        Text('Model', style: TextStyle(fontSize: 14, color: Colors.grey[400])),
                        Text('8x8x8 RGB', style: const TextStyle(fontSize: 14, color: Colors.white, fontWeight: FontWeight.w600))
                      ],
                    ),
                  ),
                  Padding(
                    padding: const EdgeInsets.only(bottom: 12),
                    child: Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        Text('ID thiết bị', style: TextStyle(fontSize: 14, color: Colors.grey[400])),
                        Text('12345678', style: const TextStyle(fontSize: 14, color: Colors.white, fontWeight: FontWeight.w600))
                      ],
                    ),
                  ),
                  Padding(
                    padding: const EdgeInsets.only(bottom: 12),
                    child: Row(
                      mainAxisAlignment: MainAxisAlignment.spaceBetween,
                      children: [
                        Text('Firmware version', style: TextStyle(fontSize: 14, color: Colors.grey[400])),
                        Text('v2.1.0', style: const TextStyle(fontSize: 14, color: Colors.white, fontWeight: FontWeight.w600))
                      ],
                    ),
                  ),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                    children: [
                      Text('Trạng thái', style: TextStyle(fontSize: 14, color: Colors.grey[400])),
                      Row(
                        children: [
                          Container(
                            width: 8,
                            height: 8,
                            decoration: const BoxDecoration(color: Color(0xFF4ADE80), shape: BoxShape.circle),
                          ),
                          const SizedBox(width: 8),
                          const Text('Đang kết nối', style: TextStyle(fontSize: 14, color: Color(0xFF4ADE80), fontWeight: FontWeight.w600))
                        ],
                      ),
                    ],
                  ),
                ],
              ),
            ),
            const SizedBox(height: 16),

            // WiFi Connection
            Container(
              padding: const EdgeInsets.all(20),
              decoration: BoxDecoration(
                color: const Color(0xFF1A1F2E),
                borderRadius: BorderRadius.circular(16),
                border: Border.all(color: Colors.grey.withOpacity(0.2), width: 1),
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  const Row(
                    children: [
                      Icon(Icons.wifi, color: Colors.white, size: 20),
                      SizedBox(width: 12),
                      Text('Kết nối', style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold, color: Colors.white)),
                    ],
                  ),
                  const SizedBox(height: 16),
                  InkWell(
                    onTap: _showWifiDialog,
                    child: Container(
                      padding: const EdgeInsets.all(16),
                      decoration: BoxDecoration(
                        color: const Color(0xFF0A0E1A),
                        borderRadius: BorderRadius.circular(12),
                      ),
                      child: Row(
                        children: [
                          Container(
                            width: 48,
                            height: 48,
                            decoration: BoxDecoration(
                              color: const Color(0xFF5B6EF5).withOpacity(0.2),
                              borderRadius: BorderRadius.circular(12),
                            ),
                            child: const Icon(Icons.wifi, color: Color(0xFF5B6EF5), size: 24),
                          ),
                          const SizedBox(width: 16),
                          Expanded(
                            child: Column(
                              crossAxisAlignment: CrossAxisAlignment.start,
                              children: [
                                const Text('WiFi', style: TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: Colors.white)),
                                const SizedBox(height: 4),
                                Text(_wifiNetwork, style: TextStyle(fontSize: 13, color: Colors.grey[400])),
                              ],
                            ),
                          ),
                          Switch(
                            value: _isWifiConnected,
                            onChanged: (value) {
                              queue.addCommand(command_id: BleCommand.WIFI_ON_OFF.value, command_value: value ? [Status.ON.value] : [Status.OFF.value], method: CommunicationMethod.SEND_BY_BLE);
                              setState(() {
                                _isWifiConnected = value;
                              });
                            },
                            activeColor: const Color(0xFF5B6EF5),
                          ),
                        ],
                      ),
                    ),
                  ),
                ],
              ),
            ),
            const SizedBox(height: 16),

            // Power Control
            Container(
              padding: const EdgeInsets.all(20),
              decoration: BoxDecoration(
                color: const Color(0xFF1A1F2E),
                borderRadius: BorderRadius.circular(16),
                border: Border.all(color: Colors.grey.withOpacity(0.2), width: 1),
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  const Row(
                    children: [
                      Icon(Icons.power_settings_new, color: Colors.white, size: 20),
                      SizedBox(width: 12),
                      Text('Nguồn điện và thông báo', style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold, color: Colors.white)),
                    ],
                  ),
                  const SizedBox(height: 16),
                  Container(
                    padding: const EdgeInsets.all(16),
                    decoration: BoxDecoration(
                      color: const Color(0xFF0A0E1A),
                      borderRadius: BorderRadius.circular(12),
                    ),
                    child: Row(
                      children: [
                        Container(
                          width: 48,
                          height: 48,
                          decoration: BoxDecoration(
                            color: Colors.yellow.withOpacity(0.2),
                            borderRadius: BorderRadius.circular(12),
                          ),
                          child: const Icon(Icons.lightbulb, color: Colors.yellow, size: 24),
                        ),
                        const SizedBox(width: 16),
                        Expanded(
                          child: Column(
                            crossAxisAlignment: CrossAxisAlignment.start,
                            children: [
                              const Text('Đèn LED', style: TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: Colors.white)),
                              const SizedBox(height: 4),
                              Text('Hiệu ứng đèn', style: TextStyle(fontSize: 13, color: Colors.grey[400])),
                            ],
                          ),
                        ),
                        Switch(
                          value: _ledPowerOn,
                          onChanged: (value) {
                            queue.addCommand(command_id: CommonCommand.LED_ON_OFF.value, command_value: value ? [Status.ON.value] : [Status.OFF.value], method: CommunicationMethod.SEND_BY_AUTO);
                            setState(() {
                              _ledPowerOn = value;
                            });
                          },
                          activeColor: const Color(0xFF5B6EF5),
                        ),
                      ],
                    ),
                  ),
                  const SizedBox(height: 16),
                  Container(
                    padding: const EdgeInsets.all(16),
                    decoration: BoxDecoration(
                      color: const Color(0xFF0A0E1A),
                      borderRadius: BorderRadius.circular(12),
                    ),
                    child: Row(
                      children: [
                        Container(
                          width: 48,
                          height: 48,
                          decoration: BoxDecoration(
                            color: const Color(0xFF4ADE80).withOpacity(0.2),
                            borderRadius: BorderRadius.circular(12),
                          ),
                          child: const Icon(Icons.volume_up, color: Color(0xFF4ADE80), size: 24),
                        ),
                        const SizedBox(width: 16),
                        Expanded(
                          child: Column(
                            crossAxisAlignment: CrossAxisAlignment.start,
                            children: [
                              const Text('Tin nhắn voice', style: TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: Colors.white)),
                              const SizedBox(height: 4),
                              Text('Âm thanh tin nhắn', style: TextStyle(fontSize: 13, color: Colors.grey[400])),
                            ],
                          ),
                        ),
                        Switch(
                          value: _voiceNotify,
                          onChanged: (value) {
                            queue.addCommand(command_id: CommonCommand.VOICE_ON_OFF.value, command_value: value ? [Status.ON.value] : [Status.OFF.value], method: CommunicationMethod.SEND_BY_AUTO);
                            setState(() {
                              _voiceNotify = value;
                            });
                          },
                          activeColor: const Color(0xFF5B6EF5),
                        ),
                      ],
                    ),
                  ),
                ],
              ),
            ),
            const SizedBox(height: 16),

            // Sleep Timer
            Container(
              padding: const EdgeInsets.all(20),
              decoration: BoxDecoration(
                color: const Color(0xFF1A1F2E),
                borderRadius: BorderRadius.circular(16),
                border: Border.all(color: Colors.grey.withOpacity(0.2), width: 1),
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  const Row(
                    children: [
                      Icon(Icons.bedtime, color: Colors.white, size: 20),
                      SizedBox(width: 12),
                      Text('Hẹn giờ ngủ', style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold, color: Colors.white)),
                    ],
                  ),
                  const SizedBox(height: 16),
                  InkWell(
                    onTap: _sleepTimerEnabled ? _showSleepTimerDialog : null,
                    child: Container(
                      padding: const EdgeInsets.all(16),
                      decoration: BoxDecoration(
                        color: const Color(0xFF0A0E1A),
                        borderRadius: BorderRadius.circular(12),
                      ),
                      child: Row(
                        children: [
                          Container(
                            width: 48,
                            height: 48,
                            decoration: BoxDecoration(
                              color: const Color(0xFF8B5CF6).withOpacity(0.2),
                              borderRadius: BorderRadius.circular(12),
                            ),
                            child: const Icon(Icons.schedule, color: Color(0xFF8B5CF6), size: 24),
                          ),
                          const SizedBox(width: 16),
                          Expanded(
                            child: Column(
                              crossAxisAlignment: CrossAxisAlignment.start,
                              children: [
                                const Text('Tự động tắt', style: TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: Colors.white)),
                                const SizedBox(height: 4),
                                Text(
                                    '${_sleepStartTime.hour.toString().padLeft(2, '0')}:${_sleepStartTime.minute.toString().padLeft(2, '0')} - ${_sleepEndTime.hour.toString().padLeft(2, '0')}:${_sleepEndTime.minute.toString().padLeft(2, '0')}',
                                    style: TextStyle(fontSize: 13, color: Colors.grey[400]),
                                ),
                              ],
                            ),
                          ),
                          Switch(
                            value: _sleepTimerEnabled,
                            onChanged: (value) {
                              queue.addCommand(command_id: CommonCommand.AUTO_OFF.value, command_value: value ? [Status.ON.value] : [Status.OFF.value], method: CommunicationMethod.SEND_BY_AUTO);
                              setState(() {
                                _sleepTimerEnabled = value;
                              });
                            },
                            activeColor: const Color(0xFF5B6EF5),
                          ),
                        ],
                      ),
                    ),
                  ),
                ],
              ),
            ),

            const SizedBox(height: 16),

            // System setting
            Container(
              padding: const EdgeInsets.all(20),
              decoration: BoxDecoration(
                color: const Color(0xFF1A1F2E),
                borderRadius: BorderRadius.circular(16),
                border: Border.all(color: Colors.grey.withOpacity(0.2), width: 1),
              ),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  const Row(
                    children: [
                      Icon(Icons.hardware, color: Colors.white, size: 20),
                      SizedBox(width: 12),
                      Text('Hệ thống', style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold, color: Colors.white)),
                    ],
                  ),
                  const SizedBox(height: 16),
                  // Test Mode
                  InkWell(
                    // onTap: _testModeDialog,
                    child: Container(
                      padding: const EdgeInsets.all(16),
                      decoration: BoxDecoration(
                        color: const Color(0xFF0A0E1A),
                        borderRadius: BorderRadius.circular(12),
                      ),
                      child: Row(
                        children: [
                          Container(
                            width: 48,
                            height: 48,
                            decoration: BoxDecoration(
                              color: Colors.purple.withOpacity(0.2),
                              borderRadius: BorderRadius.circular(12),
                            ),
                            child: const Icon(Icons.add_chart, color: Colors.purple, size: 24),
                          ),
                          const SizedBox(width: 16),
                          Expanded(
                            child: Column(
                              crossAxisAlignment: CrossAxisAlignment.start,
                              children: [
                                const Text('Chế độ Test', style: TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: Colors.white)),
                                const SizedBox(height: 4),
                                Text('Đưa về trạng thái trình diễn', style: TextStyle(fontSize: 13, color: Colors.grey[400])),
                              ],
                            ),
                          ),
                        ],
                      ),
                    ),
                  ),

                  const SizedBox(height: 16),
                  // Check update
                  InkWell(
                    onTap: _checkUpdateDialog,
                    child: Container(
                      padding: const EdgeInsets.all(16),
                      decoration: BoxDecoration(
                        color: const Color(0xFF0A0E1A),
                        borderRadius: BorderRadius.circular(12),
                      ),
                        child: Row(
                          children: [
                            Container(
                              width: 48,
                              height: 48,
                              decoration: BoxDecoration(
                                color: Colors.blue.withOpacity(0.2),
                                borderRadius: BorderRadius.circular(12),
                              ),
                              child: const Icon(Icons.download, color: Colors.blue, size: 24),
                            ),
                            const SizedBox(width: 16),
                            Expanded(
                              child: Column(
                                crossAxisAlignment: CrossAxisAlignment.start,
                                children: [
                                  const Text('Kiểm tra cập nhật', style: TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: Colors.white)),
                                  const SizedBox(height: 4),
                                  Text('Tìm kiếm phiên bản mới nhất', style: TextStyle(fontSize: 13, color: Colors.grey[400])),
                                ],
                              ),
                            ),
                          ],
                        ),
                      ),
                  ),

                  const SizedBox(height: 16),

                  //Restart
                  InkWell(
                    onTap: () => _showConfirmDialog('Khởi động lại', 'restart'),
                    child: Container(
                      padding: const EdgeInsets.all(16),
                      decoration: BoxDecoration(
                        color: const Color(0xFF0A0E1A),
                        borderRadius: BorderRadius.circular(12),
                      ),
                        child: Row(
                          children: [
                            Container(
                              width: 48,
                              height: 48,
                              decoration: BoxDecoration(
                                color: Colors.green.withOpacity(0.2),
                                borderRadius: BorderRadius.circular(12),
                              ),
                              child: const Icon(Icons.refresh, color: Colors.green, size: 24),
                            ),
                            const SizedBox(width: 16),
                            Expanded(
                              child: Column(
                                crossAxisAlignment: CrossAxisAlignment.start,
                                children: [
                                  const Text('Khởi động lại', style: TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: Colors.white)),
                                  const SizedBox(height: 4),
                                  Text('Làm mới thiết bị', style: TextStyle(fontSize: 13, color: Colors.grey[400])),
                                ],
                              ),
                            ),
                          ],
                        ),
                      ),
                  ),

                  const SizedBox(height: 16),

                  // Reset Device
                  InkWell(
                    onTap: () => _showConfirmDialog('Đặt lại','reset'),
                    child: Container(
                      padding: const EdgeInsets.all(16),
                      decoration: BoxDecoration(
                        color: const Color(0xFF0A0E1A),
                        borderRadius: BorderRadius.circular(12),
                      ),
                        child: Row(
                          children: [
                            Container(
                              width: 48,
                              height: 48,
                              decoration: BoxDecoration(
                                color: Colors.orange.withOpacity(0.2),
                                borderRadius: BorderRadius.circular(12),
                              ),
                              child: const Icon(Icons.cached, color: Colors.orange, size: 24),
                            ),
                            const SizedBox(width: 16),
                            Expanded(
                              child: Column(
                                crossAxisAlignment: CrossAxisAlignment.start,
                                children: [
                                  const Text('Đặt lại', style: TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: Colors.white)),
                                  const SizedBox(height: 4),
                                  Text('Đưa về trạng thái nhà sản xuất', style: TextStyle(fontSize: 13, color: Colors.grey[400])),
                                ],
                              ),
                            ),
                          ],
                        ),
                      ),
                  ),
                  const SizedBox(height: 16),

                  // Delete Device
                  InkWell(
                    onTap: () => _showConfirmDialog('Xóa thiết bị','delete'),
                    child: Container(
                      padding: const EdgeInsets.all(16),
                      decoration: BoxDecoration(
                        color: const Color(0xFF0A0E1A),
                        borderRadius: BorderRadius.circular(12),
                      ),
                        child: Row(
                          children: [
                            Container(
                              width: 48,
                              height: 48,
                              decoration: BoxDecoration(
                                color: Colors.red.withOpacity(0.2),
                                borderRadius: BorderRadius.circular(12),
                              ),
                              child: const Icon(Icons.delete_outline, color: Colors.red, size: 24),
                            ),
                            const SizedBox(width: 16),
                            Expanded(
                              child: Column(
                                crossAxisAlignment: CrossAxisAlignment.start,
                                children: [
                                  const Text('Xóa thiết bị', style: TextStyle(fontSize: 16, fontWeight: FontWeight.w600, color: Colors.white)),
                                  const SizedBox(height: 4),
                                  Text('Gỡ bỏ khỏi danh sách', style: TextStyle(fontSize: 13, color: Colors.grey[400])),
                                ],
                              ),
                            ),
                          ],
                        ),
                      ),
                  ),
                ],
              ),
            ),
            const SizedBox(height: 32),
            // App Version
            Center(
              child: Text('Cubik LED App v1.0.0', style: TextStyle(fontSize: 13, color: Colors.grey[600])),
            ),
            const SizedBox(height: 24),
          ],
        ),
      ),
    );
  }

  Widget _buildInfoRow(String label, String value) {
    return Padding(
      padding: const EdgeInsets.only(bottom: 12),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        children: [
          Text(label, style: TextStyle(fontSize: 14, color: Colors.grey[400])),
          Text(value, style: const TextStyle(fontSize: 14, color: Colors.white, fontWeight: FontWeight.w600)),
          const Icon(Icons.chevron_right, color: Colors.white),
        ],
      ),
    );
  }
}