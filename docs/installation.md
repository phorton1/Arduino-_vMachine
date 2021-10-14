# The vMachine - Installation

**[Home](../readme.md)** --
**[History](history.md)** --
**[Hardware](hardware.md)** --
**[Electronics](electronics.md)** --
**[User Interface](ui.md)** --
**[Software](software.md)** --
**Installation**

Some of these instructions are duplicated on and/or reference other pages.

The main steps in building and flashing (installing the firmware) for the vMachine
on an ESP32 module are as follows:

- install the **Arduino IDE** - we assume you have alredy done this
- add the **ESP32 board** to the Arduino IDE
- copy or clone **this repository** into a folder called **_vMachine** in your Arduino sketch folder
- copy, clone, or otherwise install **dependent libraries** in the Arduino/libraries folder (detailed below)
- open the **_vMachine.ino** sketch in the Arduino IDE
- build and upload the firmware using the Arduino IDE Verify/Compile/Upload commands

Once the firmware is flashed to the ESP32 we need to upload a few files to the
SPIFFS file system on the ESP32. By default the firmware will boot as a WiFi
**Access Point** with an SSID of **FluidNC**.

- Connect to the **FluidNC** Wifi Network from your computer or other device.
- The default WiFi password is **12345678**
- You **should** be automatically presented with a webpage on the browser of your device that looks like the following:





- add "https://dl.espressif.com/dl/package_esp32_index.json" to the *Additional Board Manager URLS* in the Arduino IDE preferences window
- install the **esp32 by Espressif Systems** *board* into the Arduino IDE using the *Tools-Boards-Boards Manager* menu option
- connect an ESP32 via USB cable
- select the **ESP32 Dev Module** board from the Arduino *Tools-Boards* menu
- select the **Minimal SPIFFS** (1.9MB App with OTA/190KB SPIFFS) from the *Tools-Partition Scheme* menu
- select the correct **COM Port** for the ESP32


<br>
<hr>
<div style="text-align: right">
<a href='../readme.md'>HOME</a><i> - back to the beginning!</i>
</div>
