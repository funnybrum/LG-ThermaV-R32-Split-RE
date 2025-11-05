#include "Main.h"

ActuatedValve::ActuatedValve(uint8_t on_pin, uint8_t off_pin, uint32_t change_mode_cycle_duration) {
    this->_on_pin = on_pin;
    this->_off_pin = off_pin;
    this->_change_mode_cycle_duration = change_mode_cycle_duration;
    pinMode(this->_on_pin, OUTPUT);
    pinMode(this->_off_pin, OUTPUT);
    digitalWrite(this->_on_pin, LOW);
    digitalWrite(this->_off_pin, LOW);
    this->setMode(AV_OFF);
}

void ActuatedValve::begin() {
}

void ActuatedValve::loop() {
    uint32_t millis_since_last_update = millis() - this->_last_mode_change_ts;

    if (millis_since_last_update <= this->_change_mode_cycle_duration) {
        // Turn on one of the two relays to get the actuated valve mooving.
        digitalWrite(_on_pin, (this->_mode==AV_ON)?HIGH:LOW);
        digitalWrite(_off_pin, (this->_mode==AV_OFF)?HIGH:LOW);
        return;
    }

    // Not in the process of changing valve mode, turn off both relays.
    digitalWrite(_on_pin, LOW);
    digitalWrite(_off_pin, LOW);
}

void ActuatedValve::setMode(ActuatedValveMode mode) {
    if (this->_mode == mode) {
        // Already in that mode. No need for change.
        return;
    }
    this->_mode = mode;
    this->_last_mode_change_ts = millis();
}

ActuatedValveMode ActuatedValve::getMode() {
    return this->_mode;
}