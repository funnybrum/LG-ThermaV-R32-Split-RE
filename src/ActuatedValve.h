#pragma once

enum ActuatedValveMode {
    AV_OFF,
    AV_ON,
    AV_UNKNOWN
};

class ActuatedValve {
    public:
        ActuatedValve(uint8_t on_pin, uint8_t off_pin, uint32_t change_mode_cycle_duration=10000);
        void begin();
        void loop();
        void setMode(ActuatedValveMode mode);
        ActuatedValveMode getMode();
    private:
        uint8_t _on_pin;
        uint8_t _off_pin;
        uint32_t _last_mode_change_ts = 0;
        uint32_t _change_mode_cycle_duration;
        ActuatedValveMode _mode = AV_UNKNOWN;
};
