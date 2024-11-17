# <img src="docs/icon.jpg" alt="synscancontrol icon" width="64"/> synscancontrol

This project is an open source drop-in replacement for the motherboard of the popular Sky-Watcher HEQ5 (Orion Sirius EQ-G) mount using the SynScan protocol.

## Features

- Drop-in replacement from the original HEQ5 motherboard
  - Compatible with the SynScan hand controller
  - Compatible with EQMOD
- ESP32-based project with plenty of onboard memory to spare
  - If you wanted a WiFi or Bluetooth version of the HEQ5, this or [Open-Synscan](https://github.com/vsirvent/Open-Synscan) is a good place to start.

## TODO

- Implement the autoguider port interface in software (does anybody use this?)

## Motivation

My original motherboard failed and the (really expensive) replacement I got from Sky-Watcher had a problem and wouldn't drive one motor. Sky-Watcher couldn't replicate the problem, and wouldn't replace it since the mount I have is from Orion, not Sky-Watcher.

...So I designed my own instead to never have this problem again (it's also much cheaper!).

## Hardware Details

[**Link to Project on OSHWLab**](https://oshwlab.com/j_dalrym2/synscan-control-rev-1_copy_copy)

The project is based on the [HiLetGo NodeMCU ESP-32S](http://www.hiletgo.com/ProductDetail/1906566.html). Here is an [Amazon link to the one I have](https://www.amazon.com/gp/product/B0718T232Z). I will include a chart of this microcontroller's pinout in case a clone of this board matching the same pinout might work.

Other key hardware components are:
- 2x DRV8825 stepper motor driver board ([Amazon link](https://www.amazon.com/gp/product/B07XF2LYC8), set the current limiter to about 1.5A)
- 1x ADUM1201 magnetic isolator board ([Amazon link](https://www.amazon.com/gp/product/B0725BJMTQ))
- Some sort of 5v regular board with a 4-pin header ([Amazon link](https://www.amazon.com/gp/product/B08JZ5FVLC))

I have included all of the PCB development files and images of the components I used under the [./docs](./docs) folder in this repository.

### Bill of Materials
|                Name                 | Designator |                                   Comments                                     |
|:------------------------------------|:----------:|:-------------------------------------------------------------------------------|
|1x HiLetGo NodeMCU ESP-32S           |U1          |Microcontroller                                                                 |
|1x ADUM1201                          |U5          |Converts 5V hand controller logic to 3.3V MCU logic                             |
|2x DRV8825                           |U2,U3       |Stepper motor drivers                                                           |
|1x 1μF 0805 Capacitor                |C1          |MCU RST/EN Capacitor (improves reliability of automatic reset circuit)          |
|2x 470Ω 0805 Resistor                |R1          |For Power LED                                                                   |
|1x 1kΩ 0805 Resistor                 |R2          |Polar Scope LED resistor                                                        |
|2x 10kΩ 0805 Resistor                |R7,R8       |Improves hand controller serial reliability                                     |
|1x Header Jumper                     |5V ENABLE   |Enable regulator's 5V output (disconnect to power the MCU via USB in isolation) |
|2x JST-PH 4-Pin Connector            |DEC,RA      |For DEC/RA cables                                                               |
|1x JST-PH 5-Pin Connector            |HC          |For SynScan hand controller / serial port                                       |
|1x JST-PH 2-Pin Connector            |PSLED       |For Polar Scope LED                                                             |
|4x 10kΩ 0805 Resistor (Optional)     |R3,R4,R5,R6 |Autoguider Port Pullup Resistors                                                |
|1x JST-PH 6-Pin Connector (Optional) |AG          |For autoguider cable (not implemented in software)                              |

## Software Details

This project was built in [VS Code](https://code.visualstudio.com/) using the [PlatformIO](https://platformio.org/) extension for embedded devices. See [platformio.ini](platformio.ini) for the specific PlatformIO configuration for this project, including build flags.

### OTA Updates (`-DOTA_UPDATES`)

### UDP Logging (`-DUDP_LOGGING`)

### Serial Timeout (`-DSERIAL_TIMEOUT`)

## Credits

- [Open-Synscan](https://github.com/vsirvent/Open-Synscan) inspired me to do this project, and a lot of the reverse engineering of the SynScan protocol provided by this product is helpful. A lot of the serial bus logic for this project is similar to Open-Synscan. Licensed under GPLv3.
- [AccelStepper](https://www.airspayce.com/mikem/arduino/AccelStepper/) was really useful to help me understand how you can implement stepper motor acceleration in software. I couldn't use it as-is, but it did prove to be a helpful reference nonetheless. Licensed under GPLv3.

## License

[GPLv3](https://choosealicense.com/licenses/gpl-3.0/) &copy; 2024 Jon Dalrymple