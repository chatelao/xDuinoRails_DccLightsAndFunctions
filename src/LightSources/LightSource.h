#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include <cstdint>

namespace xDuinoRails {

class LightSource {
public:
    virtual ~LightSource() {}

    virtual void begin() = 0;
    virtual void on() = 0;
    virtual void off() = 0;
    virtual void setLevel(uint8_t level) = 0;
    virtual void update(uint32_t delta_ms) = 0;
};

}

#endif // LIGHTSOURCE_H
