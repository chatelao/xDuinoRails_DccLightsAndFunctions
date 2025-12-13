#ifndef ARDUINOSTL_AVR_COMPAT_H
#define ARDUINOSTL_AVR_COMPAT_H

#include <Arduino.h>

#if defined(ARDUINO_ARCH_AVR)

    // =========================================================================
    // COMPATIBILITY STRATEGY: "Let FastLED Win"
    // =========================================================================
    // 1. Include FastLED first. Let it define its placement new.
    #include <FastLED.h>

    // Clean up macros that might hurt STL
    #ifdef min
    #undef min
    #endif
    #ifdef max
    #undef max
    #endif

    // 2. FORCE suppression of ArduinoSTL's placement new
    // FastLED defines it, so we must prevent ArduinoSTL from defining it.
    // ArduinoSTL (uClibc++) typically checks __INPLACENEW_H or __INPLACENEW_H__
    // We define ALL known variations to be safe.
    #ifndef __INPLACENEW_H
    #define __INPLACENEW_H
    #endif
    #ifndef __INPLACENEW_H__
    #define __INPLACENEW_H__
    #endif
    #ifndef _INPLACENEW_H_
    #define _INPLACENEW_H_
    #endif
    #ifndef INPLACENEW_H
    #define INPLACENEW_H
    #endif

    // 3. Suppress ArduinoSTL's definition of GLOBAL operator new/delete
    // We will provide our own malloc-based implementation below.
    #ifndef _UCXX_NEW
    #define _UCXX_NEW
    #endif

    // 4. Include ArduinoSTL
    #include <ArduinoSTL.h>

    // Ensure std::bad_alloc is available (usually in <exception>)
    #include <exception>

    // 5. Global new/delete implementation (malloc wrappers)
    #include <stdlib.h>

    inline void* operator new(size_t size) {
        void* ptr = malloc(size);
        if (!ptr) {
            return NULL;
        }
        return ptr;
    }

    inline void* operator new[](size_t size) {
        return malloc(size);
    }

    inline void operator delete(void* ptr) {
        if (ptr) free(ptr);
    }

    inline void operator delete[](void* ptr) {
        if (ptr) free(ptr);
    }

    // Note: We DO NOT define placement new here.
    // FastLED has already defined it.

    // 6. Polyfill std::unique_ptr for C++11 on AVR (ArduinoSTL 1.3.3 misses this)
    namespace std {
        template<class T> struct _Unique_if {
            typedef unique_ptr<T> _Single_object;
        };

        template<class T> struct _Unique_if<T[]> {
            typedef unique_ptr<T[]> _Unknown_bound;
        };

        template<class T, size_t N> struct _Unique_if<T[N]> {
            typedef void _Known_bound;
        };

        template<class T, class... Args>
        typename _Unique_if<T>::_Single_object
        make_unique(Args&&... args) {
            return unique_ptr<T>(new T(static_cast<Args&&>(args)...));
        }

        template<class T>
        typename _Unique_if<T>::_Unknown_bound
        make_unique(size_t n) {
            typedef typename remove_extent<T>::type U;
            return unique_ptr<T>(new U[n]());
        }

        template<class T, class... Args>
        typename _Unique_if<T>::_Known_bound
        make_unique(Args&&...) = delete;
    }

#else
    // Non-AVR Platforms (e.g. ESP32, Desktop)
    #include <memory>
#endif

#endif // ARDUINOSTL_AVR_COMPAT_H
