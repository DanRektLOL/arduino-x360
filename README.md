# arduino-x360
An xbox 360-related arduino project

A custom arduino leonardo + esp32 setup which translates ps4 (or bluepad32 compatible controllers) input via bluetooth into xinput which a jailbroken xbox 360 can read, with the help of the [usbdsecpatch plugin](https://github.com/InvoxiPlayGames/UsbdSecPatch).

Version 1.0 has been released which features no rumble and noticeable input delay.

# Compatible ESP32 boards

ESP32 (devkit v1) (known to work)

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

Simply flash your esp32 (model must support bluetooth classic) with the given code and flash your Arduino Leonardo, with the given code, plug the leonardo into a jailbroken x360 with [usbdsecpatch](https://github.com/InvoxiPlayGames/UsbdSecPatch) as a plugin in dashlaunch, run a wire from leonardo 5v to esp 5v, and establish common ground, and join the esp tx to leonardo rx.


In order to pair your controller, put it into pairing mode (hold the share and playstation button), the led should blink fast, and keep it near the esp, and it should connect. Your controller should vibrate for a moment, and its led should turn red. It is possible that it will first connect, then disconnect after a few seconds with a red led light. This is normal, and after this, putting the controller into pair mode again, should successfully connect.

also, as is stated in the usbdsecpatch readme, the leonardo will have to be reconnected after the splash animation, after which you can connect your controller.
