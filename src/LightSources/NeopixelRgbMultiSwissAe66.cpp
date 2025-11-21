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
    _strip.setBrightness(255);
    _strip.show();
}

void NeopixelRgbMultiSwissAe66::on() {
    setLevel(255);
}

void NeopixelRgbMultiSwissAe66::off() {
    setLevel(0);
}

void NeopixelRgbMultiSwissAe66::setLevel(uint8_t level) {
    uint32_t targetColor;

    if (level == 0) {
        targetColor = 0;
    } else if (level == 255) {
        targetColor = _color;
    } else {
        uint8_t r = (uint8_t)(_color >> 16);
        uint8_t g = (uint8_t)(_color >> 8);
        uint8_t b = (uint8_t)(_color);

        r = (uint8_t)(((uint16_t)r * level) >> 8);
        g = (uint8_t)(((uint16_t)g * level) >> 8);
        b = (uint8_t)(((uint16_t)b * level) >> 8);

        targetColor = _strip.Color(r, g, b);
    }

    // Apply to specific pixels (Swiss Ae 6/6 tail light pattern)
    if (_numPixels >= 2) {
        _strip.setPixelColor(0, targetColor); // First pixel
        _strip.setPixelColor(_numPixels - 1, targetColor); // Last pixel
    }
    // Ensure all other pixels are off
    for (uint16_t i = 1; i < _numPixels - 1; i++) {
        _strip.setPixelColor(i, 0);
    }

    _strip.show();
}

void NeopixelRgbMultiSwissAe66::update(uint32_t delta_ms) {
    // No-op
}

}
