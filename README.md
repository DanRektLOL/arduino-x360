# arduino-x360
An xbox 360-related arduino project

(WIP) A custom arduino leonardo + esp32 setup which translates ps4 (or most bluetooth controllers, really) input via bluetooth into xinput which a jailbroken xbox 360 reads, with the help of the [usbdsecpatch plugin](https://github.com/InvoxiPlayGames/UsbdSecPatch).

feel free to contact me at discord @danrekt, if you have a problem.

I'm still working on the Arduino Leonardo side of things at the moment, but the esp32 requires richardoquesada's [bluepad32](https://github.com/ricardoquesada/bluepad32) library/board to be installed, and the Leonardo will require dmadison's [xinput library](https://github.com/dmadison/ArduinoXInput). The arduino leonardo can be substituted for by many [other boards](https://github.com/dmadison/ArduinoXInput#compatible-boards).

# Compatible ESP32 boards

ESP32

WROOM (not tested)

WROVER (not tested)

The following boards are NOT compatible, as they lack bluetooth classic
ESP32-S2 

ESP32-S3

ESP32-C3

ESP32-C6

ESP32-C2

ESP32-H2 

ESP8266

I have tested an esp32-cam which did not work for me, but it may have been a hardware issue.

# Usage

Simply flash your esp32 (model must support bluetooth classic) with the given code, and your xinput-compatible board, such as the Arduino Leonardo, with the given code, plug the leonardo into a jailbroken x360 with [usbdsecpatch](https://github.com/InvoxiPlayGames/UsbdSecPatch) as a plugin, run a wire from leonardo 5v to esp 5v, and establish common ground, and join the esp tx to leonardo rx.


# Roadmap

actually release the leonardo code 

rumble support (will require leonardo tx to esp rx)
