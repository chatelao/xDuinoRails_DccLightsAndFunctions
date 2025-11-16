#include "LogicalFunction.h"

namespace xDuinoRails {

LogicalFunction::LogicalFunction(Effect* effect) : _effect(effect) {}

LogicalFunction::~LogicalFunction() {
    delete _effect;
}

void LogicalFunction::addOutput(PhysicalOutput* output) {
    _outputs.push_back(output);
}

void LogicalFunction::setActive(bool active) {
    if (_effect) _effect->setActive(active);
}

bool LogicalFunction::isActive() const {
    return _effect ? _effect->isActive() : false;
}

void LogicalFunction::setDimmed(bool dimmed) {
    if (_effect) _effect->setDimmed(dimmed);
}

bool LogicalFunction::isDimmed() const {
    return _effect ? _effect->isDimmed() : false;
}

void LogicalFunction::update(uint32_t delta_ms) {
    if (_effect) {
        _effect->update(delta_ms, _outputs);
    }
}

}
