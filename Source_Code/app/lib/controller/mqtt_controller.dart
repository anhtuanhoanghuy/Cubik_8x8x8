import 'dart:io';
import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

const String mqttServerUrl = '35a196d8b54146f08f917c8c382e1c0a.s1.eu.hivemq.cloud';
const String mqttServerName = 'HELLO_CUBIK_8X8X8';
const String mqttServerPassword = 'hello_cubik_8X8X8';
const String topicName = 'Cubik/30102002/Status';
// connection states for easy identification
enum MqttCurrentConnectionState {
  IDLE,
  CONNECTING,
  CONNECTED,
  DISCONNECTED,
  ERROR_WHEN_CONNECTING
}

enum MqttSubscriptionState {
  IDLE,
  SUBSCRIBED
}

class MQTTClientWrapper {

  late MqttServerClient client;

  MqttCurrentConnectionState connectionState = MqttCurrentConnectionState.IDLE;
  MqttSubscriptionState subscriptionState = MqttSubscriptionState.IDLE;

  // using async tasks, so the connection won't hinder the code flow
  void prepareMqttClient() async {
    _setupMqttClient();
    await _connectClient();
    _subscribeToTopic(topicName);
    publishMessage('Hello_Cubik');
  }

  // waiting for the connection, if an error occurs, print it and disconnect
  Future<void> _connectClient() async {
    try {
      print('client connecting....');
      connectionState = MqttCurrentConnectionState.CONNECTING;
      await client.connect(mqttServerName, mqttServerPassword);
    } on Exception catch (e) {
      print('client exception - $e');
      connectionState = MqttCurrentConnectionState.ERROR_WHEN_CONNECTING;
      client.disconnect();
    }

    // when connected, print a confirmation, else print an error
    if (client.connectionStatus!.state == MqttConnectionState.connected) {
      connectionState = MqttCurrentConnectionState.CONNECTED;
      _setupMessageListener();
      print('client connected');
    } else {
      print(
          'ERROR client connection failed - disconnecting, status is ${client.connectionStatus}');
      connectionState = MqttCurrentConnectionState.ERROR_WHEN_CONNECTING;
      client.disconnect();
    }
  }

  void _setupMqttClient() {
    client = MqttServerClient.withPort(mqttServerUrl, mqttServerName, 8883);
    // the next 2 lines are necessary to connect with tls, which is used by HiveMQ Cloud
    client.secure = true;
    client.securityContext = SecurityContext.defaultContext;
    client.keepAlivePeriod = 20;
    client.onDisconnected = _onDisconnected;
    client.onConnected = _onConnected;
    client.onSubscribed = _onSubscribed;
    }

  void _subscribeToTopic(String topicName) {
    print('Subscribing to the $topicName topic');
    client.subscribe(topicName, MqttQos.atMostOnce);
  }

  void _setupMessageListener() {
    client.updates!.listen((List<MqttReceivedMessage<MqttMessage>> c) {
      try {
        final MqttPublishMessage recMess = c[0].payload as MqttPublishMessage;

        String topic = c[0].topic;

        // ✅ Lấy bytes trực tiếp (không cần Uint8Buffer)
        List<int> bytes = recMess.payload.message.toList();

        // Log
        String hex = bytes.map((b) =>
        '0x${b.toRadixString(16).toUpperCase().padLeft(2, '0')}'
        ).join(' ');

        print('📥 MQTT Received:');
        print('   Topic: $topic');
        print('   Bytes: $hex');
        print('   Length: ${bytes.length}');

        // Try parse as string
        String message = MqttPublishPayload.bytesToStringAsString(
            recMess.payload.message
        );
        print('   String: $message');

        // Handle message
        // _handleMessage(topic, bytes);

      } catch (e) {
        print('❌ Error handling message: $e');
      }
    });
  }

  void publishMessage(String message) {
    final MqttClientPayloadBuilder builder = MqttClientPayloadBuilder();
    builder.addString(message);

    print('Publishing message "$message" to topic $topicName');
    client.publishMessage(topicName, MqttQos.atLeastOnce, builder.payload!);
  }

  // ✅ Publish raw bytes
  Future<void> publishBytes(List<int> data) async {
    try {
      final builder = MqttClientPayloadBuilder();
      // ✅ Add từng byte (đơn giản nhất!)
      for (int byte in data) {
        builder.addByte(byte);
      }
      final payload = builder.payload;
      if (payload != null) {
        client.publishMessage(topicName, MqttQos.atLeastOnce, payload);
        print('📤 MQTT Published: [$topicName] ${data.length} bytes');
      }

    } catch (e) {
      print('❌ MQTT Publish error: $e');
      throw e;
    }

    await Future.delayed(Duration(milliseconds: 200));
  }

  // callbacks for different events
  void _onSubscribed(String topic) {
    print('Subscription confirmed for topic $topic');
    subscriptionState = MqttSubscriptionState.SUBSCRIBED;
  }

  void _onDisconnected() {
    print('OnDisconnected client callback - Client disconnection');
    connectionState = MqttCurrentConnectionState.DISCONNECTED;
  }

  void _onConnected() {
    connectionState = MqttCurrentConnectionState.CONNECTED;
    print('OnConnected client callback - Client connection was sucessful');
  }

}