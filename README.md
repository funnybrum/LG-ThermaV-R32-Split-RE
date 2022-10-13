# THERMA V R32 Split

A project used for reverse engineering the THERMA V R32 Split controller communication protocol.

Foundations are based on the data provided by Sascha Kloß at [https://github.com/cribskip/esp8266_lgawhp].

The goal is to validate his approach and once ready - to merge it as part of the [https://github.com/funnybrum/HeatPump] project. The functionality provide there will be extended by:
1) COP data added in the telemtry.
2) Ability to change temperature up and down with goal to achieve specific power consumption. This will be used to optimize the photovoltaic power consumption.

Current upgrades include implementation over the [ESP32 blank](https://github.com/funnybrum/ESP32Blank) project. The result is HTTP interface for setting up the WiFi and support for retrieving captured commands using an HTTP call:
```
curl http://{hostname/address}/logs
```

This eposes something like:
```
[   10][  5][ 6951] R:  a0 b2 02 02 40 16 20 00 22 2d 24 12 2c 00 06 40 00
[   11][  6][  201] R:  a0 b2 02 02 40 16 20 00 22 2c 00 01 24 12 2c 00 06 40 00 b1
[   12][  7][  169] R:  a5 02 00 00 00 00 00 00 00 00 00 00 00 00 00 dc
[   13][  8][  169] R:  a5 02 22 00 c0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 dc
[   14][  9][  271] R:  a6 02 00 0b b8 18 01 19 01 01 19 18 00 00 00 00 00 00 66
[   15][ 10][  268] R:  a6 02 00 b8 19 19 19 19 00 19 19 19 00 00 00 00 00 00 66
```

The format is:
```
[{seconds since startup}][{package count}][{gap between packages in ms}] R:  {package bytes}
```

# Bulding the project
Check the [ESP32 blank](https://github.com/funnybrum/ESP32Blank) for details on how to get the `ESP32 Base` library.

# Hardware
Simmilar to the one provided by provided by Sascha Kloß at [https://github.com/cribskip/esp8266_lgawhp], but with RX pin connected only at GPIO16.

Once the protocol is reverse engineered a better hardware will be included as part of the [https://github.com/funnybrum/HeatPump] project.

# Notes
1) The RX part can be implemented with simpler [voltage divider](https://en.wikipedia.org/wiki/Voltage_divider). This will reduce the load on the communication pull down circuit.

2) OOB Bidirectional level shifter solutions come with built in pull up resistor on both sides. If such is used it is a good idea to remove the HV pull up resistor and to disable the RX pin internal pull up (by calling `ss.enableRxGPIOPullup(false)`). The first will reduce the pull up current on the communication line, the second will reduce the load on the pull down circuit of the communication line.

3) The main loop should be low. Check the Software Serial basics for details, but shortly - with 100+ ms dealy between read operations the Software Serial will overflow. This will halt its operation. A workaround for slower loop would be to increase the SoftwareSerial `bufCapacity` above 64. This is done in the `.begin(...)` call.
