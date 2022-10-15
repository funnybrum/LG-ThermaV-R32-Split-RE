#include "Main.h"

HardwareSerial MySerial(2);

ThermaV::ThermaV() {
    _bufferIndex = 0;
}

void ThermaV::begin() {
    MySerial.begin(300, SERIAL_8N1, 16, 17);
    _lastByteTimestamp = millis();
    _packageEndTime = millis();
}

void ThermaV::loop() {
    while (MySerial.available() && _bufferIndex < 20) {
        _lastByteTimestamp = millis();
        _buffer[_bufferIndex] = MySerial.read();
        _bufferIndex++;
    }

    /*
     300bps with SWSERIAL_8N1 is 30 bytes per second or 33.3ms per byte.

     Gaps between bytes >= 34ms should be considered as separate package.

     Async processing with delays results in issues with 34ms gap.

     So if byte gap is > 100ms or there are 20 bytes in the buffer - process the package.

     The approach handles startup during package transmission.
     */
    if (_bufferIndex == 20 ||
        (millis() - _lastByteTimestamp > 100 && _bufferIndex > 1)) {
        _packageEndTime = millis();

        uint16_t crc = 0;
        for (int i = 0; i < 19; i++) {
            crc += _buffer[i];
        }
        crc = crc & 0xFF;
        crc = crc ^ 0x55;

        if (debug) {
            logPackage(crc);
        }

        if (crc == _buffer[19]) {
            switch (_buffer[0]) {
                case 0xA0:
                    memcpy(_a0Command, _buffer, 20);
                    _a0CommandCount++;
                    _a0CommandTs = millis();
                    break;
                case 0xA5:
                    memcpy(_a5Command, _buffer, 20);
                    _a5CommandCount++;
                    break;
                case 0xA6:
                    memcpy(_a6Command, _buffer, 20);
                    _a6CommandCount++;
                    break;
                case 0xC0:
                    memcpy(_c0Command, _buffer, 20);
                    _c0CommandCount++;
                    break;
                case 0xC5:
                    memcpy(_c5Command, _buffer, 20);
                    _c5CommandCount++;
                    break;
                case 0xC6:
                    switch (_buffer[1]) {
                        case 0x00:
                            memcpy(_c600Command, _buffer, 20);
                            _c600CommandCount++;
                            break;
                        case 0x01:
                            memcpy(_c601Command, _buffer, 20);
                            _c601CommandCount++;
                            _c601CommandTs = millis();
                            _C601IsFresh = true;
                            break;
                        case 0x02:
                            memcpy(_c602Command, _buffer, 20);
                            _c602CommandCount++;
                            break;
                        case 0x03:
                            memcpy(_c603Command, _buffer, 20);
                            _c603CommandCount++;
                            break;
                        default:
                            _invalidPackagesCount++;
                            logger.log("Valid package received: 0xC6 0x%02X", _buffer[1]);
                            break;
                    }
                    break;
                default:
                    _invalidPackagesCount++;
                    break;
            }
        } else {
            _invalidPackagesCount++;
        }
        _packagesCount++;
        _bufferIndex = 0;
    }

}

void ThermaV::setDebug(bool on) {
    this->debug = on;
}

void append(uint8_t cmd[20], uint32_t count, uint32_t timestamp=0) {

    uint32_t receivedBefore = 0;
    if (timestamp != 0) {
        receivedBefore = (millis() - timestamp) / 1000;
    }

    logger.log("[%5d][%2d] %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X",
        count, receivedBefore,
        cmd[0],  cmd[1],  cmd[2],  cmd[3],  cmd[4],
        cmd[5],  cmd[6],  cmd[7],  cmd[8],  cmd[9],
        cmd[10], cmd[11], cmd[12], cmd[13], cmd[14],
        cmd[15], cmd[16], cmd[17], cmd[18], cmd[19]);
}

void ThermaV::getOutput() {
    logger.log("seconds since last package: %d / invalid: %d",
        (millis() - _packageEndTime)/1000,
        _invalidPackagesCount);
    append(_a0Command, _a0CommandCount, _a0CommandTs);
    append(_a5Command, _a5CommandCount);
    append(_a6Command, _a6CommandCount);
    append(_c0Command, _c0CommandCount);
    append(_c5Command, _c5CommandCount);
    append(_c600Command, _c600CommandCount);
    append(_c601Command, _c601CommandCount, _c601CommandTs);
    append(_c602Command, _c602CommandCount);
    append(_c603Command, _c603CommandCount);
}


float ThermaV::getFlow() {
    return ((_c601Command[16]<<8) + _c601Command[17]) * 0.1f;
}

uint8_t ThermaV::getInflowTemp() {
    return _a0Command[11];
}

uint8_t ThermaV::getOutflowTemp() {
    return _a0Command[12];
}

float ThermaV::getOutputPower() {
    float flow = getFlow();
    if (getFlow() < 5.1) {
        // 5.0L/min means that the pump is not running.
        return 0;
    }
 
    float deltaT = tempSensors.getOutflowTemp() - tempSensors.getInflowTemp();
    // 4186j/g*C is the water specific heat energy
    // 60 - to convert the l/s to l/h
    // 3600 - seconds in 1 hour
    // The formula is dT * flow in l/m * 60 * 4186 / 3600
    // The result is in kW.
    return deltaT * getFlow() * 4186 / 60;
}

bool ThermaV::freshC601() {
    return _C601IsFresh;
}

void ThermaV::resetFreshC601() {
    _C601IsFresh = false;
}

uint32_t ThermaV::getIdleMs() {
    return millis() - _lastByteTimestamp;
}

void ThermaV::logPackage(uint8_t crc) {
    logger.log("[%5d] %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X  CRC: %02X",
        _packagesCount,
        _buffer[0],  _buffer[1],  _buffer[2],  _buffer[3],  _buffer[4],
        _buffer[5],  _buffer[6],  _buffer[7],  _buffer[8],  _buffer[9],
        _buffer[10], _buffer[11], _buffer[12], _buffer[13], _buffer[14],
        _buffer[15], _buffer[16], _buffer[17], _buffer[18], _buffer[19],
        crc);
}