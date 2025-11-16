#ifndef LOGICALFUNCTION_H
#define LOGICALFUNCTION_H

#include "effects/Effect.h"
#include "PhysicalOutput.h"
#include <vector>

namespace xDuinoRails {

class LogicalFunction {
public:
    LogicalFunction(Effect* effect);
    ~LogicalFunction();

    void addOutput(PhysicalOutput* output);
    void update(uint32_t delta_ms);
    void setActive(bool active);
    bool isActive() const;
    void setDimmed(bool dimmed);
    bool isDimmed() const;

private:
    Effect* _effect;
    std::vector<PhysicalOutput*> _outputs;
};

}

#endif // LOGICALFUNCTION_H
