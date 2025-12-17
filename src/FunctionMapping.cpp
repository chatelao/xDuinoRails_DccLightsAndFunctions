#include "FunctionMapping.h"
#include "xDuinoRails_DccLightsAndFunctions.h"
#include <cstring> // For memcpy

namespace xDuinoRails {

// --- ConditionVariable ---

ConditionVariable::~ConditionVariable() {
    delete[] conditions;
}

ConditionVariable::ConditionVariable(const ConditionVariable& other) {
    id = other.id;
    conditions_count = other.conditions_count;
    conditions_capacity = other.conditions_capacity;
    if (conditions_capacity > 0) {
        conditions = new Condition[conditions_capacity];
        memcpy(conditions, other.conditions, conditions_count * sizeof(Condition));
    } else {
        conditions = nullptr;
    }
}

ConditionVariable& ConditionVariable::operator=(const ConditionVariable& other) {
    if (this != &other) {
        delete[] conditions;
        id = other.id;
        conditions_count = other.conditions_count;
        conditions_capacity = other.conditions_capacity;
        if (conditions_capacity > 0) {
            conditions = new Condition[conditions_capacity];
            memcpy(conditions, other.conditions, conditions_count * sizeof(Condition));
        } else {
            conditions = nullptr;
        }
    }
    return *this;
}

// --- MappingRule ---

MappingRule::~MappingRule() {
    delete[] positive_conditions;
    delete[] negative_conditions;
}

MappingRule::MappingRule(const MappingRule& other) {
    target_logical_function_id = other.target_logical_function_id;
    action = other.action;

    positive_conditions_count = other.positive_conditions_count;
    positive_conditions_capacity = other.positive_conditions_capacity;
    if (positive_conditions_capacity > 0) {
        positive_conditions = new uint16_t[positive_conditions_capacity];
        memcpy(positive_conditions, other.positive_conditions, positive_conditions_count * sizeof(uint16_t));
    } else {
        positive_conditions = nullptr;
    }

    negative_conditions_count = other.negative_conditions_count;
    negative_conditions_capacity = other.negative_conditions_capacity;
    if (negative_conditions_capacity > 0) {
        negative_conditions = new uint16_t[negative_conditions_capacity];
        memcpy(negative_conditions, other.negative_conditions, negative_conditions_count * sizeof(uint16_t));
    } else {
        negative_conditions = nullptr;
    }
}

MappingRule& MappingRule::operator=(const MappingRule& other) {
    if (this != &other) {
        delete[] positive_conditions;
        delete[] negative_conditions;

        target_logical_function_id = other.target_logical_function_id;
        action = other.action;

        positive_conditions_count = other.positive_conditions_count;
        positive_conditions_capacity = other.positive_conditions_capacity;
        if (positive_conditions_capacity > 0) {
            positive_conditions = new uint16_t[positive_conditions_capacity];
            memcpy(positive_conditions, other.positive_conditions, positive_conditions_count * sizeof(uint16_t));
        } else {
            positive_conditions = nullptr;
        }

        negative_conditions_count = other.negative_conditions_count;
        negative_conditions_capacity = other.negative_conditions_capacity;
        if (negative_conditions_capacity > 0) {
            negative_conditions = new uint16_t[negative_conditions_capacity];
            memcpy(negative_conditions, other.negative_conditions, negative_conditions_count * sizeof(uint16_t));
        } else {
            negative_conditions = nullptr;
        }
    }
    return *this;
}

bool ConditionVariable::evaluate(const AuxController& controller) const {
    for (size_t i = 0; i < conditions_count; ++i) {
        const auto& cond = conditions[i];
        bool result = false;
        switch (cond.source) {
            case TriggerSource::FUNC_KEY:
                if (cond.comparator == TriggerComparator::IS_TRUE) result = controller.getFunctionState(cond.parameter);
                break;
            case TriggerSource::DIRECTION:
                if (cond.comparator == TriggerComparator::EQ) result = (controller.getDirection() == (DecoderDirection)cond.parameter);
                break;
            case TriggerSource::SPEED:
                if (cond.comparator == TriggerComparator::GT) result = (controller.getSpeed() > cond.parameter);
                break;
            case TriggerSource::BINARY_STATE:
                if (cond.comparator == TriggerComparator::IS_TRUE) result = controller.getBinaryState(cond.parameter);
                break;
            case TriggerSource::LOGICAL_FUNC_STATE:
                if (cond.comparator == TriggerComparator::IS_TRUE) {
                    const LogicalFunction* lf = controller.getLogicalFunction(cond.parameter);
                    result = (lf != nullptr && lf->isActive());
                }
                break;
            default: break;
        }
        if (!result) return false;
    }
    return true;
}

bool MappingRule::evaluate(const AuxController& controller) const {
    for (size_t i = 0; i < positive_conditions_count; ++i) {
        if (!controller.getConditionVariableState(positive_conditions[i])) return false;
    }
    for (size_t i = 0; i < negative_conditions_count; ++i) {
        if (controller.getConditionVariableState(negative_conditions[i])) return false;
    }
    return true;
}

}
