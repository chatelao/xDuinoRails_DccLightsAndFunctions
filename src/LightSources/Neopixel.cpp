#include "Neopixel.h"

namespace xDuinoRails {

Neopixel::Neopixel(uint8_t pin, uint32_t color) : _strip(1, pin, NEO_GRB + NEO_KHZ800), _color(color), _pin(pin) {}

void Neopixel::begin() {
    _strip.begin();
    _strip.setBrightness(255); // Set global brightness to max, we handle scaling manually
    _strip.show();
}

void Neopixel::on() {
    setLevel(255);
}

void Neopixel::off() {
    setLevel(0);
}

void Neopixel::setLevel(uint8_t level) {
    if (level == 0) {
        _strip.setPixelColor(0, 0);
    } else if (level == 255) {
        _strip.setPixelColor(0, _color);
    } else {
        // Extract RGB components
        uint8_t r = (uint8_t)(_color >> 16);
        uint8_t g = (uint8_t)(_color >> 8);
        uint8_t b = (uint8_t)(_color);

        // Scale components using uint16_t to prevent overflow on 8-bit AVR
        r = (uint8_t)(((uint16_t)r * level) >> 8);
        g = (uint8_t)(((uint16_t)g * level) >> 8);
        b = (uint8_t)(((uint16_t)b * level) >> 8);

        _strip.setPixelColor(0, _strip.Color(r, g, b));
    }
    _strip.show();
}

void Neopixel::update(uint32_t delta_ms) {
    // No-op
}

}
