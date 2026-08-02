#include <XInput.h>

struct __attribute__((packed)) ControllerPacket {
  uint8_t header1;
  uint8_t header2;
  int16_t lx, ly, rx, ry;
  uint16_t l2, r2;
  uint16_t buttons;
};

ControllerPacket packet;

void parseAndApplyInput() {
  XInput.setButton(BUTTON_A, (packet.buttons & (1 << 0)) != 0);
  XInput.setButton(BUTTON_B, (packet.buttons & (1 << 1)) != 0);
  XInput.setButton(BUTTON_X, (packet.buttons & (1 << 2)) != 0);
  XInput.setButton(BUTTON_Y, (packet.buttons & (1 << 3)) != 0);

  XInput.setButton(BUTTON_LB, (packet.buttons & (1 << 4)) != 0);
  XInput.setButton(BUTTON_RB, (packet.buttons & (1 << 5)) != 0);

  XInput.setButton(BUTTON_L3, (packet.buttons & (1 << 6)) != 0);
  XInput.setButton(BUTTON_R3, (packet.buttons & (1 << 7)) != 0);

  XInput.setDpad((packet.buttons & (1 << 8))  != 0,
                 (packet.buttons & (1 << 9))  != 0,
                 (packet.buttons & (1 << 10)) != 0,
                 (packet.buttons & (1 << 11)) != 0);

  bool sharePressed    = (packet.buttons & (1 << 12)) != 0;
  bool optionsPressed  = (packet.buttons & (1 << 13)) != 0;
  bool psPressed       = (packet.buttons & (1 << 14)) != 0;
  bool touchpadPressed = (packet.buttons & (1 << 15)) != 0;

  XInput.setButton(BUTTON_BACK,  sharePressed || touchpadPressed);
  XInput.setButton(BUTTON_START, optionsPressed);
  XInput.setButton(BUTTON_LOGO,  psPressed);

  XInput.setTrigger(TRIGGER_LEFT,  (uint8_t)packet.l2);
  XInput.setTrigger(TRIGGER_RIGHT, (uint8_t)packet.r2);

  // Scale sticks (-512..512 to XInput bounds) and invert Y axes
  int16_t lx = map(constrain(packet.lx, -512, 512), -512, 512, -32768, 32767);
  int16_t ly = map(constrain(-packet.ly, -512, 512), -512, 512, -32768, 32767);
  int16_t rx = map(constrain(packet.rx, -512, 512), -512, 512, -32768, 32767);
  int16_t ry = map(constrain(-packet.ry, -512, 512), -512, 512, -32768, 32767);

  XInput.setJoystick(JOY_LEFT,  lx, ly);
  XInput.setJoystick(JOY_RIGHT, rx, ry);

  XInput.send();
}

void setup() {
  Serial1.begin(500000);
  XInput.setAutoSend(false);
  XInput.begin();
}

void loop() {
  bool newPacketReceived = false;

  // Drain buffer to grab the newest packet frame
  while (Serial1.available() >= sizeof(ControllerPacket)) {
    if (Serial1.peek() != 0xAA) {
      Serial1.read();
      continue;
    }

    Serial1.readBytes((char*)&packet, sizeof(ControllerPacket));

    if (packet.header1 == 0xAA && packet.header2 == 0xBB) {
      newPacketReceived = true;
    }
  }

  if (newPacketReceived) {
    parseAndApplyInput();
  }
}
