#pragma once

#include "SoftwareSerial.h"

class ThermaV {
    public:
        ThermaV();
        void begin();
        void loop();

    private:
        uint8_t _buffer[1024];
        uint8_t _bufferIndex;
        uint32_t _lastByteTimestamp;
        uint32_t _packageEndTime;
        uint32_t _packageGap;
        uint16_t _count = 0;

        SoftwareSerial* ss;
};
