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
}

void NeopixelRgbMulti::on() {
    for (uint16_t i = 0; i < _numPixels; i++) {
        _strip.setPixelColor(i, _color);
    }
    _strip.show();
}

void NeopixelRgbMulti::off() {
    for (uint16_t i = 0; i < _numPixels; i++) {
        _strip.setPixelColor(i, 0);
    }
    _strip.show();
}

void NeopixelRgbMulti::setLevel(uint8_t level) {
    _strip.setBrightness(level);
    _strip.show();
}

void NeopixelRgbMulti::update(uint32_t delta_ms) {
    // No-op
}

}
