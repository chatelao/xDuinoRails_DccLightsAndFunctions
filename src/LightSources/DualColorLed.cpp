#include "DualColorLed.h"

namespace xDuinoRails {

DualColorLed::DualColorLed(uint8_t pin1, uint8_t pin2) : _pin1(pin1), _pin2(pin2) {}

void DualColorLed::begin() {
    pinMode(_pin1, OUTPUT);
    pinMode(_pin2, OUTPUT);
    off();
}

void DualColorLed::on() {
    setColor(DualColorLedState::Color1);
}

void DualColorLed::off() {
    digitalWrite(_pin1, LOW);
    digitalWrite(_pin2, LOW);
    _state = DualColorLedState::Off;
}

void DualColorLed::setLevel(uint8_t level) {
    if (level > 0) {
        on();
    } else {
        off();
    }
}

void DualColorLed::setColor(DualColorLedState color) {
    _state = color;
    switch (_state) {
        case DualColorLedState::Off:
            digitalWrite(_pin1, LOW);
            digitalWrite(_pin2, LOW);
            break;
        case DualColorLedState::Color1:
            digitalWrite(_pin1, HIGH);
            digitalWrite(_pin2, LOW);
            break;
        case DualColorLedState::Color2:
            digitalWrite(_pin1, LOW);
            digitalWrite(_pin2, HIGH);
            break;
    }
}

void DualColorLed::update(uint32_t delta_ms) {
    // No-op
}

}
