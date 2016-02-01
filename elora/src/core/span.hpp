#pragma once

#include <cstddef>

namespace elora {

template <typename T>
class Span {
public:
    Span() noexcept : data_(nullptr), size_(0) {}

    Span(T* data, std::size_t size) noexcept : data_(data), size_(size) {}

    template <std::size_t N>
    Span(T (&arr)[N]) noexcept : data_(arr), size_(N) {}

    T* data() const noexcept { return data_; }
    std::size_t size() const noexcept { return size_; }
    bool empty() const noexcept { return size_ == 0; }
    T& operator[](std::size_t index) const { return data_[index]; }

private:
    T* data_;
    std::size_t size_;
};

}  // namespace elora
