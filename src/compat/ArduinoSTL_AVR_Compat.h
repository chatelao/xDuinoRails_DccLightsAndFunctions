#ifndef ARDUINOSTL_AVR_COMPAT_H
#define ARDUINOSTL_AVR_COMPAT_H

#ifdef ARDUINO_ARCH_AVR

#include <stddef.h>
#include <type_traits>

// Suppress new/delete from FastLED and ArduinoSTL to provide our own.
#define __INPLACENEW_H
#define _UCXX_NEW

// Global new/delete operators
void* operator new(size_t size);
void operator delete(void* ptr);
void* operator new[](size_t size);
void operator delete[](void* ptr);

// Placement new
inline void* operator new(size_t, void* ptr) { return ptr; }
inline void operator delete(void*, void*) {}

namespace std {

// Forward declaration for unique_ptr
template <typename T, typename Deleter = std::default_delete<T>> class unique_ptr;

// Deleter for single objects
template <typename T>
struct default_delete {
    constexpr default_delete() noexcept = default;
    void operator()(T* ptr) const {
        static_assert(sizeof(T) > 0, "can't delete an incomplete type");
        delete ptr;
    }
};

// Deleter for arrays
template <typename T>
struct default_delete<T[]> {
    void operator()(T* ptr) const {
        static_assert(sizeof(T) > 0, "can't delete an incomplete type");
        delete[] ptr;
    }
};

// unique_ptr for single objects
template <typename T, typename Deleter>
class unique_ptr {
public:
    using pointer = T*;
    using element_type = T;
    using deleter_type = Deleter;

    constexpr unique_ptr() noexcept : _ptr(nullptr) {}
    constexpr unique_ptr(std::nullptr_t) noexcept : _ptr(nullptr) {}
    explicit unique_ptr(pointer p) noexcept : _ptr(p) {}

    unique_ptr(unique_ptr&& u) noexcept : _ptr(u.release()) {}

    template<typename U, typename E>
    unique_ptr(unique_ptr<U, E>&& u) noexcept : _ptr(u.release()) {}


    ~unique_ptr() {
        if (_ptr) {
            get_deleter()(_ptr);
        }
    }

    unique_ptr& operator=(unique_ptr&& u) noexcept {
        reset(u.release());
        return *this;
    }

    unique_ptr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    pointer release() noexcept {
        pointer p = _ptr;
        _ptr = nullptr;
        return p;
    }

    void reset(pointer p = pointer()) noexcept {
        if (_ptr) {
            get_deleter()(_ptr);
        }
        _ptr = p;
    }

    void swap(unique_ptr& u) noexcept {
        using std::swap;
        swap(_ptr, u._ptr);
    }

    pointer get() const noexcept { return _ptr; }
    deleter_type& get_deleter() noexcept { return _deleter; }
    const deleter_type& get_deleter() const noexcept { return _deleter; }

    explicit operator bool() const noexcept { return _ptr != nullptr; }

    typename std::add_lvalue_reference<element_type>::type operator*() const { return *_ptr; }
    pointer operator->() const noexcept { return _ptr; }

    // Disable copy semantics
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

private:
    pointer _ptr;
    deleter_type _deleter;
};

// unique_ptr for arrays
template <typename T, typename Deleter>
class unique_ptr<T[], Deleter> {
public:
    using pointer = T*;
    using element_type = T;
    using deleter_type = Deleter;

    constexpr unique_ptr() noexcept : _ptr(nullptr) {}
    constexpr unique_ptr(std::nullptr_t) noexcept : _ptr(nullptr) {}
    explicit unique_ptr(pointer p) noexcept : _ptr(p) {}

    unique_ptr(unique_ptr&& u) noexcept : _ptr(u.release()) {}

    ~unique_ptr() {
        if (_ptr) {
            get_deleter()(_ptr);
        }
    }

    unique_ptr& operator=(unique_ptr&& u) noexcept {
        reset(u.release());
        return *this;
    }

    unique_ptr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    pointer release() noexcept {
        pointer p = _ptr;
        _ptr = nullptr;
        return p;
    }

    void reset(pointer p = pointer()) noexcept {
        if (_ptr) {
            get_deleter()(_ptr);
        }
        _ptr = p;
    }

    void swap(unique_ptr& u) noexcept {
        using std::swap;
        swap(_ptr, u._ptr);
    }

    pointer get() const noexcept { return _ptr; }
    deleter_type& get_deleter() noexcept { return _deleter; }
    const deleter_type& get_deleter() const noexcept { return _deleter; }


    explicit operator bool() const noexcept { return _ptr != nullptr; }

    T& operator[](size_t i) const { return _ptr[i]; }

    // Disable copy semantics
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

private:
    pointer _ptr;
    deleter_type _deleter;
};


// make_unique for single objects
template <typename T, typename... Args>
typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// make_unique for arrays with known bounds
template <typename T>
typename std::enable_if<std::is_array<T>::value && std::extent<T>::value != 0, std::unique_ptr<T>>::type
make_unique() {
    return std::unique_ptr<T>(new typename std::remove_extent<T>::type[std::extent<T>::value]());
}

// make_unique for arrays with unknown bounds
template <typename T, typename... Args>
typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0, std::unique_ptr<T>>::type
make_unique(size_t size) {
    return std::unique_ptr<T>(new typename std::remove_extent<T>::type[size]());
}

} // namespace std

#endif // ARDUINO_ARCH_AVR
#endif // ARDUINOSTL_AVR_COMPAT_H
