#pragma once

enum HeatPumpMode {
    HP_OFF,
    HP_HEAT,
    HP_COOL,
    HP_DHW_HEAT,
    HP_UNKNOWN
};

class ThermaV {
    public:
        ThermaV();
        void begin();
        void loop();

        float getFlow();
        uint8_t getOutflowTemp();
        uint8_t getInflowTemp();
        uint8_t getHeatingSetTemp();
        uint8_t getDhwSetTemp();
        float getIndoorTemp();
        bool isOutdoorUnitRunning();
        HeatPumpMode getMode();


        float getOutputPower();
        float getPumpOutputPower();

        void setDebug(bool on);
        void getOutput();

        static void onReceive();
        static void onReceiveError(hardwareSerial_error_t err);

        bool freshC601();
        void resetFreshC601();
        uint32_t getIdleMs();
        void logPackage(uint8_t crc);

        void setUfhTemp(uint8_t temp);
        void setDhwTemp(uint8_t temp);

    private:
        uint8_t getCrc(const uint8_t buf[20]);
        void sendUpdateCommand();

        uint8_t _buffer[20];
        uint8_t _bufferIndex;
        uint32_t _lastByteTimestamp;
        uint32_t _packageEndTime;
        uint16_t _packagesCount = 0;
        uint16_t _invalidPackagesCount = 0;
        uint32_t _skipped = 0;

        uint8_t _20Command[20];
        uint32_t _20CommandCount = 0;
        uint32_t _20CommandTs = 0;

        uint8_t _a0Command[20];
        uint32_t _a0CommandCount = 0;
        uint32_t _a0CommandTs = 0;

        uint8_t _a5Command[20];
        uint32_t _a5CommandCount = 0;

        uint8_t _a6Command[20];
        uint32_t _a6CommandCount = 0;

        uint8_t _c0Command[20];
        uint32_t _c0CommandCount = 0;

        uint8_t _c5Command[20];
        uint32_t _c5CommandCount = 0;

        uint8_t _c600Command[20];
        uint32_t _c600CommandCount = 0;

        uint8_t _c601Command[20];
        uint8_t _C601IsFresh = false;
        uint32_t _c601CommandCount = 0;
        uint32_t _c601CommandTs = 0;

        uint8_t _c602Command[20];
        uint32_t _c602CommandCount = 0;

        uint8_t _c603Command[20];
        uint32_t _c603CommandCount = 0;

        bool debug = false;

        uint8_t _ufhTemp = 0;
        uint8_t _dhwTemp = 0;
        bool _pendingUfhTemp = false;
};
