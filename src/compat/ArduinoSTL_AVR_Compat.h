#ifndef ARDUINOSTL_AVR_COMPAT_H
#define ARDUINOSTL_AVR_COMPAT_H

#include <Arduino.h>

#if defined(ARDUINO_ARCH_AVR)
    #include <ArduinoSTL.h>

    // ArduinoSTL typically lacks C++11 unique_ptr. Polyfill it.
    namespace std {
        template<typename T>
        class unique_ptr {
            T* ptr;
        public:
            explicit unique_ptr(T* p = nullptr) : ptr(p) {}
            ~unique_ptr() { delete ptr; }

            // Move constructor
            unique_ptr(unique_ptr&& other) : ptr(other.ptr) { other.ptr = nullptr; }

            // Move assignment
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

        // Simple implementation of make_unique (assuming move semantics work roughly)
        // If std::forward is missing, we might need to cast.
        template<typename T, typename... Args>
        unique_ptr<T> make_unique(Args&&... args) {
            return unique_ptr<T>(new T(static_cast<Args&&>(args)...));
        }
    }
#else
    // Non-AVR platforms (e.g. ESP32) usually have standard <memory>
    #include <memory>
#endif

#endif // ARDUINOSTL_AVR_COMPAT_H
