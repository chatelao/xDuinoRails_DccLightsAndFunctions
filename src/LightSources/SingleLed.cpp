#include "SingleLed.h"

namespace xDuinoRails {

SingleLed::SingleLed(uint8_t pin) : _pin(pin) {}

void SingleLed::begin() {
    pinMode(_pin, OUTPUT);
}

void SingleLed::on() {
    digitalWrite(_pin, HIGH);
}

void SingleLed::off() {
    digitalWrite(_pin, LOW);
}

void SingleLed::setLevel(uint8_t level) {
    analogWrite(_pin, level);
}

void SingleLed::update(uint32_t delta_ms) {
    // No-op
}

}
