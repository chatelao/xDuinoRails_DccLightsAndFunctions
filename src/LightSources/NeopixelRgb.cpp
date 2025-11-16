#include "NeopixelRgb.h"

namespace xDuinoRails {

NeopixelRgb::NeopixelRgb(uint8_t pin, uint8_t r, uint8_t g, uint8_t b) :
    _strip(1, pin, NEO_GRB + NEO_KHZ800),
    _pin(pin)
{
    _color = _strip.Color(r, g, b);
}

void NeopixelRgb::begin() {
    _strip.begin();
}

void NeopixelRgb::on() {
    _strip.setPixelColor(0, _color);
    _strip.show();
}

void NeopixelRgb::off() {
    _strip.setPixelColor(0, 0);
    _strip.show();
}

void NeopixelRgb::setLevel(uint8_t level) {
    _strip.setBrightness(level);
    _strip.show();
}

void NeopixelRgb::update(uint32_t delta_ms) {
    // No-op
}

}
