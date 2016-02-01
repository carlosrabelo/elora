#pragma once

#include <new>
#include <type_traits>
#include <utility>

namespace elora {

template <typename T>
class Optional {
public:
    Optional() noexcept : has_(false) {}

    Optional(const T& value) : has_(true) { new (&storage_) T(value); }

    Optional(T&& value) : has_(true) { new (&storage_) T(std::move(value)); }

    Optional(const Optional& other) : has_(other.has_) {
        if (has_) {
            new (&storage_) T(*other);
        }
    }

    Optional(Optional&& other) noexcept(std::is_nothrow_move_constructible<T>::value) : has_(other.has_) {
        if (has_) {
            new (&storage_) T(std::move(*other));
        }
    }

    ~Optional() { reset(); }

    Optional& operator=(const Optional& other) {
        if (this == &other) {
            return *this;
        }
        if (other.has_) {
            *this = *other;
        } else {
            reset();
        }
        return *this;
    }

    Optional& operator=(Optional&& other) noexcept(std::is_nothrow_move_assignable<T>::value &&
                                                   std::is_nothrow_move_constructible<T>::value) {
        if (this == &other) {
            return *this;
        }
        if (other.has_) {
            *this = std::move(*other);
        } else {
            reset();
        }
        return *this;
    }

    Optional& operator=(const T& value) {
        if (has_) {
            *ptr() = value;
        } else {
            new (&storage_) T(value);
            has_ = true;
        }
        return *this;
    }

    Optional& operator=(T&& value) {
        if (has_) {
            *ptr() = std::move(value);
        } else {
            new (&storage_) T(std::move(value));
            has_ = true;
        }
        return *this;
    }

    void reset() {
        if (has_) {
            ptr()->~T();
            has_ = false;
        }
    }

    bool has_value() const noexcept { return has_; }
    explicit operator bool() const noexcept { return has_; }

    T& operator*() { return *ptr(); }
    const T& operator*() const { return *ptr(); }
    T* operator->() { return ptr(); }
    const T* operator->() const { return ptr(); }

private:
    T* ptr() { return reinterpret_cast<T*>(&storage_); }
    const T* ptr() const { return reinterpret_cast<const T*>(&storage_); }

    typename std::aligned_storage<sizeof(T), alignof(T)>::type storage_;
    bool has_;
};

}  // namespace elora
