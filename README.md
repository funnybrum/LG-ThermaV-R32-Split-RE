# THERMA V R32 Split

A project used for reverse engineering the THERMA V R32 Split controller communication protocol.

Foundations are based on the data provided by Sascha Kloß at [https://github.com/cribskip/esp8266_lgawhp].

The goal is to validate his approach and once ready - to merge it as part of the [https://github.com/funnybrum/HeatPump] project. The functionality provide there will be extended by:
1) COP data added in the telemtry.
2) Ability to change temperature up and down with goal to achieve specific power consumption. This will be used to optimize the photovoltaic power consumption.

Current upgrades include implementation over the [ESP32 blank](https://github.com/funnybrum/ESP32Blank) project. The result is HTTP interface for setting up the WiFi and support for retrieving captured commands using an HTTP call:
```
curl http://{hostname/address}/debug; curl http://{hostname/address}/logs
```

This exposes something like:
```
since last package: 0 / invalid: 0
[    7][ 0] A0 B0 00 00 40  16 20 00 22 2D  00 17 18 13 2C  00 06 58 00 B4
[    6][ 0] A5 00 22 00 C0  00 00 00 00 00  00 00 00 00 00  00 00 00 00 D2
[    6][ 0] A6 02 00 0B B8  19 19 19 19 19  19 19 19 00 00  00 00 00 00 66
[    2][ 0] C0 B0 00 00 40  00 00 00 22 2D  00 17 18 13 2C  00 00 58 00 90
[    2][ 0] C5 00 22 00 C0  00 00 00 00 00  00 00 00 00 00  00 00 00 00 F2
[    2][ 0] C6 00 00 00 D7  1D 00 00 00 00  00 00 53 41 00  00 00 16 02 33
[    2][37] C6 01 00 00 00  00 00 00 00 16  00 00 4B 00 01  1A 00 32 00 20
[    2][ 0] C6 02 00 0B B8  19 19 19 19 19  19 19 19 00 00  00 00 00 00 06
[    2][ 0] C6 03 0A 0A 0A  0A 0A 0A 0A 0A  00 00 00 00 0F  00 0C 0B B8 A2
```

The format is:
```
[{package count}][{seconds since last package}] {package bytes}
```

There are total of 9 regular packages. The one in the example above.

Packages starting with A0, A5 and A6 are send on regular intervals of ~20 seconds from the controller.
Packages starting with C0, C5 and C6 are send on regular intervals of ~65 seconds from the heat pump.


# Bulding the project
Check the [ESP32 blank](https://github.com/funnybrum/ESP32Blank) for details on how to get the `ESP32 Base` library.

# Hardware
Simmilar to the one provided by provided by Sascha Kloß at [https://github.com/cribskip/esp8266_lgawhp], but with RX pin connected only at GPIO16. Once the protocol is reverse engineered a better hardware will be included as part of the [https://github.com/funnybrum/HeatPump] project.

# Notes
1) The RX part can be implemented with simpler [voltage divider](https://en.wikipedia.org/wiki/Voltage_divider). This will reduce the load on the communication pull down circuit.

2) OOB Bidirectional level shifter solutions come with built in pull up resistor on both sides. If such is used it is a good idea to remove the HV pull up resistor and to disable the RX pin internal pull up (by calling `ss.enableRxGPIOPullup(false)`). The first will reduce the pull up current on the communication line, the second will reduce the load on the pull down circuit of the communication line.

3) The main loop should be low. Check the Software Serial basics for details, but shortly - with 100+ ms dealy between read operations the Software Serial will overflow. This will halt its operation. A workaround for slower loop would be to increase the SoftwareSerial `bufCapacity` above 64. This is done in the `.begin(...)` call.

# Notes 2
1) There were issues with the RX part. It was updated to address the issues by replacing it with tow passive elements:
  - 1K pullup resistor from 3.3V to GPIO16
  - Shotkey diode from GPIO16 (anode) to the heat-pump communication wire (cathode to the yellow wire for the heat pump controller)

This improved communication and all packages are having valid CRC now.

2) There is an issue with the SoftwareSerial - with heavy load to the web server the SoftwareSerial is failing at certain point. The only fix seems to be controller restart.

3) The heat-pump flow data was identified. This is the C6 01 prefied package. Two bytes provide the flow. The formula is (package[16] * 256 + package[17]) * 0.1f. Below are two package examples.

    // C6 01 00 00 00  00 00 00 05 09  00 00 3C 00 00  F8 01 15 00 4A
    //  00 F2 -> 24.2L

    // C6 01 00 00 00  00 00 00 05 10  00 00 3C 00 00  F8 00 F2 00 57
    //  01 15 -> 27.7L

4) The logging output was improved to enable better tracking of the package updates. Invoke the http://{hostname}/debug API and then the http://{hostname}/logs API. This will output the last version of the received commands. The following is an example:

```
Seconds since last command: 0 / last known command: 0
[    7] A0 B2 02 02 40  1B 20 00 22 2D  00 1E 22 13 2C  00 06 58 00 A8
[    6] A5 02 22 00 C0  00 00 00 00 00  00 00 00 00 00  00 00 00 00 DC
[    6] A6 02 00 0B B8  19 19 19 19 19  19 19 19 00 00  00 00 00 00 66
[    4] C0 B2 02 02 40  00 00 00 22 2D  00 1E 22 13 2C  00 00 58 00 89
[    4] C5 02 22 00 C0  00 00 00 00 00  00 00 00 00 00  00 00 00 00 FC
[    4] C6 00 00 00 BA  28 00 00 00 00  00 00 43 63 00  00 01 D8 02 7C
[    4] C6 01 00 00 00  00 00 00 01 D8  00 00 3E 00 00  FA 00 5B 00 66
[    4] C6 02 00 0B B8  19 19 19 19 19  19 19 19 00 00  00 00 00 00 06
[    4] C6 03 0A 0A 0A  0A 0A 0A 0A 0A  00 00 00 00 0E  00 13 0B B8 A8
```
The first column (in the brackets) shows the number of successfully received packages. The other reprent the last version of the 9 different packages that the pump sends.

5) For output power calculation the built in temperature sensor is inadequate. It measures the temperature in resolution of 1C. 1C difference at 30l/min flow is ~2kW. To address this two DS18B20 sensors will be added.


# Notes 3
1) There are two type of packages:
  - starting with 0xA0, 0xA5 and 0xA6 are packages from the controller
  - starting with 0xC0, 0xC5, 0xC6 0x01, 0xC6 0x02, 0xC6 0x03 - are packages from the heat pump

2) 0xA0 and 0xC0 seems identical. 0xA0 is likely trying to set the HP state and 0xC0 reports the current heat pump state back to the controller.

3) Serial communication works better with HardwareSerial port. The SoftwareSerial uses interrupts and is not compatible with the OneWire library. The latest will disable interrupts during communication with OneWire devices and this will render the SoftwareSerial non-working during that time.