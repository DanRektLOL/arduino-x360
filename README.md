# arduino-x360
An xbox 360-related arduino project

(WIP) A custom arduino leonardo + esp32 setup which translates ps4 (or most bluetooth controllers, really) input via bluetooth into xinput which a jailbroken xbox 360 reads, with the help of the usbdsecpatch plugin.

feel free to contact me at discord @danrekt, if you have a problem.

I'm still working on the Arduino Leonardo side of things at the moment, but the esp32 requires richardoquesada's [bluepad32](https://github.com/ricardoquesada/bluepad32) library/board to be installed, and the Leonardo will require dmadison's [xinput library](https://github.com/dmadison/ArduinoXInput).
