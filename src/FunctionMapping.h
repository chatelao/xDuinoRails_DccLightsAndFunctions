#ifndef FUNCTIONMAPPING_H
#define FUNCTIONMAPPING_H

#include <vector>
#include <cstdint>

namespace xDuinoRails {

class AuxController; // Forward declaration

enum class TriggerSource : uint8_t {
    NONE = 0,
    FUNC_KEY = 1,
    DIRECTION = 2,
    SPEED = 3,
    LOGICAL_FUNC_STATE = 4,
    BINARY_STATE = 5,
};

enum class TriggerComparator : uint8_t {
    NONE = 0,
    EQ = 1,
    NEQ = 2,
    GT = 3,
    LT = 4,
    GTE = 5,
    LTE = 6,
    BIT_AND = 7,
    IS_TRUE = 8,
};

enum class MappingAction : uint8_t {
    NONE = 0,
    ACTIVATE = 1,
    DEACTIVATE = 2,
    SET_DIMMED = 3,
};

enum class FunctionMappingMethod : uint8_t {
    PROPRIETARY = 0,
    RCN_225 = 1,
    RCN_227_PER_FUNCTION = 2,
    RCN_227_PER_OUTPUT_V1 = 3,
    RCN_227_PER_OUTPUT_V2 = 4,
    RCN_227_PER_OUTPUT_V3 = 5,
};

struct Condition {
    TriggerSource source;
    TriggerComparator comparator;
    uint8_t parameter;
};

struct ConditionVariable {
    uint8_t id;
    std::vector<Condition> conditions;
    bool evaluate(const AuxController& controller) const;
};

struct MappingRule {
    uint8_t target_logical_function_id;
    std::vector<uint8_t> positive_conditions;
    std::vector<uint8_t> negative_conditions;
    MappingAction action;
    bool evaluate(const AuxController& controller) const;
};

}

#endif // FUNCTIONMAPPING_H
