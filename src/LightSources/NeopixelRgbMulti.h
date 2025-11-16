#ifndef NEOPIXELRGBMULTI_H
#define NEOPIXELRGBMULTI_H

#include "LightSource.h"
#include <Adafruit_NeoPixel.h>
#include <vector>

namespace xDuinoRails {

class NeopixelRgbMulti : public LightSource {
public:
    NeopixelRgbMulti(uint8_t pin, uint16_t numPixels, uint8_t r, uint8_t g, uint8_t b);

    void begin() override;
    void on() override;
    void off() override;
    void setLevel(uint8_t level) override;
    void update(uint32_t delta_ms) override;

private:
    Adafruit_NeoPixel _strip;
    uint32_t _color;
    uint16_t _numPixels;
};

}

#endif // NEOPIXELRGBMULTI_H
