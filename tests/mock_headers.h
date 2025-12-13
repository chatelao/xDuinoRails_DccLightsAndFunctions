
// Mock Arduino.h
#ifndef Arduino_h
#define Arduino_h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
class Print {};
#endif

// Mock FastLED.h
#ifndef __INC_FASTLED_H
#define __INC_FASTLED_H
// FastLED usually defines placement new like this:
inline void* operator new(size_t, void* ptr) { return ptr; }
#endif

// Mock ArduinoSTL.h
#ifndef ARDUINOSTL_H
#define ARDUINOSTL_H
// ArduinoSTL checks _UCXX_NEW. If not defined, it defines global new.
#ifndef _UCXX_NEW
inline void* operator new(size_t size) { return malloc(size); }
// ... etc
#endif
namespace std {
    class bad_alloc {};
    template<typename T> class unique_ptr {
    public:
        unique_ptr(T* p = NULL) : ptr(p) {}
        ~unique_ptr() { delete ptr; }
        T* ptr;
    };
    template<typename T> struct remove_extent { typedef T type; };
}
#endif
