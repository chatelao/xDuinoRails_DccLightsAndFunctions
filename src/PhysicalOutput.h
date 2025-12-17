#ifndef PHYSICALOUTPUT_H
#define PHYSICALOUTPUT_H

#include <cstdint>
#include <Servo.h>
#include "LightSources/LightSource.h"

namespace xDuinoRails {

enum class OutputType {
    LIGHT_SOURCE,
    SERVO
};

class PhysicalOutput {
public:
    PhysicalOutput(LightSource* lightSource);
    PhysicalOutput(uint8_t pin); // For Servo
    ~PhysicalOutput();

    // Add move constructor and assignment operator
    PhysicalOutput(PhysicalOutput&& other) noexcept;
    PhysicalOutput& operator=(PhysicalOutput&& other) noexcept;

    // Delete copy constructor and assignment operator
    PhysicalOutput(const PhysicalOutput&) = delete;
    PhysicalOutput& operator=(const PhysicalOutput&) = delete;

    void begin();
    void setValue(uint8_t value);
    void setServoAngle(uint16_t angle);
    void update(uint32_t delta_ms);

private:
    OutputType _type;
    LightSource* _lightSource = nullptr;
    Servo _servo;
    uint8_t _pin; // For Servo
};

}

#endif // PHYSICALOUTPUT_H
