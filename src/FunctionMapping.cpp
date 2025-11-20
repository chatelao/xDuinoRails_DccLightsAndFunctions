#include "FunctionMapping.h"
#include "AuxController.h"

namespace xDuinoRails {

bool ConditionVariable::evaluate(const AuxController& controller) const {
    for (const auto& cond : conditions) {
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
            default: break;
        }
        if (!result) return false;
    }
    return true;
}

bool MappingRule::evaluate(const AuxController& controller) const {
    for (uint16_t id : positive_conditions) {
        if (!controller.getConditionVariableState(id)) return false;
    }
    for (uint16_t id : negative_conditions) {
        if (controller.getConditionVariableState(id)) return false;
    }
    return true;
}

}
