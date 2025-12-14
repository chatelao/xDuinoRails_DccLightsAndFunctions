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

    // 6. Polyfill std::unique_ptr for C++11 on AVR (ArduinoSTL 1.3.3 misses this)
    // We must define it inside namespace std.
    namespace std {

        // Basic remove_extent (needed for make_unique<T[]>)
        template<class T> struct remove_extent { typedef T type; };
        template<class T> struct remove_extent<T[]> { typedef T type; };
        template<class T, size_t N> struct remove_extent<T[N]> { typedef T type; };

        // Default deleters
        template<class T> struct default_delete {
            void operator()(T* ptr) const {
                // We use global delete
                ::operator delete(ptr);
            }
        };

        template<class T> struct default_delete<T[]> {
            void operator()(T* ptr) const {
                ::operator delete[](ptr);
            }
        };

        // Minimal unique_ptr implementation
        template <class T, class D = default_delete<T> >
        class unique_ptr {
        public:
            typedef T element_type;
            typedef D deleter_type;
            typedef T* pointer;

            // Constructors
            explicit unique_ptr(pointer p = NULL) : _ptr(p) {}

            // Move constructor
            unique_ptr(unique_ptr&& u) : _ptr(u.release()) {
                // deleter is stateless in this simple impl
            }

            // Destructor
            ~unique_ptr() {
                reset();
            }

            // Move assignment
            unique_ptr& operator=(unique_ptr&& u) {
                if (this != &u) {
                    reset(u.release());
                }
                return *this;
            }

            // No copy
            unique_ptr(const unique_ptr&) = delete;
            unique_ptr& operator=(const unique_ptr&) = delete;

            // Modifiers
            pointer release() {
                pointer p = _ptr;
                _ptr = NULL;
                return p;
            }

            void reset(pointer p = NULL) {
                if (_ptr != p) {
                    if (_ptr) D()(_ptr);
                    _ptr = p;
                }
            }

            void swap(unique_ptr& u) {
                pointer tmp = _ptr;
                _ptr = u._ptr;
                u._ptr = tmp;
            }

            // Observers
            pointer get() const { return _ptr; }
            D& get_deleter() { return _deleter; }
            const D& get_deleter() const { return _deleter; }

            // Dereference
            typename add_lvalue_reference<T>::type operator*() const { return *_ptr; }
            pointer operator->() const { return _ptr; }

            // Bool conversion
            explicit operator bool() const { return _ptr != NULL; }

        private:
            pointer _ptr;
            D _deleter;
        };

        // Array specialization
        template <class T, class D>
        class unique_ptr<T[], D> {
        public:
            typedef T element_type;
            typedef D deleter_type;
            typedef T* pointer;

            explicit unique_ptr(pointer p = NULL) : _ptr(p) {}
            unique_ptr(unique_ptr&& u) : _ptr(u.release()) {}
            ~unique_ptr() { reset(); }

            unique_ptr& operator=(unique_ptr&& u) {
                if (this != &u) reset(u.release());
                return *this;
            }

            unique_ptr(const unique_ptr&) = delete;
            unique_ptr& operator=(const unique_ptr&) = delete;

            pointer release() {
                pointer p = _ptr;
                _ptr = NULL;
                return p;
            }

            void reset(pointer p = NULL) {
                if (_ptr != p) {
                    if (_ptr) D()(_ptr);
                    _ptr = p;
                }
            }

            pointer get() const { return _ptr; }
            T& operator[](size_t i) const { return _ptr[i]; }
            explicit operator bool() const { return _ptr != NULL; }

        private:
            pointer _ptr;
            D _deleter;
        };

        // make_unique support structs
        template<class T> struct _Unique_if {
            typedef unique_ptr<T> _Single_object;
        };

        template<class T> struct _Unique_if<T[]> {
            typedef unique_ptr<T[]> _Unknown_bound;
        };

        template<class T, size_t N> struct _Unique_if<T[N]> {
            typedef void _Known_bound;
        };

        // make_unique implementation
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
