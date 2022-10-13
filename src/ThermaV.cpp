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
        if (_bufferIndex == 0) {
            _packageGap = millis() - _packageEndTime;
        }
        _buffer[_bufferIndex] = ss->read();
        _bufferIndex++;
        _lastByteTimestamp = millis();
    }

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

        String data = "";
        for (int i = 0; i < _bufferIndex; i++) {
            uint8_t byte = _buffer[i];
            String hexstring = " ";
            if (byte < 0x10) {
                hexstring += "0";
            }
            hexstring += String(byte, HEX);

            data += hexstring;
        }

        _count++;
        logger.log("[%5d][%3d][%5d] R: %s", millis() / 1000, _count, _packageGap, data.c_str());
        _bufferIndex = 0;
    }
}
