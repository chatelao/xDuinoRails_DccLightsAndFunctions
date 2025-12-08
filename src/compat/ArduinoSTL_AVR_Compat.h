#ifndef XDRAILS_ARDUINO_STL_AVR_COMPAT_H
#define XDRAILS_ARDUINO_STL_AVR_COMPAT_H

#if defined(ARDUINO_ARCH_AVR)

// Suppress ArduinoSTL's <new> definition to avoid conflicts with FastLED
#ifndef _NEW
#define _NEW
#endif

#include <Arduino.h>
#undef min
#undef max

// Remove <type_traits> as it is often missing in AVR STL ports (uClibc++).
// We include <utility> and <exception> hoping they exist for pair and exception base.
#include <utility>
#include <exception>

namespace xDuinoRails {
namespace internal {
    // Polyfill for type_traits helpers needed for move/forward
    template<class T> struct remove_reference { typedef T type; };
    template<class T> struct remove_reference<T&> { typedef T type; };
    template<class T> struct remove_reference<T&&> { typedef T type; };

    template<typename T>
    constexpr T&& forward(typename remove_reference<T>::type& t) noexcept {
        return static_cast<T&&>(t);
    }
    template<typename T>
    constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept {
        return static_cast<T&&>(t);
    }

    template<typename T>
    constexpr typename remove_reference<T>::type&& move(T&& t) noexcept {
        return static_cast<typename remove_reference<T>::type&&>(t);
    }
}
}

namespace std {
    struct nothrow_t {};
    extern const nothrow_t nothrow;

    // Polyfill bad_alloc if not provided (ArduinoSTL usually provides exception but maybe not bad_alloc)
    // We assume std::exception is available via <exception>
    class bad_alloc : public exception {
    public:
        virtual const char* what() const throw() {
            return "bad_alloc";
        }
    };
}

// Standard placement new
inline void* operator new(size_t, void* ptr) throw() { return ptr; }
inline void operator delete(void*, void*) throw() {}

// Polyfill std::unique_ptr for AVR
namespace std {
    template<typename T>
    struct default_delete {
        void operator()(T* ptr) const {
            delete ptr;
        }
    };

    template<typename T, typename D = default_delete<T>>
    class unique_ptr {
    public:
        typedef T* pointer;
        typedef T element_type;
        typedef D deleter_type;

        constexpr unique_ptr() noexcept : ptr_(nullptr) {}
        constexpr unique_ptr(nullptr_t) noexcept : ptr_(nullptr) {}
        explicit unique_ptr(pointer p) noexcept : ptr_(p) {}

        unique_ptr(unique_ptr&& u) noexcept : ptr_(u.release()) {}

        template<typename U, typename E>
        unique_ptr(unique_ptr<U, E>&& u) noexcept : ptr_(u.release()) {}

        ~unique_ptr() {
            if (ptr_) get_deleter()(ptr_);
        }

        unique_ptr& operator=(unique_ptr&& u) noexcept {
            reset(u.release());
            return *this;
        }

        unique_ptr& operator=(nullptr_t) noexcept {
            reset();
            return *this;
        }

        T& operator*() const { return *ptr_; }
        pointer operator->() const noexcept { return ptr_; }
        pointer get() const noexcept { return ptr_; }
        deleter_type& get_deleter() noexcept { return deleter_; }
        const deleter_type& get_deleter() const noexcept { return deleter_; }
        explicit operator bool() const noexcept { return ptr_ != nullptr; }

        pointer release() noexcept {
            pointer p = ptr_;
            ptr_ = nullptr;
            return p;
        }

        void reset(pointer p = pointer()) noexcept {
            pointer old = ptr_;
            ptr_ = p;
            if (old) get_deleter()(old);
        }

        void swap(unique_ptr& u) noexcept {
            pointer tmp = ptr_;
            ptr_ = u.ptr_;
            u.ptr_ = tmp;
        }

    private:
        pointer ptr_;
        D deleter_;

        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;
    };

    template<typename T, typename... Args>
    unique_ptr<T> make_unique(Args&&... args) {
        return unique_ptr<T>(new T(xDuinoRails::internal::forward<Args>(args)...));
    }
}

#endif // ARDUINO_ARCH_AVR
#endif // XDRAILS_ARDUINO_STL_AVR_COMPAT_H
