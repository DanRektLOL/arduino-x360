#include <Bluepad32.h>

ControllerPtr myControllers[BP32_MAX_CONTROLLERS];

// Compact 16-byte binary packet sent over Serial to Arduino
struct __attribute__((packed)) ControllerPacket {
  uint8_t header1 = 0xAA;
  uint8_t header2 = 0xBB;
  int16_t lx, ly, rx, ry;
  uint16_t l2, r2;
  uint16_t buttons;
};

// --- Tightened Deadzone Function ---
int16_t cleanRawAxis(int raw) {
  // 1. Reduced deadzone cutoff (4 instead of 15) for tight responsiveness
  if (abs(raw) < 5) {
    return 0;
  }

  // 2. Hardware scaling: maps non-symmetric limits (-508 UP, 512 DOWN) to clean -512..512
  if (raw < 0) {
    return (int16_t)map(constrain(raw, -508, 0), -508, 0, -512, 0);
  } else {
    return (int16_t)map(constrain(raw, 0, 512), 0, 512, 0, 512);
  }
}

void setup() {
  // Main Hardware Serial
  Serial.begin(115200);

  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.enableVirtualDevice(false); // Disables virtual mouse emulation for touchpad
}

void loop() {
  if (BP32.update()) {
    processControllers();
  }
  vTaskDelay(1);
}

void onConnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
    if (myControllers[i] == nullptr) {
      myControllers[i] = ctl;
      ctl->setColorLED(255, 0, 0); // Solid RED light bar on successful pair
      break;
    }
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
    if (myControllers[i] == ctl) {
      myControllers[i] = nullptr;
      break;
    }
  }
}

void processControllers() {
  for (auto ctl : myControllers) {
    if (ctl && ctl->isConnected() && ctl->isGamepad()) {
      
      // DISCONNECT SHORTCUT: Hold PS Button + Press Triangle
      if (ctl->miscSystem() && ctl->y()) {
        ctl->disconnect();
        return;
      }

      // Pack analog stick and trigger values with tightened 5-unit deadzone
      ControllerPacket packet;
      packet.lx = cleanRawAxis(ctl->axisX());
      packet.ly = cleanRawAxis(ctl->axisY());
      packet.rx = cleanRawAxis(ctl->axisRX());
      packet.ry = cleanRawAxis(ctl->axisRY());

      // Clamp triggers to ensure 100% squeeze hits full range
      int rawL2 = map(constrain(ctl->brake(), 0, 1023), 0, 1023, 0, 255);
      int rawR2 = map(constrain(ctl->throttle(), 0, 1023), 0, 1023, 0, 255);
      packet.l2 = (rawL2 >= 254) ? 255 : rawL2;
      packet.r2 = (rawR2 >= 254) ? 255 : rawR2;

      // Check Touchpad click state
      uint16_t miscMask = ctl->miscButtons();
      uint16_t btnMask  = ctl->buttons();
      bool touchpadClick = (miscMask & 0x0004) || (miscMask & 0x0008) || (btnMask & 0x0400);

      // Pack buttons into bitmask
      uint16_t b = 0;
      if (ctl->a())          b |= (1 << 0);  // Cross
      if (ctl->b())          b |= (1 << 1);  // Circle
      if (ctl->x())          b |= (1 << 2);  // Square
      if (ctl->y())          b |= (1 << 3);  // Triangle
      if (ctl->l1())         b |= (1 << 4);  // L1
      if (ctl->r1())         b |= (1 << 5);  // R1
      if (ctl->thumbL())     b |= (1 << 6);  // L3 (Left Stick Click)
      if (ctl->thumbR())     b |= (1 << 7);  // R3 (Right Stick Click)
      
      uint8_t dpad = ctl->dpad();
      if (dpad & DPAD_UP)    b |= (1 << 8);  // D-Pad Up
      if (dpad & DPAD_DOWN)  b |= (1 << 9);  // D-Pad Down
      if (dpad & DPAD_LEFT)  b |= (1 << 10); // D-Pad Left
      if (dpad & DPAD_RIGHT) b |= (1 << 11); // D-Pad Right
      
      if (ctl->miscSelect()) b |= (1 << 12); // SHARE
      if (ctl->miscStart())  b |= (1 << 13); // OPTIONS
      if (ctl->miscSystem()) b |= (1 << 14); // PS Button
      if (touchpadClick)     b |= (1 << 15); // Touchpad Click

      packet.buttons = b;

      // Send 16-byte raw binary frame over Serial
      Serial.write((uint8_t*)&packet, sizeof(packet));
    }
  }
}
