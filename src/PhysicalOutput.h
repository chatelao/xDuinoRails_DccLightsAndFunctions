#ifndef PHYSICALOUTPUT_H
#define PHYSICALOUTPUT_H

#include <cstdint>
#include <Servo.h>

namespace xDuinoRails {

enum class OutputType {
    PWM,
    SERVO
};

class PhysicalOutput {
public:
    PhysicalOutput(uint8_t pin, OutputType type);
    void attach();
    void setValue(uint8_t value);
    void setServoAngle(uint16_t angle);

private:
    uint8_t _pin;
    OutputType _type;
    Servo _servo;
};

}

#endif // PHYSICALOUTPUT_H
