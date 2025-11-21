#include "NeopixelRgbMulti.h"

namespace xDuinoRails {

NeopixelRgbMulti::NeopixelRgbMulti(uint8_t pin, uint16_t numPixels, uint8_t r, uint8_t g, uint8_t b) :
    _strip(numPixels, pin, NEO_GRB + NEO_KHZ800),
    _numPixels(numPixels)
{
    _color = _strip.Color(r, g, b);
}

void NeopixelRgbMulti::begin() {
    _strip.begin();
    _strip.setBrightness(255);
    _strip.show();
}

void NeopixelRgbMulti::on() {
    setLevel(255);
}

void NeopixelRgbMulti::off() {
    setLevel(0);
}

void NeopixelRgbMulti::setLevel(uint8_t level) {
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

    for (uint16_t i = 0; i < _numPixels; i++) {
        _strip.setPixelColor(i, targetColor);
    }
    _strip.show();
}

void NeopixelRgbMulti::update(uint32_t delta_ms) {
    // No-op
}

}
