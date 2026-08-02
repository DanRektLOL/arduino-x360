#include <Bluepad32.h>

ControllerPtr myControllers[BP32_MAX_CONTROLLERS];

struct __attribute__((packed)) ControllerPacket {
  uint8_t header1 = 0xAA;
  uint8_t header2 = 0xBB;
  int16_t lx, ly, rx, ry;
  uint16_t l2, r2;
  uint16_t buttons;
};

void onConnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_CONTROLLERS; i++) {
    if (myControllers[i] == nullptr) {
      myControllers[i] = ctl;
      ctl->setColorLED(255, 0, 0);
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
      
      // PS + START to disconnect
      if (ctl->miscSystem() && ctl->miscStart()) {
        ctl->disconnect();
        return;
      }

      ControllerPacket packet;
      
      packet.lx = ctl->axisX();
      packet.ly = ctl->axisY();
      packet.rx = ctl->axisRX();
      packet.ry = ctl->axisRY();

      packet.l2 = ctl->brake();
      packet.r2 = ctl->throttle();

      uint16_t miscMask = ctl->miscButtons();
      uint16_t btnMask  = ctl->buttons();
      bool touchpadClick = (miscMask & 0x0004) || (miscMask & 0x0008) || (btnMask & 0x0400);

      uint16_t b = 0;
      if (ctl->a())          b |= (1 << 0);
      if (ctl->b())          b |= (1 << 1);
      if (ctl->x())          b |= (1 << 2);
      if (ctl->y())          b |= (1 << 3);
      if (ctl->l1())         b |= (1 << 4);
      if (ctl->r1())         b |= (1 << 5);
      if (ctl->thumbL())     b |= (1 << 6);
      if (ctl->thumbR())     b |= (1 << 7);
      
      uint8_t dpad = ctl->dpad();
      if (dpad & DPAD_UP)    b |= (1 << 8);
      if (dpad & DPAD_DOWN)  b |= (1 << 9);
      if (dpad & DPAD_LEFT)  b |= (1 << 10);
      if (dpad & DPAD_RIGHT) b |= (1 << 11);
      
      if (ctl->miscSelect()) b |= (1 << 12);
      if (ctl->miscStart())  b |= (1 << 13);
      if (ctl->miscSystem()) b |= (1 << 14);
      if (touchpadClick)     b |= (1 << 15);

      packet.buttons = b;

      Serial.write((uint8_t*)&packet, sizeof(packet));
    }
  }
}

void setup() {
  Serial.begin(500000);
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.enableVirtualDevice(false);
}

void loop() {
  if (BP32.update()) {
    processControllers();
  }
  yield();
}
