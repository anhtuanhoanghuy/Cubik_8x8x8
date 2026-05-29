enum Status {
  OFF(0x00),
  ON(0X01),
  OFFLINE(0X30),
  ONLINE(0X31);

  final int value;
  const Status(this.value);
}
// ============================
// Communication Method
// ============================

enum CommunicationMethod {
  SEND_BY_WIFI,
  SEND_BY_BLE,
  SEND_BY_AUTO
}

// ============================
// Command via Bluetooth
// ============================

enum BleCommand {
  WIFI_SCAN(0x00),
  WIFI_CONNECT(0x01),
  WIFI_DISCONNECT(0x02),
  WIFI_ON_OFF(0x03),
  TEST_MODE(0x04);

  final int value;
  const BleCommand(this.value);
}

// ============================
// Command via Bluetooth/WiFi
// ============================

enum CommonCommand {
  NAME_CHANGE(0x05),
  TAG_CHANGE(0x06),
  LED_ON_OFF(0x07),
  VOICE_ON_OFF(0x08),
  AUTO_OFF(0x09),
  AUTO_OFF_SET_TIME(0x0A),
  RESTART(0x0B),
  RESET(0x0C),
  DELETE(0x0D),
  LED_MODE(0x0E),
  SPEED(0x0F),
  BRIGHTNESS(0x10);

  final int value;
  const CommonCommand(this.value);
}

// ============================
// Command via WiFi
// ============================

enum WifiCommand {
  CHECK_UPDATE(0x11),
  MESSAGE(0x12);

  final int value;
  const WifiCommand(this.value);
}

// ============================
// LED Mode
// ============================

enum LedMode {
  RAINBOW(0x01),
  WAVE(0x02),
  PULSE(0x03),
  MUSIC(0x04),
  HEART(0x05),
  STAR(0x06),
  CUSTOM(0x07);

  final int value;
  const LedMode(this.value);
}
