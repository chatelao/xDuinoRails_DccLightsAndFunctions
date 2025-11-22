#ifndef ARDUINO_STL_AVR_COMPAT_H
#define ARDUINO_STL_AVR_COMPAT_H

#if defined(ARDUINO_ARCH_AVR)

#include <Arduino.h>
#undef min
#undef max

// Define guards to prevent FastLED from redefining placement new
// This assumes FastLED uses __INPLACENEW_H as the guard, which is standard for this file.
#ifndef __INPLACENEW_H
#define __INPLACENEW_H
#endif

// Ensure ArduinoSTL is included
#include <ArduinoSTL.h>
#include <vector>
#include <map>
#include <memory>
#include <utility>
#include <cstddef>

namespace std {

    // Polyfill for default_delete
    template<typename T>
    struct default_delete {
        void operator()(T* ptr) const {
            delete ptr;
        }
    };

    // Polyfill for unique_ptr
    template<typename T, typename D = default_delete<T>>
    class unique_ptr {
    public:
        typedef T element_type;
        typedef D deleter_type;
        typedef T* pointer;

        unique_ptr() : _ptr(nullptr) {}
        explicit unique_ptr(pointer p) : _ptr(p) {}

        // Move constructor
        unique_ptr(unique_ptr&& u) : _ptr(u.release()) {}

        ~unique_ptr() { reset(); }

        // Move assignment
        unique_ptr& operator=(unique_ptr&& u) {
            reset(u.release());
            return *this;
        }

        unique_ptr& operator=(decltype(nullptr)) {
            reset();
            return *this;
        }

        T& operator*() const { return *_ptr; }
        T* operator->() const { return _ptr; }
        T* get() const { return _ptr; }

        void reset(pointer p = nullptr) {
            if (_ptr != p) {
                if (_ptr) D()(_ptr);
                _ptr = p;
            }
        }

        pointer release() {
            pointer p = _ptr;
            _ptr = nullptr;
            return p;
        }

        explicit operator bool() const { return _ptr != nullptr; }

        // Disable copy
        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;

    private:
        pointer _ptr;
    };

}

#endif // ARDUINO_ARCH_AVR

#endif // ARDUINO_STL_AVR_COMPAT_H
