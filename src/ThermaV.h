#pragma once

#include "SoftwareSerial.h"

class ThermaV {
    public:
        ThermaV();
        void begin();
        void loop();

        float getFlow();
        int8_t getDeltaT();
        float getOutputPower();

        void setDebug(bool on);
        void getOutput();
    private:
        uint8_t _buffer[32];
        uint8_t _bufferIndex;
        uint32_t _lastByteTimestamp;
        uint32_t _packageEndTime;
        uint32_t _knownPackageEndTime;
        uint16_t _packagesCount = 0;
        uint16_t _unknownPackagesCount = 0;
        uint16_t _invalidPackagesCount = 0;

        uint8_t _a0Command[20];
        uint8_t _a5Command[20];
        uint8_t _a6Command[20];
        uint8_t _c0Command[20];
        uint8_t _c5Command[20];
        uint8_t _c600Command[20];
        uint8_t _c601Command[20];
        uint8_t _c602Command[20];
        uint8_t _c603Command[20];
        uint32_t _a0CommandCount = 0;
        uint32_t _a0CommandTs = 0;
        uint32_t _a5CommandCount = 0;
        uint32_t _a6CommandCount = 0;
        uint32_t _c0CommandCount = 0;
        uint32_t _c5CommandCount = 0;
        uint32_t _c600CommandCount = 0;
        uint32_t _c601CommandCount = 0;
        uint32_t _c601CommandTs = 0;
        uint32_t _c602CommandCount = 0;
        uint32_t _c603CommandCount = 0;

        bool debug = false;

        SoftwareSerial* ss;
};
