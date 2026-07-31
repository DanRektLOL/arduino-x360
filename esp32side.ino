#include <Bluepad32.h>

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            myControllers[i] = ctl;
            
            ctl->setColorLED(255, 0, 0);
            ctl->playDualRumble(0, 100, 250, 0); 
            break;
        }
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            myControllers[i] = nullptr;
            break;
        }
    }
}

int cleanRawAxis(int raw) {
    if (abs(raw) < 5) {
        return 0;
    }

    if (raw < 0) {
        return map(constrain(raw, -508, 0), -508, 0, -512, 0);
    } else {
        return map(constrain(raw, 0, 512), 0, 512, 0, 512);
    }
}

void processGamepad(ControllerPtr ctl) {
    uint16_t buttons = ctl->buttons();
    uint16_t miscButtons = ctl->miscButtons();

    if ((miscButtons & MISC_BUTTON_SYSTEM) && (miscButtons & MISC_BUTTON_START)) {
        ctl->disconnect();
        return;
    }

    int btnCross    = (buttons & BUTTON_A) ? 1 : 0;
    int btnCircle   = (buttons & BUTTON_B) ? 1 : 0;
    int btnSquare   = (buttons & BUTTON_X) ? 1 : 0;
    int btnTriangle = (buttons & BUTTON_Y) ? 1 : 0;
    
    int btnL1       = (buttons & BUTTON_SHOULDER_L) ? 1 : 0;
    int btnR1       = (buttons & BUTTON_SHOULDER_R) ? 1 : 0;

    int btnL3       = (buttons & BUTTON_THUMB_L) ? 1 : 0;
    int btnR3       = (buttons & BUTTON_THUMB_R) ? 1 : 0;

    int btnSelect   = (miscButtons & MISC_BUTTON_SELECT) ? 1 : 0;
    int btnStart    = (miscButtons & MISC_BUTTON_START)  ? 1 : 0;
    int btnPS       = (miscButtons & MISC_BUTTON_SYSTEM) ? 1 : 0;

    uint8_t dpad = ctl->dpad();
    int dpadUp    = (dpad & DPAD_UP) ? 1 : 0;
    int dpadDown  = (dpad & DPAD_DOWN) ? 1 : 0;
    int dpadLeft  = (dpad & DPAD_LEFT) ? 1 : 0;
    int dpadRight = (dpad & DPAD_RIGHT) ? 1 : 0;

    int rawL2 = map(constrain(ctl->brake(), 0, 1023), 0, 1023, 0, 255);
    int rawR2 = map(constrain(ctl->throttle(), 0, 1023), 0, 1023, 0, 255);
    
    int triggerL2 = (rawL2 >= 254) ? 255 : rawL2;
    int triggerR2 = (rawR2 >= 254) ? 255 : rawR2;

    int axisX  = cleanRawAxis(ctl->axisX());
    int axisY  = cleanRawAxis(ctl->axisY());
    int axisRX = cleanRawAxis(ctl->axisRX());
    int axisRY = cleanRawAxis(ctl->axisRY());

    Serial.print(btnCross);    Serial.print(",");
    Serial.print(btnCircle);   Serial.print(",");
    Serial.print(btnSquare);   Serial.print(",");
    Serial.print(btnTriangle); Serial.print(",");
    Serial.print(btnL1);       Serial.print(",");
    Serial.print(btnR1);       Serial.print(",");
    Serial.print(btnL3);       Serial.print(",");
    Serial.print(btnR3);       Serial.print(",");
    Serial.print(btnSelect);   Serial.print(",");
    Serial.print(btnStart);    Serial.print(",");
    Serial.print(btnPS);       Serial.print(",");
    Serial.print(dpadUp);      Serial.print(",");
    Serial.print(dpadDown);    Serial.print(",");
    Serial.print(dpadLeft);    Serial.print(",");
    Serial.print(dpadRight);   Serial.print(",");
    Serial.print(triggerL2);   Serial.print(",");
    Serial.print(triggerR2);   Serial.print(",");
    Serial.print(axisX);       Serial.print(",");
    Serial.print(axisY);       Serial.print(",");
    Serial.print(axisRX);      Serial.print(",");
    Serial.println(axisRY);
}

void setup() {
    Serial.begin(115200);

    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();
}

void loop() {
    BP32.update();

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        ControllerPtr myController = myControllers[i];
        if (myController && myController->isConnected()) {
            processGamepad(myController);
        }
    }
    delay(4);
}
