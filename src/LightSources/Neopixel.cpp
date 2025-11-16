#include "Neopixel.h"

namespace xDuinoRails {

Neopixel::Neopixel(uint8_t pin, uint32_t color) : _strip(1, pin, NEO_GRB + NEO_KHZ800), _color(color), _pin(pin) {}

void Neopixel::begin() {
    _strip.begin();
}

void Neopixel::on() {
    _strip.setPixelColor(0, _color);
    _strip.show();
}

void Neopixel::off() {
    _strip.setPixelColor(0, 0);
    _strip.show();
}

void Neopixel::setLevel(uint8_t level) {
    _strip.setBrightness(level);
    _strip.show();
}

void Neopixel::update(uint32_t delta_ms) {
    // No-op
}

}
