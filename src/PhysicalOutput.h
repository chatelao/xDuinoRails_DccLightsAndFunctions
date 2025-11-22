#ifndef PHYSICALOUTPUT_H
#define PHYSICALOUTPUT_H

#include <cstdint>
#include <memory>
#include "compat/ArduinoSTL_AVR_Compat.h"
#include <Servo.h>
#include "LightSources/LightSource.h"

namespace xDuinoRails {

enum class OutputType {
    LIGHT_SOURCE,
    SERVO
};

class PhysicalOutput {
public:
    PhysicalOutput(std::unique_ptr<LightSource> lightSource);
    PhysicalOutput(uint8_t pin); // For Servo
    void begin();
    void setValue(uint8_t value);
    void setServoAngle(uint16_t angle);
    void update(uint32_t delta_ms);

private:
    OutputType _type;
    std::unique_ptr<LightSource> _lightSource;
    Servo _servo;
    uint8_t _pin; // For Servo
};

}

#endif // PHYSICALOUTPUT_H
