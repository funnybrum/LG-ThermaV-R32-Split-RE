#include "Main.h"
#include "SoftwareSerial.h"

ThermaV::ThermaV() {
    _bufferIndex = 0;
    ss = new SoftwareSerial();
}

void ThermaV::begin() {
    ss->enableRxGPIOPullup(false);
    ss->begin(300, SWSERIAL_8N1, 16, 17);
    _lastByteTimestamp = 0;
}

void ThermaV::loop() {
    while (ss->available() & _bufferIndex < 1000) {
        _buffer[_bufferIndex] = ss->read();
        _bufferIndex++;
        _lastByteTimestamp = millis();
    }

    // If no package is received in 30 seconds - reset the SofwareSerial
    if (millis() - _knownPackageEndTime > 30000) {
        logger.log("SS reset");
        delete ss;
        delay(100);
        ss = new SoftwareSerial();
        ss->enableRxGPIOPullup(false);
        ss->begin(300, SWSERIAL_8N1, 16, 17);
        _knownPackageEndTime = millis();
    }

    if (debug) {
        if (ss->bufferIndex >= 100 && ss->bufferIndex < 1000) {
            for (int i = 0; i < ss->bufferIndex; i++) {
                logger.log("%2.2f %d", ss->cycles[i]/800000.0, ss->leves[i]);
            }
            ss->bufferIndex = 1000;
        }
    }
    // logger.log("%d", ss->interrupts);

    if (ss->overflow()) {
        logger.log("Overflow");
    }

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
                _count,
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
                            logger.log("Valid package received: 0xC6 0x%02X", _buffer[1]);
                    }
                    break;
                default:
                    logger.log("Valid package received: 0x%02X", _buffer[0]);
            }
        } else {
            logger.log("Invalid package received: 0x%02X", _buffer[0]);
        }
        _count++;
        _bufferIndex = 0;
    }

}

void ThermaV::setDebug(bool on) {
    this->debug = on;
    ss->bufferIndex = 0;
}

void append(uint8_t cmd[20], uint32_t count) {
    logger.log("[%5d] %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X  %02X %02X %02X %02X %02X",
        count,
        cmd[0],  cmd[1],  cmd[2],  cmd[3],  cmd[4],
        cmd[5],  cmd[6],  cmd[7],  cmd[8],  cmd[9],
        cmd[10], cmd[11], cmd[12], cmd[13], cmd[14],
        cmd[15], cmd[16], cmd[17], cmd[18], cmd[19]);
}

void ThermaV::getOutput(char* buf) {
    logger.log("Seconds since last command: %d / last known command: %d", (millis() - _packageEndTime)/1000, (millis() - _knownPackageEndTime) / 1000);
    append(_a0Command, _a0CommandCount);
    append(_a5Command, _a5CommandCount);
    append(_a6Command, _a6CommandCount);
    append(_c0Command, _c0CommandCount);
    append(_c5Command, _c5CommandCount);
    append(_c600Command, _c600CommandCount);
    append(_c601Command, _c601CommandCount);
    append(_c602Command, _c602CommandCount);
    append(_c603Command, _c603CommandCount);
}


float ThermaV::getFlow() {
    // C6 01 00 00 00  00 00 00 05 09  00 00 3C 00 00  F8 01 15 00 4A
    // C6 01 00 00 00  00 00 00 05 10  00 00 3C 00 00  F8 00 F2 00 57
    //                                                    [pump flow]
    //  00 F2 -> 24.2L
    //  01 15 -> 27.7L

    // The C6 packages are 1 per 60 seconds. If there is no package for 65+
    // seconds - there is communication problem. 
    if (millis() - _c601CommandTs > 65000) {
        return -100;
    }
    float flow = ((_c601Command[16]<<8) + _c601Command[17]) * 0.1f;
    // logger.log("%02X %02X ... %02X %02X %.1f", _c601Command[0], _c601Command[1],_c601Command[16], _c601Command[17], flow);
    return flow;
}

int8_t ThermaV::getDeltaT() {
    // The A0 packages are 1 per 20 seconds. If there is no package for 30+
    // seconds - there is communication problem. 
    if (millis() - _a0CommandTs > 30000) {
        return -100;
    }

    return _a0Command[12] - _a0Command[11];
}

float ThermaV::getOutputPower() {
    float power = getDeltaT() * getFlow() * 60 * 4200 / 3600;
    logger.log("%d %.1f %d", getDeltaT(), getFlow(), power);
    return power;
}