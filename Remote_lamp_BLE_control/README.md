# Remote lamp BLE control tool
An ESP32-based tool for remote lamp control, utilizing a few reverse engineered communication protocols. This allows controlling several different types of BLE lamp controllers. It has functions for:

* sending CCT, brightness, on/off, setup and other commands
* sending raw advertising packets
* setting different attributes including the number of repeated packets and the time between them
* sniffing BLE packets

## BLE lamp control library
A part of the source code generates the BLE advertising packets which is essential for lamp control. The library can be used independently. It currently includes the following files:

* AdvData.cpp
* AdvData.h
* AdvProtocolF0.cpp
* AdvProtocolF0.h
* AdvProtocolF9.cpp
* AdvProtocolF9.h
* Cmd.h

## Short tool user guide
The tool is interfaced through usb serial. You should select the appropriate virtual COM port and set the baud rate to ***115200***. ***Don't forget to put line feed character at the end of every input.***

### List of commands
Every command starts with a forward slash.


/send {on|off|setup|cw|ww|neutral|sleep}
* cw, ww correspond to cool white and warm white, neutral means neutral white.

/send brightness {0-100}
* 0 = off
* 100 = max

/send cct_lvl {0-100}
* Correlated colour temperature (CCT) level where 0 and 100 stand for coldest and warmest cct and every value in between is a mix between cold and warm light.<br/><br/>


/sendraw {packet}
* Packet should be 1-31 bytes (2-62 chars) and should contain a data length byte as a first byte.
* The /scan command can be used first and then the desired packet could be sent with this command. Copying advertising data from Wireshark is done by selecting Advertising data > Right click > Copy > as a Hex Stream.<br/><br/>


/set protocol {F9|F0|ALL}
* Default = ALL
* The tool supports a number of protocols to be used with the /send command. Depending on the led controller a suitable protocol could be selected or it can be left at default state.

/set pks_to_send {1-20000}
* Default = 4
* How many advertising packets to send per command.

/set adv_delay {0-5000}
* Default = 150
* Time (in ms) before sending the next packet.

/set mode {count|time}
* Default = count
* How packets are expected to be sent when advertising. When advertising by time packets are sent for 5 seconds.

/set inverted {true|false}
* Default = false
* Inversion of warm and cold channels.<br/><br/>


/get_settings
* Returns all current settings.<br/><br/>


/scan {2-300} {1-100}
* First argument is for how much time to scan (in seconds). Second argument is used for an RSSI filter and only packets with higher RSSI value are shown. Its value is interpreted as a NEGATIVE number.
* /scan is actually a basic BLE packet sniffer.

## How to install
Setup your IDE then compile and upload the project to your ESP32 board.

### Setup Arduino IDE for ESP32 development
* Install [Arduino](https://www.arduino.cc/en/software), _tested on [VS Code Arduino extension v0.6.0](https://github.com/microsoft/vscode-arduino)_
* Go to `preferences/Additional Board Manager URLs` and paste the following URL: https://espressif.github.io/arduino-esp32/package_esp32_index.json 
_NOTE: if you already have some URLs you should split them with commas_
* Then install Espressif's official ESP32 Arduino Core by going to `Tools/Board/Boards Manager`. Search for "esp32" and install the latest version. It is normal to take a few minutes. [Arduino Core Source code](https://github.com/espressif/arduino-esp32), _tested on Arduino Core for ESP32 v2.0.9_
* Select board and set board preferences from `Tools` according to your development board hardware.

### Project setup
* Save the project to a directory of your choice.
* After you have verified your board preferences are set and you have selected the right virtual COM port from `Tools` you can proceed to uploading the project.
* When uploading finishes successfully you should open the Serial Monitor. Set the baud rate to _115200_ and line ending to _Newline_ or _LF_.
* You are ready to send commands. Don't forget that commands start with a forward slash. _More info is available in the section above._
