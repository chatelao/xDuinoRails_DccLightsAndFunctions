#ifndef NEOPIXELRGB_H
#define NEOPIXELRGB_H

#include "LightSource.h"
#include <Adafruit_NeoPixel.h>

namespace xDuinoRails {

class NeopixelRgb : public LightSource {
public:
    NeopixelRgb(uint8_t pin, uint8_t r, uint8_t g, uint8_t b);

    void begin() override;
    void on() override;
    void off() override;
    void setLevel(uint8_t level) override;
    void update(uint32_t delta_ms) override;

private:
    Adafruit_NeoPixel _strip;
    uint32_t _color;
    uint8_t _pin;
};

}

#endif // NEOPIXELRGB_H
