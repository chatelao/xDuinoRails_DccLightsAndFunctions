#ifndef ARDUINO_STL_AVR_COMPAT_H
#define ARDUINO_STL_AVR_COMPAT_H

#if defined(ARDUINO_ARCH_AVR)

#include <Arduino.h>
#undef min
#undef max

// Suppress ArduinoSTL's <new> header to avoid conflict with FastLED's placement new.
// This macro must be defined before any STL header is included.
#ifndef _NEW
#define _NEW
#endif

// Include FastLED (provides placement new)
#include <FastLED.h>

// Include ArduinoSTL and exception
#include <ArduinoSTL.h>
#include <exception>

// Polyfill functionality normally provided by <new>
namespace std {
    class bad_alloc : public exception {
    public:
        bad_alloc() throw() { }
        virtual ~bad_alloc() throw() { }
        virtual const char* what() const throw() { return "bad_alloc"; }
    };

    struct nothrow_t {};
    extern const nothrow_t nothrow;

    typedef void (*new_handler)();
    new_handler set_new_handler(new_handler new_p) throw();
}

// Declare allocation operators (implemented in ArduinoSTL library)
void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr);
void operator delete[](void* ptr);

void* operator new(size_t size, const std::nothrow_t&) throw();
void* operator new[](size_t size, const std::nothrow_t&) throw();
void operator delete(void* ptr, const std::nothrow_t&) throw();
void operator delete[](void* ptr, const std::nothrow_t&) throw();

// Include other standard headers needed
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

        // Converting move constructor (Derived -> Base)
        template<typename U, typename E>
        unique_ptr(unique_ptr<U, E>&& u) : _ptr(u.release()) {}

        ~unique_ptr() { reset(); }

        // Move assignment
        unique_ptr& operator=(unique_ptr&& u) {
            reset(u.release());
            return *this;
        }

        // Converting move assignment
        template<typename U, typename E>
        unique_ptr& operator=(unique_ptr<U, E>&& u) {
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
