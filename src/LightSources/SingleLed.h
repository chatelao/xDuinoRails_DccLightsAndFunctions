#ifndef SINGLELED_H
#define SINGLELED_H

#include "LightSource.h"
#include <Arduino.h>

namespace xDuinoRails {

class SingleLed : public LightSource {
public:
    SingleLed(uint8_t pin);

    void begin() override;
    void on() override;
    void off() override;
    void setLevel(uint8_t level) override;
    void update(uint32_t delta_ms) override;

private:
    uint8_t _pin;
};

}

#endif // SINGLELED_H
