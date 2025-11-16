#include "CharlieplexedLeds.h"

namespace xDuinoRails {

CharlieplexedLeds::CharlieplexedLeds(const std::vector<uint8_t>& pins) : _pins(pins) {
    _ledStates.resize(_pins.size() * (_pins.size() - 1), false);
}

void CharlieplexedLeds::begin() {
    // All pins to INPUT to effectively disconnect them
    for (uint8_t pin : _pins) {
        pinMode(pin, INPUT);
    }
}

void CharlieplexedLeds::on() {
    for (size_t i = 0; i < _ledStates.size(); ++i) {
        _ledStates[i] = true;
    }
}

void CharlieplexedLeds::off() {
    for (size_t i = 0; i < _ledStates.size(); ++i) {
        _ledStates[i] = false;
    }
}

void CharlieplexedLeds::setLevel(uint8_t level) {
    // Charlieplexing doesn't really support brightness levels in a simple way.
    // We'll just turn them on or off.
    if (level > 0) {
        on();
    } else {
        off();
    }
}

void CharlieplexedLeds::setLed(uint8_t led, bool state) {
    if (led < _ledStates.size()) {
        _ledStates[led] = state;
    }
}

void CharlieplexedLeds::update(uint32_t delta_ms) {
    _lastUpdateTime += delta_ms;
    if (_lastUpdateTime < 1) {
        return;
    }
    _lastUpdateTime = 0;

    // Turn off the current LED
    uint8_t anode = _currentLed / (_pins.size() - 1);
    uint8_t cathode = _currentLed % (_pins.size() - 1);
    if (cathode >= anode) {
        cathode++;
    }
    pinMode(_pins[anode], INPUT);
    pinMode(_pins[cathode], INPUT);

    // Move to the next LED
    _currentLed = (_currentLed + 1) % _ledStates.size();

    // Turn on the next LED if it's supposed to be on
    if (_ledStates[_currentLed]) {
        anode = _currentLed / (_pins.size() - 1);
        cathode = _currentLed % (_pins.size() - 1);
        if (cathode >= anode) {
            cathode++;
        }
        pinMode(_pins[anode], OUTPUT);
        digitalWrite(_pins[anode], HIGH);
        pinMode(_pins[cathode], OUTPUT);
        digitalWrite(_pins[cathode], LOW);
    }
}

}
