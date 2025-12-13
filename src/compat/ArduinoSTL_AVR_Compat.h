#ifndef ARDUINOSTL_AVR_COMPAT_H
#define ARDUINOSTL_AVR_COMPAT_H

#include <Arduino.h>

#if defined(ARDUINO_ARCH_AVR)

    // Suppress ArduinoSTL's definition of operator new/delete
    #ifndef _UCXX_NEW
    #define _UCXX_NEW
    #endif

    // Suppress FastLED's definition of placement new
    // We define all common guards used by different FastLED versions
    #ifndef __INPLACENEW_H
    #define __INPLACENEW_H 1
    #endif
    #ifndef __INPLACENEW_H__
    #define __INPLACENEW_H__ 1
    #endif
    #ifndef FASTLED_INPLACENEW_H
    #define FASTLED_INPLACENEW_H 1
    #endif
    #ifndef _INPLACENEW_H_
    #define _INPLACENEW_H_ 1
    #endif
    #ifndef INPLACENEW_H
    #define INPLACENEW_H 1
    #endif

    #include <ArduinoSTL.h>

    // We must manually provide what we suppressed from <new> and what FastLED would have provided

    // 1. bad_alloc (needed by vector)
    #include <exception>
    namespace std {
        class bad_alloc : public exception {
        public:
            virtual const char* what() const throw() { return "bad_alloc"; }
        };
        struct nothrow_t {};
        extern const nothrow_t nothrow;
    }

    // 2. Global new/delete
    inline void* operator new(size_t size) { return malloc(size); }
    inline void* operator new[](size_t size) { return malloc(size); }
    inline void operator delete(void* ptr) { free(ptr); }
    inline void operator delete[](void* ptr) { free(ptr); }

    // 3. Placement new/delete
    inline void* operator new(size_t size, void* ptr) { return ptr; }
    inline void* operator new[](size_t size, void* ptr) { return ptr; }
    inline void operator delete(void* ptr, void* voidptr2) {}
    inline void operator delete[](void* ptr, void* voidptr2) {}

    // 4. unique_ptr polyfill (ArduinoSTL 1.3.3 lacks it)
    namespace std {
        template<typename T>
        class unique_ptr {
            T* ptr;
        public:
            explicit unique_ptr(T* p = nullptr) : ptr(p) {}
            ~unique_ptr() { delete ptr; }
            unique_ptr(unique_ptr&& other) : ptr(other.ptr) { other.ptr = nullptr; }
            unique_ptr& operator=(unique_ptr&& other) {
                if (this != &other) {
                    delete ptr;
                    ptr = other.ptr;
                    other.ptr = nullptr;
                }
                return *this;
            }
            // Disable copy
            unique_ptr(const unique_ptr&) = delete;
            unique_ptr& operator=(const unique_ptr&) = delete;

            T* get() const { return ptr; }
            T& operator*() const { return *ptr; }
            T* operator->() const { return ptr; }
            T* release() { T* p = ptr; ptr = nullptr; return p; }
            void reset(T* p = nullptr) { delete ptr; ptr = p; }
            explicit operator bool() const { return ptr != nullptr; }
        };

        template<typename T, typename... Args>
        unique_ptr<T> make_unique(Args&&... args) {
            return unique_ptr<T>(new T(static_cast<Args&&>(args)...));
        }
    }

#else
    // Non-AVR
    #include <memory>
#endif

#endif // ARDUINOSTL_AVR_COMPAT_H
