#ifndef CHARLIEPLEXEDLEDS_H
#define CHARLIEPLEXEDLEDS_H

#include "LightSource.h"
#include <Arduino.h>
#include <vector>

namespace xDuinoRails {

class CharlieplexedLeds : public LightSource {
public:
    CharlieplexedLeds(const std::vector<uint8_t>& pins);

    void begin() override;
    void on() override;
    void off() override;
    void setLevel(uint8_t level) override;
    void update(uint32_t delta_ms) override;

    void setLed(uint8_t led, bool state);

private:

    std::vector<uint8_t> _pins;
    std::vector<bool> _ledStates;
    uint8_t _currentLed = 0;
    uint32_t _lastUpdateTime = 0;
};

}

#endif // CHARLIEPLEXEDLEDS_H
