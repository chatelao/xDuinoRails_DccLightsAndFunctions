/**
 * @file AuxController.cpp
 * @brief Implements the AuxController class and all related helper classes.
 */
#include "AuxController.h"
#include "cv_definitions.h"
#include <math.h>
#include "effects/Effect.h"
#include "LightSources/SingleLed.h"

namespace xDuinoRails {

// --- AuxController ---

AuxController::AuxController() {}

AuxController::~AuxController() {
    reset();
}

void AuxController::addPhysicalOutput(uint8_t pin, OutputType type) {
    if (type == OutputType::SERVO) {
        _outputs.emplace_back(pin);
    } else {
        _outputs.emplace_back(std::make_unique<SingleLed>(pin));
    }
    _outputs.back().begin();
}

void AuxController::addLightSource(std::unique_ptr<LightSource> lightSource) {
    _outputs.emplace_back(std::move(lightSource));
    _outputs.back().begin();
}

void AuxController::update(uint32_t delta_ms) {
    if (_state_changed) {
        evaluateMapping();
        _state_changed = false;
    }
    for (auto& func : _logical_functions) {
        func->update(delta_ms);
    }
    for (auto& output : _outputs) {
        output.update(delta_ms);
    }
}

void AuxController::loadFromCVs(ICVAccess& cvAccess) {
    reset();
    auto mapping_method = static_cast<FunctionMappingMethod>(cvAccess.readCV(CV_FUNCTION_MAPPING_METHOD));
    switch (mapping_method) {
        case FunctionMappingMethod::RCN_225:
            parseRcn225(cvAccess);
            break;
        case FunctionMappingMethod::RCN_227_PER_FUNCTION:
            parseRcn227PerFunction(cvAccess);
            break;
        case FunctionMappingMethod::RCN_227_PER_OUTPUT_V1:
            parseRcn227PerOutputV1(cvAccess);
            break;
        case FunctionMappingMethod::RCN_227_PER_OUTPUT_V2:
            parseRcn227PerOutputV2(cvAccess);
            break;
        case FunctionMappingMethod::PROPRIETARY:
        default:
            return;
        case FunctionMappingMethod::RCN_227_PER_OUTPUT_V3:
            parseRcn227PerOutputV3(cvAccess);
            break;
    }
}

void AuxController::setFunctionState(uint8_t functionNumber, bool functionState) {
    if (functionNumber < MAX_DCC_FUNCTIONS && _function_states[functionNumber] != functionState) {
        _function_states[functionNumber] = functionState;
        _state_changed = true;
    }
}

void AuxController::setDirection(DecoderDirection direction) {
    if (_direction != direction) {
        _direction = direction;
        _state_changed = true;
    }
}

void AuxController::setSpeed(uint16_t speed) {
    if (_speed != speed) {
        _speed = speed;
        _state_changed = true;
    }
}

void AuxController::setBinaryState(uint16_t state_number, bool value) {
    if (m_binary_states.find(state_number) == m_binary_states.end() || m_binary_states[state_number] != value) {
        m_binary_states[state_number] = value;
        _state_changed = true;
    }
}

bool AuxController::getFunctionState(uint8_t functionNumber) const {
    return (functionNumber < MAX_DCC_FUNCTIONS) ? _function_states[functionNumber] : false;
}

DecoderDirection AuxController::getDirection() const {
    return _direction;
}

uint16_t AuxController::getSpeed() const {
    return _speed;
}

bool AuxController::getConditionVariableState(uint8_t cv_id) const {
    auto it = _cv_states.find(cv_id);
    return (it != _cv_states.end()) ? it->second : false;
}

bool AuxController::getBinaryState(uint16_t state_number) const {
    auto it = m_binary_states.find(state_number);
    return (it != m_binary_states.end()) ? it->second : false;
}

LogicalFunction* AuxController::getLogicalFunction(size_t index) {
    return (index < _logical_functions.size()) ? _logical_functions[index] : nullptr;
}

void AuxController::addLogicalFunction(LogicalFunction* function) {
    _logical_functions.push_back(function);
}

void AuxController::addConditionVariable(const ConditionVariable& cv) {
    _condition_variables.push_back(cv);
}

void AuxController::addMappingRule(const MappingRule& rule) {
    _mapping_rules.push_back(rule);
}

void AuxController::reset() {
    for (auto lf : _logical_functions) delete lf;
    _logical_functions.clear();
    _condition_variables.clear();
    _mapping_rules.clear();
    _cv_states.clear();
    m_binary_states.clear();
    for (int i = 0; i < MAX_DCC_FUNCTIONS; ++i) _function_states[i] = false;
    _direction = DECODER_DIRECTION_FORWARD;
    _speed = 0;
    _state_changed = true;
}

void AuxController::evaluateMapping() {
    _cv_states.clear();
    for (const auto& cv : _condition_variables) {
        _cv_states[cv.id] = cv.evaluate(*this);
    }
    for (const auto& rule : _mapping_rules) {
        if (rule.evaluate(*this)) {
            if (rule.target_logical_function_id < _logical_functions.size()) {
                LogicalFunction* target_func = _logical_functions[rule.target_logical_function_id];
                switch (rule.action) {
                    case MappingAction::ACTIVATE: target_func->setActive(true); break;
                    case MappingAction::DEACTIVATE: target_func->setActive(false); break;
                    case MappingAction::SET_DIMMED: target_func->setDimmed(!target_func->isDimmed()); break;
                    default: break;
                }
            }
        }
    }
}

PhysicalOutput* AuxController::getOutputById(uint8_t id) {
    return (id < _outputs.size()) ? &_outputs[id] : nullptr;
}

void AuxController::parseRcn225(ICVAccess& cvAccess) {
    const int num_mapping_cvs = CV_OUTPUT_LOCATION_CONFIG_END - CV_OUTPUT_LOCATION_CONFIG_START + 1;
    for (int i = 0; i < num_mapping_cvs; ++i) {
        uint16_t cv_addr = CV_OUTPUT_LOCATION_CONFIG_START + i;
        uint8_t mapping_mask = cvAccess.readCV(cv_addr);
        if (mapping_mask == 0) continue;

        ConditionVariable cv;
        cv.id = i + 1;
        if (i == 0) {
            cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, DECODER_DIRECTION_FORWARD});
            cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, 0});
        } else if (i == 1) {
            cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, DECODER_DIRECTION_REVERSE});
            cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, 0});
        } else {
            cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, (uint8_t)(i - 1)});
        }
        addConditionVariable(cv);

        for (int output_bit = 0; output_bit < 8; ++output_bit) {
            if ((mapping_mask >> output_bit) & 1) {
                uint8_t physical_output_id = output_bit + 1;
                LogicalFunction* lf = new LogicalFunction(new EffectSteady(255));
                lf->addOutput(getOutputById(physical_output_id));
                addLogicalFunction(lf);
                uint8_t lf_idx = _logical_functions.size() - 1;
                MappingRule rule;
                rule.target_logical_function_id = lf_idx;
                rule.positive_conditions.push_back(cv.id);
                rule.action = MappingAction::ACTIVATE;
                addMappingRule(rule);
            }
        }
    }
}

void AuxController::parseRcn227PerOutputV3(ICVAccess& cvAccess) {
    cvAccess.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvAccess.writeCV(CV_INDEXED_CV_LOW_BYTE, 43);
    const int num_outputs = 32;
    for (int output_num = 0; output_num < num_outputs; ++output_num) {
        LogicalFunction* lf = nullptr;
        uint16_t base_cv = 257 + (output_num * 8);
        std::vector<uint8_t> activating_cv_ids, blocking_cv_ids;

        for (int i = 0; i < 4; ++i) {
            uint8_t cv_value = cvAccess.readCV(base_cv + i);
            if (cv_value == 255) continue;
            uint8_t func_num = cv_value & 0x3F;
            uint8_t dir_bits = (cv_value >> 6) & 0x03;
            bool is_blocking = (dir_bits == 0x03);
            ConditionVariable cv;
            cv.id = 700 + (output_num * 8) + i;
            cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, func_num});
            if (dir_bits == 0x01) cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, DECODER_DIRECTION_FORWARD});
            else if (dir_bits == 0x02) cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, DECODER_DIRECTION_REVERSE});
            addConditionVariable(cv);
            (is_blocking ? blocking_cv_ids : activating_cv_ids).push_back(cv.id);
        }

        for (int i = 0; i < 2; ++i) {
            uint8_t cv_high = cvAccess.readCV(base_cv + 4 + (i * 2));
            uint8_t cv_low = cvAccess.readCV(base_cv + 5 + (i * 2));
            if (cv_high == 255 && cv_low == 255) continue;
            bool is_blocking = (cv_high & 0x80) != 0;
            uint16_t value = ((cv_high & 0x7F) << 8) | cv_low;
            ConditionVariable cv;
            cv.id = 700 + (output_num * 8) + 4 + i;
            if (value <= 68) cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, (uint8_t)value});
            else cv.conditions.push_back({TriggerSource::BINARY_STATE, TriggerComparator::IS_TRUE, (uint8_t)(value - 69)});
            addConditionVariable(cv);
            (is_blocking ? blocking_cv_ids : activating_cv_ids).push_back(cv.id);
        }

        if (!activating_cv_ids.empty()) {
            lf = new LogicalFunction(new EffectSteady(255));
            lf->addOutput(getOutputById(output_num + 1));
            addLogicalFunction(lf);
            uint8_t lf_idx = _logical_functions.size() - 1;
            for (uint8_t activating_id : activating_cv_ids) {
                MappingRule rule;
                rule.target_logical_function_id = lf_idx;
                rule.positive_conditions.push_back(activating_id);
                rule.negative_conditions = blocking_cv_ids;
                rule.action = MappingAction::ACTIVATE;
                addMappingRule(rule);
            }
        }
    }
}

void AuxController::parseRcn227PerFunction(ICVAccess& cvAccess) {
    cvAccess.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvAccess.writeCV(CV_INDEXED_CV_LOW_BYTE, 40);

    const int num_functions = 32;

    for (int func_num = 0; func_num < num_functions; ++func_num) {
        for (int dir = 0; dir < 2; ++dir) {
            uint16_t base_cv = 257 + (func_num * 2 + dir) * 4;
            uint32_t output_mask = (uint32_t)cvAccess.readCV(base_cv + 2) << 16 | (uint32_t)cvAccess.readCV(base_cv + 1) << 8 | cvAccess.readCV(base_cv);
            uint8_t blocking_func_num = cvAccess.readCV(base_cv + 3);

            if (output_mask == 0) continue;

            ConditionVariable cv;
            cv.id = (func_num * 2) + dir + 1;
            cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, (uint8_t)func_num});
            cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, (uint8_t)((dir == 0) ? DECODER_DIRECTION_FORWARD : DECODER_DIRECTION_REVERSE)});
            addConditionVariable(cv);

            uint8_t blocking_cv_id = 0;
            if (blocking_func_num != 255) {
                ConditionVariable blocking_cv;
                blocking_cv.id = 100 + blocking_func_num;
                blocking_cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, blocking_func_num});
                addConditionVariable(blocking_cv);
                blocking_cv_id = blocking_cv.id;
            }

            for (int output_bit = 0; output_bit < 24; ++output_bit) {
                if ((output_mask >> output_bit) & 1) {
                    uint8_t physical_output_id = output_bit + 1;
                    LogicalFunction* lf = new LogicalFunction(new EffectSteady(255));
                    lf->addOutput(getOutputById(physical_output_id));
                    addLogicalFunction(lf);
                    uint8_t lf_idx = _logical_functions.size() - 1;

                    MappingRule rule;
                    rule.target_logical_function_id = lf_idx;
                    rule.positive_conditions.push_back(cv.id);
                    if (blocking_cv_id != 0) rule.negative_conditions.push_back(blocking_cv_id);
                    rule.action = MappingAction::ACTIVATE;
                    addMappingRule(rule);
                }
            }
        }
    }
}

void AuxController::parseRcn227PerOutputV1(ICVAccess& cvAccess) {
    cvAccess.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvAccess.writeCV(CV_INDEXED_CV_LOW_BYTE, 41);

    const int num_outputs = 24;

    for (int output_num = 0; output_num < num_outputs; ++output_num) {
        LogicalFunction* lf = nullptr; // Lazily created

        for (int dir = 0; dir < 2; ++dir) {
            uint16_t base_cv = 257 + (output_num * 2 + dir) * 4;
            uint32_t func_mask = (uint32_t)cvAccess.readCV(base_cv + 3) << 24 | (uint32_t)cvAccess.readCV(base_cv + 2) << 16 | (uint32_t)cvAccess.readCV(base_cv + 1) << 8 | cvAccess.readCV(base_cv);

            if (func_mask == 0) continue;

            if (lf == nullptr) {
                lf = new LogicalFunction(new EffectSteady(255));
                lf->addOutput(getOutputById(output_num + 1));
                addLogicalFunction(lf);
            }
            uint8_t lf_idx = _logical_functions.size() - 1;

            for (int func_num = 0; func_num < 32; ++func_num) {
                if ((func_mask >> func_num) & 1) {
                    ConditionVariable cv;
                    cv.id = 200 + (output_num * 64) + (dir * 32) + func_num; // Unique ID
                    cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, (uint8_t)func_num});
                    cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, (uint8_t)((dir == 0) ? DECODER_DIRECTION_FORWARD : DECODER_DIRECTION_REVERSE)});
                    addConditionVariable(cv);

                    MappingRule rule;
                    rule.target_logical_function_id = lf_idx;
                    rule.positive_conditions.push_back(cv.id);
                    rule.action = MappingAction::ACTIVATE;
                    addMappingRule(rule);
                }
            }
        }
    }
}

void AuxController::parseRcn227PerOutputV2(ICVAccess& cvAccess) {
    cvAccess.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvAccess.writeCV(CV_INDEXED_CV_LOW_BYTE, 42);

    const int num_outputs = 32;

    for (int output_num = 0; output_num < num_outputs; ++output_num) {
        LogicalFunction* lf = nullptr;

        for (int dir = 0; dir < 2; ++dir) {
            uint16_t base_cv = 257 + (output_num * 2 + dir) * 4;
            uint8_t funcs[] = {
                cvAccess.readCV(base_cv),
                cvAccess.readCV(base_cv + 1),
                cvAccess.readCV(base_cv + 2)
            };
            uint8_t blocking_func = cvAccess.readCV(base_cv + 3);

            uint8_t blocking_cv_id = 0;
            if (blocking_func != 255) {
                ConditionVariable blocking_cv;
                blocking_cv.id = 400 + blocking_func; // Unique ID
                if (blocking_func > 28) {
                    blocking_cv.conditions.push_back({TriggerSource::BINARY_STATE, TriggerComparator::IS_TRUE, (uint16_t)(blocking_func)});
                } else {
                    blocking_cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, blocking_func});
                }
                addConditionVariable(blocking_cv);
                blocking_cv_id = blocking_cv.id;
            }

            for (int i = 0; i < 3; ++i) {
                if (funcs[i] != 255) {
                    if (lf == nullptr) {
                        lf = new LogicalFunction(new EffectSteady(255));
                        lf->addOutput(getOutputById(output_num + 1));
                        addLogicalFunction(lf);
                    }
                    uint8_t lf_idx = _logical_functions.size() - 1;

                    ConditionVariable cv;
                    cv.id = 500 + (output_num * 8) + (dir * 4) + i; // Unique ID
                    if (funcs[i] > 28) {
                        cv.conditions.push_back({TriggerSource::BINARY_STATE, TriggerComparator::IS_TRUE, (uint16_t)(funcs[i])});
                    } else {
                        cv.conditions.push_back({TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, funcs[i]});
                    }
                    cv.conditions.push_back({TriggerSource::DIRECTION, TriggerComparator::EQ, (uint8_t)((dir == 0) ? DECODER_DIRECTION_FORWARD : DECODER_DIRECTION_REVERSE)});
                    addConditionVariable(cv);

                    MappingRule rule;
                    rule.target_logical_function_id = lf_idx;
                    rule.positive_conditions.push_back(cv.id);
                    if (blocking_cv_id != 0) rule.negative_conditions.push_back(blocking_cv_id);
                    rule.action = MappingAction::ACTIVATE;
                    addMappingRule(rule);
                }
            }
        }
    }
}
}
