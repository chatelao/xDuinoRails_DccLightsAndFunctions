#ifndef NEOPIXELRGBMULTISWISSAE66_H
#define NEOPIXELRGBMULTISWISSAE66_H

#include "LightSource.h"
#include <Adafruit_NeoPixel.h>
#include <vector>

namespace xDuinoRails {

class NeopixelRgbMultiSwissAe66 : public LightSource {
public:
    NeopixelRgbMultiSwissAe66(uint8_t pin, uint16_t numPixels, uint8_t r, uint8_t g, uint8_t b);

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

#endif // NEOPIXELRGBMULTISWISSAE66_H
