#ifndef NEOPIXEL_H
#define NEOPIXEL_H

#include "LightSource.h"
#include <Adafruit_NeoPixel.h>

namespace xDuinoRails {

class Neopixel : public LightSource {
public:
    Neopixel(uint8_t pin, uint32_t color);

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

#endif // NEOPIXEL_H
