#ifndef ICVACCESS_H
#define ICVACCESS_H

#include <cstdint>

namespace xDuinoRails {

class ICVAccess {
public:
    virtual ~ICVAccess() {}
    virtual uint8_t readCV(uint16_t cv_number) = 0;
    virtual void writeCV(uint16_t cv_number, uint8_t value) = 0;
};

}

#endif // ICVACCESS_H
