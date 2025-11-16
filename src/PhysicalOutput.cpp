#include "PhysicalOutput.h"
#include <Arduino.h>

namespace xDuinoRails {

PhysicalOutput::PhysicalOutput(uint8_t pin, OutputType type) : _pin(pin), _type(type) {}

void PhysicalOutput::attach() {
    if (_type == OutputType::PWM) {
        pinMode(_pin, OUTPUT);
        analogWrite(_pin, 0);
    } else if (_type == OutputType::SERVO) {
        _servo.attach(_pin);
    }
}

void PhysicalOutput::setValue(uint8_t value) {
    if (_type == OutputType::PWM) {
        analogWrite(_pin, value);
    }
}

void PhysicalOutput::setServoAngle(uint16_t angle) {
    if (_type == OutputType::SERVO) {
        _servo.write(angle);
    }
}

}
