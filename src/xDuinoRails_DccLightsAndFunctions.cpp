/**
 * @file AuxController.cpp
 * @brief Implements the AuxController class and all related helper classes.
 */
#include "xDuinoRails_DccLightsAndFunctions.h"
#include "cv_definitions.h"
#include "effects/Effect.h"
#include "LightSources/SingleLed.h"
#include <utility> // For std::move and std::forward

namespace xDuinoRails {

// Helper function to manage dynamic arrays, mimicking vector::push_back
template <typename T>
void addToArray(T*& arr, size_t& count, size_t& capacity, const T& element) {
    if (count == capacity) {
        size_t new_capacity = (capacity == 0) ? 4 : capacity * 2;
        T* new_arr = new T[new_capacity];
        for (size_t i = 0; i < count; ++i) {
            new_arr[i] = arr[i];
        }
        delete[] arr;
        arr = new_arr;
        capacity = new_capacity;
    }
    arr[count++] = element;
}

template <typename T>
void addToArray(T*& arr, size_t& count, size_t& capacity, T&& element) {
    if (count == capacity) {
        size_t new_capacity = (capacity == 0) ? 4 : capacity * 2;
        T* new_arr = new T[new_capacity];
        for (size_t i = 0; i < count; ++i) {
            new_arr[i] = std::move(arr[i]);
        }
        delete[] arr;
        arr = new_arr;
        capacity = new_capacity;
    }
    arr[count++] = std::forward<T>(element);
}

// Helper for our KeyValue store
void setValue(AuxController::KeyValue*& arr, size_t& count, size_t& capacity, uint16_t key, bool value) {
    for (size_t i = 0; i < count; ++i) {
        if (arr[i].key == key) {
            arr[i].value = value;
            return;
        }
    }
    addToArray(arr, count, capacity, {key, value});
}

bool getValue(const AuxController::KeyValue* arr, size_t count, uint16_t key) {
    for (size_t i = 0; i < count; ++i) {
        if (arr[i].key == key) {
            return arr[i].value;
        }
    }
    return false;
}

// --- AuxController ---

AuxController::AuxController() {}

AuxController::~AuxController() {
    reset();
}

void AuxController::addPhysicalOutput(uint8_t pin, OutputType type) {
    if (type == OutputType::SERVO) {
        addToArray(_outputs, _outputs_count, _outputs_capacity, PhysicalOutput(pin));
    } else {
        addToArray(_outputs, _outputs_count, _outputs_capacity, PhysicalOutput(new SingleLed(pin)));
    }
    _outputs[_outputs_count - 1].begin();
}

void AuxController::addLightSource(LightSource* lightSource) {
    addToArray(_outputs, _outputs_count, _outputs_capacity, PhysicalOutput(lightSource));
    _outputs[_outputs_count - 1].begin();
}

void AuxController::update(uint32_t delta_ms) {
    if (_state_changed) {
        _state_changed = false;
        evaluateMapping();
    }
    for (size_t i = 0; i < _logical_functions_count; ++i) {
        _logical_functions[i]->update(delta_ms);
    }
    for (size_t i = 0; i < _outputs_count; ++i) {
        _outputs[i].update(delta_ms);
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
    if (getValue(m_binary_states, m_binary_states_count, state_number) != value) {
        setValue(m_binary_states, m_binary_states_count, m_binary_states_capacity, state_number, value);
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

bool AuxController::getConditionVariableState(uint16_t cv_id) const {
    return getValue(_cv_states, _cv_states_count, cv_id);
}

bool AuxController::getBinaryState(uint16_t state_number) const {
    return getValue(m_binary_states, m_binary_states_count, state_number);
}

LogicalFunction* AuxController::getLogicalFunction(size_t index) {
    return (index < _logical_functions_count) ? _logical_functions[index] : nullptr;
}

const LogicalFunction* AuxController::getLogicalFunction(size_t index) const {
    return (index < _logical_functions_count) ? _logical_functions[index] : nullptr;
}

void AuxController::addLogicalFunction(LogicalFunction* function) {
    addToArray(_logical_functions, _logical_functions_count, _logical_functions_capacity, function);
}

void AuxController::addConditionVariable(const ConditionVariable& cv) {
    addToArray(_condition_variables, _condition_variables_count, _condition_variables_capacity, cv);
}

void AuxController::addMappingRule(const MappingRule& rule) {
    addToArray(_mapping_rules, _mapping_rules_count, _mapping_rules_capacity, rule);
}

void AuxController::reset() {
    for (size_t i = 0; i < _logical_functions_count; ++i) {
        delete _logical_functions[i];
    }
    delete[] _logical_functions;
    _logical_functions = nullptr;
    _logical_functions_count = 0;
    _logical_functions_capacity = 0;

    delete[] _condition_variables;
    _condition_variables = nullptr;
    _condition_variables_count = 0;
    _condition_variables_capacity = 0;

    delete[] _mapping_rules;
    _mapping_rules = nullptr;
    _mapping_rules_count = 0;
    _mapping_rules_capacity = 0;

    delete[] _outputs;
    _outputs = nullptr;
    _outputs_count = 0;
    _outputs_capacity = 0;

    delete[] _cv_states;
    _cv_states = nullptr;
    _cv_states_count = 0;
    _cv_states_capacity = 0;

    delete[] m_binary_states;
    m_binary_states = nullptr;
    m_binary_states_count = 0;
    m_binary_states_capacity = 0;

    for (int i = 0; i < MAX_DCC_FUNCTIONS; ++i) _function_states[i] = false;
    _direction = DECODER_DIRECTION_FORWARD;
    _speed = 0;
    _state_changed = true;
}

void AuxController::evaluateMapping() {
    _cv_states_count = 0;
    for (size_t i = 0; i < _condition_variables_count; ++i) {
        const auto& cv = _condition_variables[i];
        setValue(_cv_states, _cv_states_count, _cv_states_capacity, cv.id, cv.evaluate(*this));
    }
    for (size_t i = 0; i < _mapping_rules_count; ++i) {
        const auto& rule = _mapping_rules[i];
        if (rule.evaluate(*this)) {
            if (rule.target_logical_function_id < _logical_functions_count) {
                LogicalFunction* target_func = _logical_functions[rule.target_logical_function_id];
                bool was_active = target_func->isActive();
                switch (rule.action) {
                    case MappingAction::ACTIVATE: target_func->setActive(true); break;
                    case MappingAction::DEACTIVATE: target_func->setActive(false); break;
                    case MappingAction::SET_DIMMED: target_func->setDimmed(!target_func->isDimmed()); break;
                    default: break;
                }
                if (target_func->isActive() != was_active) {
                    _state_changed = true;
                }
            }
        }
    }
}

PhysicalOutput* AuxController::getOutputById(uint8_t id) {
    if (id > 0 && id <= _outputs_count) {
        return &_outputs[id - 1];
    }
    return nullptr;
}

void AuxController::parseRcn225(ICVAccess& cvAccess) {
    const int num_mapping_cvs = CV_OUTPUT_LOCATION_CONFIG_END - CV_OUTPUT_LOCATION_CONFIG_START + 1;
    for (int i = 0; i < num_mapping_cvs; ++i) {
        uint16_t cv_addr = CV_OUTPUT_LOCATION_CONFIG_START + i;
        uint8_t mapping_mask = cvAccess.readCV(cv_addr);
        if (mapping_mask == 0) continue;

        ConditionVariable cv;
        cv.id = i + 1;
        addToArray(cv.conditions, cv.conditions_count, cv.conditions_capacity, Condition{TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, (uint8_t)(i > 1 ? i - 1 : 0)});
        if (i == 0) {
            addToArray(cv.conditions, cv.conditions_count, cv.conditions_capacity, Condition{TriggerSource::DIRECTION, TriggerComparator::EQ, DECODER_DIRECTION_FORWARD});
        } else if (i == 1) {
            addToArray(cv.conditions, cv.conditions_count, cv.conditions_capacity, Condition{TriggerSource::DIRECTION, TriggerComparator::EQ, DECODER_DIRECTION_REVERSE});
        }
        addConditionVariable(cv);

        for (int output_bit = 0; output_bit < 8; ++output_bit) {
            if ((mapping_mask >> output_bit) & 1) {
                uint8_t physical_output_id = output_bit + 1;
                LogicalFunction* lf = new LogicalFunction(createEffectFromCVs(cvAccess, physical_output_id));
                lf->addOutput(getOutputById(physical_output_id));
                addLogicalFunction(lf);
                uint8_t lf_idx = _logical_functions_count - 1;
                MappingRule rule;
                rule.target_logical_function_id = lf_idx;
                addToArray(rule.positive_conditions, rule.positive_conditions_count, rule.positive_conditions_capacity, cv.id);
                rule.action = MappingAction::ACTIVATE;
                addMappingRule(rule);
            }
        }
    }
}

void AuxController::parseRcn227PerOutputV3(ICVAccess& cvAccess) {
    cvAccess.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvAccess.writeCV(CV_INDEXED_CV_LOW_BYTE, RCN227_PER_OUTPUT_V3_PAGE);
    const int num_outputs = 32;
    for (int output_num = 0; output_num < num_outputs; ++output_num) {
        LogicalFunction* lf = nullptr;
        uint16_t base_cv = 257 + (output_num * 8);

        uint16_t* activating_cv_ids = nullptr;
        size_t activating_cv_ids_count = 0;
        size_t activating_cv_ids_capacity = 0;

        uint16_t* blocking_cv_ids = nullptr;
        size_t blocking_cv_ids_count = 0;
        size_t blocking_cv_ids_capacity = 0;

        for (int i = 0; i < 4; ++i) {
            uint8_t cv_value = cvAccess.readCV(base_cv + i);
            if (cv_value == 255) continue;
            uint8_t func_num = cv_value & 0x3F;
            uint8_t dir_bits = (cv_value >> 6) & 0x03;
            bool is_blocking = (dir_bits == 0x03);
            ConditionVariable cv;
            cv.id = CV_ID_BASE_RCN227_PER_OUTPUT_V3 + (output_num * 8) + i;
            addToArray(cv.conditions, cv.conditions_count, cv.conditions_capacity, Condition{TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, func_num});
            if (dir_bits == 0x01) addToArray(cv.conditions, cv.conditions_count, cv.conditions_capacity, Condition{TriggerSource::DIRECTION, TriggerComparator::EQ, DECODER_DIRECTION_FORWARD});
            else if (dir_bits == 0x02) addToArray(cv.conditions, cv.conditions_count, cv.conditions_capacity, Condition{TriggerSource::DIRECTION, TriggerComparator::EQ, DECODER_DIRECTION_REVERSE});
            addConditionVariable(cv);
            if (is_blocking) {
                addToArray(blocking_cv_ids, blocking_cv_ids_count, blocking_cv_ids_capacity, cv.id);
            } else {
                addToArray(activating_cv_ids, activating_cv_ids_count, activating_cv_ids_capacity, cv.id);
            }
        }

        for (int i = 0; i < 2; ++i) {
            uint8_t cv_high = cvAccess.readCV(base_cv + 4 + (i * 2));
            uint8_t cv_low = cvAccess.readCV(base_cv + 5 + (i * 2));
            if (cv_high == 255 && cv_low == 255) continue;
            bool is_blocking = (cv_high & 0x80) != 0;
            uint16_t value = ((cv_high & 0x7F) << 8) | cv_low;
            ConditionVariable cv;
            cv.id = CV_ID_BASE_RCN227_PER_OUTPUT_V3 + (output_num * 8) + 4 + i;
            if (value <= 68) addToArray(cv.conditions, cv.conditions_count, cv.conditions_capacity, Condition{TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, (uint8_t)value});
            else addToArray(cv.conditions, cv.conditions_count, cv.conditions_capacity, Condition{TriggerSource::BINARY_STATE, TriggerComparator::IS_TRUE, (uint16_t)(value - 69)});
            addConditionVariable(cv);
             if (is_blocking) {
                addToArray(blocking_cv_ids, blocking_cv_ids_count, blocking_cv_ids_capacity, cv.id);
            } else {
                addToArray(activating_cv_ids, activating_cv_ids_count, activating_cv_ids_capacity, cv.id);
            }
        }

        if (activating_cv_ids_count > 0) {
            lf = new LogicalFunction(createEffectFromCVs(cvAccess, output_num + 1));
            lf->addOutput(getOutputById(output_num + 1));
            addLogicalFunction(lf);
            uint8_t lf_idx = _logical_functions_count - 1;
            for (size_t i = 0; i < activating_cv_ids_count; ++i) {
                MappingRule rule;
                rule.target_logical_function_id = lf_idx;
                addToArray(rule.positive_conditions, rule.positive_conditions_count, rule.positive_conditions_capacity, activating_cv_ids[i]);

                for(size_t j = 0; j < blocking_cv_ids_count; ++j) {
                    addToArray(rule.negative_conditions, rule.negative_conditions_count, rule.negative_conditions_capacity, blocking_cv_ids[j]);
                }

                rule.action = MappingAction::ACTIVATE;
                addMappingRule(rule);
            }
        }
        delete[] activating_cv_ids;
        delete[] blocking_cv_ids;
    }
}

void AuxController::parseRcn227PerFunction(ICVAccess& cvAccess) {
    cvAccess.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvAccess.writeCV(CV_INDEXED_CV_LOW_BYTE, RCN227_PER_FUNCTION_PAGE);

    const int num_functions = 32;

    for (int func_num = 0; func_num < num_functions; ++func_num) {
        for (int dir = 0; dir < 2; ++dir) {
            uint16_t base_cv = 257 + (func_num * 2 + dir) * 4;
            uint32_t output_mask = (uint32_t)cvAccess.readCV(base_cv + 2) << 16 | (uint32_t)cvAccess.readCV(base_cv + 1) << 8 | cvAccess.readCV(base_cv);
            uint8_t blocking_func_num = cvAccess.readCV(base_cv + 3);

            if (output_mask == 0) continue;

            ConditionVariable cv;
            cv.id = (func_num * 2) + dir + 1;
            addToArray(cv.conditions, cv.conditions_count, cv.conditions_capacity, Condition{TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, (uint8_t)func_num});
            addToArray(cv.conditions, cv.conditions_count, cv.conditions_capacity, Condition{TriggerSource::DIRECTION, TriggerComparator::EQ, (uint8_t)((dir == 0) ? DECODER_DIRECTION_FORWARD : DECODER_DIRECTION_REVERSE)});
            addConditionVariable(cv);

            uint16_t blocking_cv_id = 0;
            if (blocking_func_num != 255) {
                ConditionVariable blocking_cv;
                blocking_cv.id = CV_ID_BASE_RCN227_PER_FUNCTION_BLOCKING + blocking_func_num;
                addToArray(blocking_cv.conditions, blocking_cv.conditions_count, blocking_cv.conditions_capacity, Condition{TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, blocking_func_num});
                addConditionVariable(blocking_cv);
                blocking_cv_id = blocking_cv.id;
            }

            for (int output_bit = 0; output_bit < 24; ++output_bit) {
                if ((output_mask >> output_bit) & 1) {
                    uint8_t physical_output_id = output_bit + 1;
                    LogicalFunction* lf = new LogicalFunction(createEffectFromCVs(cvAccess, physical_output_id));
                    lf->addOutput(getOutputById(physical_output_id));
                    addLogicalFunction(lf);
                    uint8_t lf_idx = _logical_functions_count - 1;

                    MappingRule rule;
                    rule.target_logical_function_id = lf_idx;
                    addToArray(rule.positive_conditions, rule.positive_conditions_count, rule.positive_conditions_capacity, cv.id);
                    if (blocking_cv_id != 0) addToArray(rule.negative_conditions, rule.negative_conditions_count, rule.negative_conditions_capacity, blocking_cv_id);
                    rule.action = MappingAction::ACTIVATE;
                    addMappingRule(rule);
                }
            }
        }
    }
}

void AuxController::parseRcn227PerOutputV1(ICVAccess& cvAccess) {
    cvAccess.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvAccess.writeCV(CV_INDEXED_CV_LOW_BYTE, RCN227_PER_OUTPUT_V1_PAGE);

    const int num_outputs = 24;

    for (int output_num = 0; output_num < num_outputs; ++output_num) {
        LogicalFunction* lf = nullptr; // Lazily created

        for (int dir = 0; dir < 2; ++dir) {
            uint16_t base_cv = 257 + (output_num * 2 + dir) * 4;
            uint32_t func_mask = (uint32_t)cvAccess.readCV(base_cv + 3) << 24 | (uint32_t)cvAccess.readCV(base_cv + 2) << 16 | (uint32_t)cvAccess.readCV(base_cv + 1) << 8 | cvAccess.readCV(base_cv);

            if (func_mask == 0) continue;

            if (lf == nullptr) {
                lf = new LogicalFunction(createEffectFromCVs(cvAccess, output_num + 1));
                lf->addOutput(getOutputById(output_num + 1));
                addLogicalFunction(lf);
            }
            uint8_t lf_idx = _logical_functions_count - 1;

            for (int func_num = 0; func_num < 32; ++func_num) {
                if ((func_mask >> func_num) & 1) {
                    ConditionVariable cv;
                    cv.id = CV_ID_BASE_RCN227_PER_OUTPUT_V1 + (output_num * 64) + (dir * 32) + func_num; // Unique ID
                    addToArray(cv.conditions, cv.conditions_count, cv.conditions_capacity, Condition{TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, (uint8_t)func_num});
                    addToArray(cv.conditions, cv.conditions_count, cv.conditions_capacity, Condition{TriggerSource::DIRECTION, TriggerComparator::EQ, (uint8_t)((dir == 0) ? DECODER_DIRECTION_FORWARD : DECODER_DIRECTION_REVERSE)});
                    addConditionVariable(cv);

                    MappingRule rule;
                    rule.target_logical_function_id = lf_idx;
                    addToArray(rule.positive_conditions, rule.positive_conditions_count, rule.positive_conditions_capacity, cv.id);
                    rule.action = MappingAction::ACTIVATE;
                    addMappingRule(rule);
                }
            }
        }
    }
}

Effect* AuxController::createEffectFromCVs(ICVAccess& cvAccess, uint8_t output_num) {
    cvAccess.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvAccess.writeCV(CV_INDEXED_CV_LOW_BYTE, EFFECTS_BLOCK_PAGE);

    uint16_t base_cv = 257 + ((output_num - 1) * EFFECTS_BLOCK_CV_PER_OUTPUT);
    uint8_t effect_type = cvAccess.readCV(base_cv + EFFECTS_CV_OFFSET_TYPE);

    uint16_t p1 = (uint16_t)cvAccess.readCV(base_cv + EFFECTS_CV_OFFSET_PARAM1_MSB) << 8 | cvAccess.readCV(base_cv + EFFECTS_CV_OFFSET_PARAM1_LSB);
    uint16_t p2 = (uint16_t)cvAccess.readCV(base_cv + EFFECTS_CV_OFFSET_PARAM2_MSB) << 8 | cvAccess.readCV(base_cv + EFFECTS_CV_OFFSET_PARAM2_LSB);
    uint16_t p3 = (uint16_t)cvAccess.readCV(base_cv + EFFECTS_CV_OFFSET_PARAM3_MSB) << 8 | cvAccess.readCV(base_cv + EFFECTS_CV_OFFSET_PARAM3_LSB);

    switch (effect_type) {
        case EFFECT_TYPE_DIMMING:
            return new EffectDimming(p1 & 0xFF, p2 & 0xFF);
        case EFFECT_TYPE_FLICKER:
            return new EffectFlicker(p1 & 0xFF, p2 & 0xFF, p3 & 0xFF);
        case EFFECT_TYPE_STROBE:
            return new EffectStrobe(p1, p2 & 0xFF, p3 & 0xFF);
        case EFFECT_TYPE_MARS_LIGHT:
            return new EffectMarsLight(p1, p2 & 0xFF, static_cast<int8_t>(p3 & 0xFF));
        case EFFECT_TYPE_SOFT_START_STOP:
            return new EffectSoftStartStop(p1, p2, p3 & 0xFF);
        case EFFECT_TYPE_SERVO:
            return new EffectServo(p1 & 0xFF, p2 & 0xFF, p3 & 0xFF);
        case EFFECT_TYPE_SMOKE_GENERATOR:
            return new EffectSmokeGenerator((p1 & 0xFF) > 0, p2 & 0xFF);
        case EFFECT_TYPE_NONE:
        default:
            return new EffectSteady(255);
    }
}

void AuxController::parseRcn227PerOutputV2(ICVAccess& cvAccess) {
    cvAccess.writeCV(CV_INDEXED_CV_HIGH_BYTE, 0);
    cvAccess.writeCV(CV_INDEXED_CV_LOW_BYTE, RCN227_PER_OUTPUT_V2_PAGE);

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

            uint16_t blocking_cv_id = 0;
            if (blocking_func != 255) {
                ConditionVariable blocking_cv;
                blocking_cv.id = CV_ID_BASE_RCN227_PER_OUTPUT_V2_BLOCKING + blocking_func; // Unique ID
                if (blocking_func > 28) {
                    addToArray(blocking_cv.conditions, blocking_cv.conditions_count, blocking_cv.conditions_capacity, Condition{TriggerSource::BINARY_STATE, TriggerComparator::IS_TRUE, (uint16_t)(blocking_func)});
                } else {
                    addToArray(blocking_cv.conditions, blocking_cv.conditions_count, blocking_cv.conditions_capacity, Condition{TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, blocking_func});
                }
                addConditionVariable(blocking_cv);
                blocking_cv_id = blocking_cv.id;
            }

            for (int i = 0; i < 3; ++i) {
                if (funcs[i] != 255) {
                    if (lf == nullptr) {
                        lf = new LogicalFunction(createEffectFromCVs(cvAccess, output_num + 1));
                        lf->addOutput(getOutputById(output_num + 1));
                        addLogicalFunction(lf);
                    }
                    uint8_t lf_idx = _logical_functions_count - 1;

                    ConditionVariable cv;
                    cv.id = CV_ID_BASE_RCN227_PER_OUTPUT_V2 + (output_num * 8) + (dir * 4) + i; // Unique ID
                    if (funcs[i] > 28) {
                        addToArray(cv.conditions, cv.conditions_count, cv.conditions_capacity, Condition{TriggerSource::BINARY_STATE, TriggerComparator::IS_TRUE, (uint16_t)(funcs[i])});
                    } else {
                        addToArray(cv.conditions, cv.conditions_count, cv.conditions_capacity, Condition{TriggerSource::FUNC_KEY, TriggerComparator::IS_TRUE, funcs[i]});
                    }
                    addToArray(cv.conditions, cv.conditions_count, cv.conditions_capacity, Condition{TriggerSource::DIRECTION, TriggerComparator::EQ, (uint8_t)((dir == 0) ? DECODER_DIRECTION_FORWARD : DECODER_DIRECTION_REVERSE)});
                    addConditionVariable(cv);

                    MappingRule rule;
                    rule.target_logical_function_id = lf_idx;
                    addToArray(rule.positive_conditions, rule.positive_conditions_count, rule.positive_conditions_capacity, cv.id);
                    if (blocking_cv_id != 0) addToArray(rule.negative_conditions, rule.negative_conditions_count, rule.negative_conditions_capacity, blocking_cv_id);
                    rule.action = MappingAction::ACTIVATE;
                    addMappingRule(rule);
                }
            }
        }
    }
}
}
