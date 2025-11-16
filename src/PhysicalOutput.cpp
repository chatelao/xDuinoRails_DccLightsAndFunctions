#include "PhysicalOutput.h"

namespace xDuinoRails {

PhysicalOutput::PhysicalOutput(std::unique_ptr<LightSource> lightSource) :
    _type(OutputType::LIGHT_SOURCE),
    _lightSource(std::move(lightSource)),
    _pin(0)
{}

PhysicalOutput::PhysicalOutput(uint8_t pin) :
    _type(OutputType::SERVO),
    _lightSource(nullptr),
    _pin(pin)
{}

void PhysicalOutput::begin() {
    if (_type == OutputType::LIGHT_SOURCE) {
        _lightSource->begin();
    } else {
        _servo.attach(_pin);
    }
}

void PhysicalOutput::setValue(uint8_t value) {
    if (_type == OutputType::LIGHT_SOURCE) {
        if (value > 0) {
            _lightSource->on();
            _lightSource->setLevel(value);
        } else {
            _lightSource->off();
        }
    }
}

void PhysicalOutput::setServoAngle(uint16_t angle) {
    if (_type == OutputType::SERVO) {
        _servo.write(angle);
    }
}

void PhysicalOutput::update(uint32_t delta_ms) {
    if (_type == OutputType::LIGHT_SOURCE) {
        _lightSource->update(delta_ms);
    }
}

}
