#include "NeopixelRgbMultiSwissAe66.h"

namespace xDuinoRails {

NeopixelRgbMultiSwissAe66::NeopixelRgbMultiSwissAe66(uint8_t pin, uint16_t numPixels, uint8_t r, uint8_t g, uint8_t b) :
    _strip(numPixels, pin, NEO_GRB + NEO_KHZ800),
    _numPixels(numPixels)
{
    _color = _strip.Color(r, g, b);
}

void NeopixelRgbMultiSwissAe66::begin() {
    _strip.begin();
}

void NeopixelRgbMultiSwissAe66::on() {
    // Swiss Ae 6/6 tail lights: only the two outer lights are red.
    // Assuming a 3-pixel setup (or more), where the outer-most are used.
    if (_numPixels >= 2) {
        _strip.setPixelColor(0, _color); // First pixel
        _strip.setPixelColor(_numPixels - 1, _color); // Last pixel
    }
    // Ensure all other pixels are off
    for (uint16_t i = 1; i < _numPixels - 1; i++) {
        _strip.setPixelColor(i, 0);
    }
    _strip.show();
}

void NeopixelRgbMultiSwissAe66::off() {
    for (uint16_t i = 0; i < _numPixels; i++) {
        _strip.setPixelColor(i, 0);
    }
    _strip.show();
}

void NeopixelRgbMultiSwissAe66::setLevel(uint8_t level) {
    _strip.setBrightness(level);
    _strip.show();
}

void NeopixelRgbMultiSwissAe66::update(uint32_t delta_ms) {
    // No-op
}

}
