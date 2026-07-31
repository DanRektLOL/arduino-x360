#include <Bluepad32.h>

ControllerPtr myControllers[BP32_MAX_CONTROLLERS];

// Compact 16-byte binary packet sent over Serial to Arduino Uno
struct __attribute__((packed)) ControllerPacket {
  uint8_t header1 = 0xAA;
  uint8_t header2 = 0xBB;
  int16_t lx, ly, rx, ry;
  uint16_t l2, r2;
  uint16_t buttons;
};

void setup() {
  // Main Hardware Serial (Pin 1 TX -> Uno Pin 0 RX)
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

      // Pack analog stick and trigger values
      ControllerPacket packet;
      packet.lx = ctl->axisX();
      packet.ly = ctl->axisY();
      packet.rx = ctl->axisRX();
      packet.ry = ctl->axisRY();
      packet.l2 = ctl->brake();
      packet.r2 = ctl->throttle();

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

      // Send 16-byte raw binary frame to Uno over Serial
      Serial.write((uint8_t*)&packet, sizeof(packet));
    }
  }
}
