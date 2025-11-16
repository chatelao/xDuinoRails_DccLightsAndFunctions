#ifndef DUALCOLORLED_H
#define DUALCOLORLED_H

#include "LightSource.h"
#include <Arduino.h>

namespace xDuinoRails {

enum class DualColorLedState {
    Off,
    Color1,
    Color2
};

class DualColorLed : public LightSource {
public:
    DualColorLed(uint8_t pin1, uint8_t pin2);

    void begin() override;
    void on() override;
    void off() override;
    void setLevel(uint8_t level) override;
    void update(uint32_t delta_ms) override;

    void setColor(DualColorLedState color);

private:
    uint8_t _pin1;
    uint8_t _pin2;
    DualColorLedState _state = DualColorLedState::Off;
};

}

#endif // DUALCOLORLED_H
