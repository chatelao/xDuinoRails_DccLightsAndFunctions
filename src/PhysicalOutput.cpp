#include "PhysicalOutput.h"
#include <utility> // For std::exchange

namespace xDuinoRails {

PhysicalOutput::PhysicalOutput(LightSource* lightSource) :
    _type(OutputType::LIGHT_SOURCE),
    _lightSource(lightSource),
    _pin(0)
{}

PhysicalOutput::PhysicalOutput(uint8_t pin) :
    _type(OutputType::SERVO),
    _lightSource(nullptr),
    _pin(pin)
{}

PhysicalOutput::~PhysicalOutput() {
    delete _lightSource;
}

PhysicalOutput::PhysicalOutput(PhysicalOutput&& other) noexcept :
    _type(other._type),
    _lightSource(std::exchange(other._lightSource, nullptr)),
    _servo(other._servo),
    _pin(other._pin)
{}

PhysicalOutput& PhysicalOutput::operator=(PhysicalOutput&& other) noexcept {
    if (this != &other) {
        delete _lightSource;
        _type = other._type;
        _lightSource = std::exchange(other._lightSource, nullptr);
        _servo = other._servo;
        _pin = other._pin;
    }
    return *this;
}

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
