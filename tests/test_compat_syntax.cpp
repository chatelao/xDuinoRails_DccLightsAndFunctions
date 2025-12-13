#include "mock_headers.h"
// Override path for this test
#define ARDUINOSTL_H "mock_headers.h"
#define __INC_FASTLED_H "mock_headers.h"

// Now include the compat file
#include "../src/compat/ArduinoSTL_AVR_Compat.h"

int main() {
    int* p = new int(5);
    delete p;
    return 0;
}
