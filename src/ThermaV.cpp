#include "Main.h"

HardwareSerial MySerial(2);

ThermaV::ThermaV() {
    _bufferIndex = 0;
}

void ThermaV::begin() {
    MySerial.setRxBufferSize(512);
    MySerial.begin(300);
    _lastByteTimestamp = millis();
    _packageEndTime = millis();
    _knownPackageEndTime = millis();
}

void ThermaV::loop() {
    while (MySerial.available() && _bufferIndex < 32) {
        _buffer[_bufferIndex] = MySerial.read();
        _bufferIndex++;
        _lastByteTimestamp = millis();
    }

    // If no package is received in 30 seconds - restart
    // if (millis() - _knownPackageEndTime > 60000) {
    //     // dataCollector.forcePush();
    //     // ESP.restart();
    // }

    /*
     300bps with SWSERIAL_8N1 is 30 bytes per second or 33.3ms per byte.

     Gaps between bytes >= 34ms should be considered as separate package.

     However async processing with delays results in issues with 34ms gap.

     So if byte gap is > 100ms or there are >= 20 bytes in the buffer - print the package.

     This can be further optimized by regularly invoking the ss->read loop after each byte is
     printed out, but only if the current approach show flaws.
     */
    if (_bufferIndex >= 20 ||
        (millis() - _lastByteTimestamp > 100 && _bufferIndex > 1)) {
        _packageEndTime = millis();

        uint16_t crc = 0;
        for (int i = 0; i < 19; i++) {
            crc += _buffer[i];
        }
        crc = crc & 0xFF;
        crc = crc ^ 0x55;

        if (debug) {
            logger.log("[%5d] %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X  CRC: %02X",
                _packagesCount,
                _buffer[0],  _buffer[1],  _buffer[2],  _buffer[3],  _buffer[4],
                _buffer[5],  _buffer[6],  _buffer[7],  _buffer[8],  _buffer[9],
                _buffer[10], _buffer[11], _buffer[12], _buffer[13], _buffer[14],
                _buffer[15], _buffer[16], _buffer[17], _buffer[18], _buffer[19],
                crc);
        }

        if (crc == _buffer[19]) {
            switch (_buffer[0]) {
                case 0xA0:
                    memcpy(_a0Command, _buffer, 20);
                    _a0CommandCount++;
                    _a0CommandTs = millis();
                    _knownPackageEndTime = millis();
                    break;
                case 0xA5:
                    memcpy(_a5Command, _buffer, 20);
                    _a5CommandCount++;
                    _knownPackageEndTime = millis();
                    break;
                case 0xA6:
                    memcpy(_a6Command, _buffer, 20);
                    _a6CommandCount++;
                    _knownPackageEndTime = millis();
                    break;
                case 0xC0:
                    memcpy(_c0Command, _buffer, 20);
                    _c0CommandCount++;
                    _knownPackageEndTime = millis();
                    break;
                case 0xC5:
                    memcpy(_c5Command, _buffer, 20);
                    _c5CommandCount++;
                    _knownPackageEndTime = millis();
                    break;
                case 0xC6:
                    switch (_buffer[1]) {
                        case 0x00:
                            memcpy(_c600Command, _buffer, 20);
                            _c600CommandCount++;
                            _knownPackageEndTime = millis();
                            break;
                        case 0x01:
                            memcpy(_c601Command, _buffer, 20);
                            _c601CommandCount++;
                            _c601CommandTs = millis();
                            _knownPackageEndTime = millis();
                            _freshC601 = true;
                            break;
                        case 0x02:
                            memcpy(_c602Command, _buffer, 20);
                            _c602CommandCount++;
                            _knownPackageEndTime = millis();
                            break;
                        case 0x03:
                            memcpy(_c603Command, _buffer, 20);
                            _c603CommandCount++;
                            _knownPackageEndTime = millis();
                            break;
                        default:
                            _unknownPackagesCount++;
                            logger.log("Valid package received: 0xC6 0x%02X", _buffer[1]);
                            break;
                    }
                    break;
                default:
                    _unknownPackagesCount++;
                    logger.log("Valid package received: 0x%02X", _buffer[0]);
                    break;
            }
        } else {
            logger.log("Invalid package received: 0x%02X", _buffer[0]);
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
    logger.log("since last: %d / since last known: %d / unknwon: %d / invalid: %d / silence = %.1f, DS18B20 = %.1f / %d",
        (millis() - _packageEndTime)/1000,
        (millis() - _knownPackageEndTime) / 1000,
        _unknownPackagesCount,
        _invalidPackagesCount,
        getIdleMs() / 1000.0,
        (millis() - tempSensors._lastReadMs) / 1000.0,
        tempSensors._count);
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
    // C6 01 00 00 00  00 00 00 05 09  00 00 3C 00 00  F8 01 15 00 4A
    //  00 F2 -> 24.2L

    // C6 01 00 00 00  00 00 00 05 10  00 00 3C 00 00  F8 00 F2 00 57
    //  01 15 -> 27.7L

    //                                                    [pump flow]

    // The C6 packages are 1 per 60 seconds. If there is no package for 65+
    // seconds - there is communication problem. 
    if (millis() - _c601CommandTs > 90000) {
        return -100;
    }
    float flow = ((_c601Command[16]<<8) + _c601Command[17]) * 0.1f;
    // logger.log("%02X %02X ... %02X %02X %.1f", _c601Command[0], _c601Command[1],_c601Command[16], _c601Command[17], flow);
    return flow;
}

int8_t ThermaV::getDeltaT() {
    // The A0 packages are 1 per 20 seconds. If there is no package for 65+
    // seconds - there is communication problem. 
    if (millis() - _a0CommandTs > 65000) {
        return -100;
    }

    return _a0Command[12] - _a0Command[11];
}

float ThermaV::getOutputPower() {
    if (getFlow() < 5.1) {
        // 5 means the pump is not running.
        return 0;
    }
 
    float deltaT = tempSensors.getOutflowTemp() - tempSensors.getInflowTemp();
    float power = deltaT * getFlow() * 60 * 4200 / 3600;
    return power;
}

bool ThermaV::freshC601() {
    return _freshC601;
}

void ThermaV::resetFreshC601() {
    _freshC601 = false;
}

uint32_t ThermaV::getIdleMs() {
    return millis() - _lastByteTimestamp;
}