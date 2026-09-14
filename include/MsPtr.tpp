#pragma once

#include "MsPtr.hpp"

namespace custom {

template <typename T>
MsPtr<T>::MsPtr(T* current, T* begin, T* end) noexcept
    : current_(current), begin_(begin), end_(end) {}

template <typename T>
T& MsPtr<T>::operator*() const {
    if (!current_ || current_ < begin_ || current_ >= end_) {
        throw std::out_of_range("MsPtr: dereferencing pointer out of bounds!");
    }
    return *current_;
}

template <typename T>
T* MsPtr<T>::operator->() const {
    if (!current_ || current_ < begin_ || current_ >= end_) {
        throw std::out_of_range("MsPtr: member access out of bounds!");
    }
    return current_;
}

template <typename T>
T& MsPtr<T>::operator[](std::ptrdiff_t index) const {
    T* target = current_ + index;
    if (!target || target < begin_ || target >= end_) {
        throw std::out_of_range("MsPtr: array subscript out of bounds!");
    }
    return *target;
}

template <typename T>
MsPtr<T>& MsPtr<T>::operator++() {
    if (current_ >= end_) {
        throw std::out_of_range("MsPtr: prefix increment past end of buffer!");
    }
    ++current_;
    return *this;
}

template <typename T>
MsPtr<T> MsPtr<T>::operator++(int) {
    MsPtr tmp = *this;
    ++(*this);
    return tmp;
}

template <typename T>
MsPtr<T>& MsPtr<T>::operator--() {
    if (current_ <= begin_) {
        throw std::out_of_range("MsPtr: prefix decrement before begin of buffer!");
    }
    --current_;
    return *this;
}

template <typename T>
MsPtr<T> MsPtr<T>::operator--(int) {
    MsPtr tmp = *this;
    --(*this);
    return tmp;
}

template <typename T>
MsPtr<T> MsPtr<T>::operator+(std::ptrdiff_t offset) const {
    T* target = current_ + offset;
    // Разрешено указывать в диапазоне [begin_, end_] (включая end как past-the-end)
    if (target < begin_ || target > end_) {
        throw std::out_of_range("MsPtr: offset addition out of bounds!");
    }
    return MsPtr(target, begin_, end_);
}

template <typename T>
MsPtr<T> MsPtr<T>::operator-(std::ptrdiff_t offset) const {
    return *this + (-offset);
}

template <typename T>
MsPtr<T>& MsPtr<T>::operator+=(std::ptrdiff_t offset) {
    *this = *this + offset;
    return *this;
}

template <typename T>
MsPtr<T>& MsPtr<T>::operator-=(std::ptrdiff_t offset) {
    *this = *this - offset;
    return *this;
}

template <typename T>
std::ptrdiff_t MsPtr<T>::operator-(const MsPtr& other) const {
    if (begin_ != other.begin_ || end_ != other.end_) {
        throw std::invalid_argument("MsPtr: cannot subtract pointers from different MemorySpan buffers!");
    }
    return current_ - other.current_;
}

template <typename T>
bool MsPtr<T>::operator==(const MsPtr& other) const noexcept {
    return current_ == other.current_;
}

template <typename T>
bool MsPtr<T>::operator!=(const MsPtr& other) const noexcept {
    return !(*this == other);
}

template <typename T>
bool MsPtr<T>::operator<(const MsPtr& other) const {
    if (begin_ != other.begin_ || end_ != other.end_) {
        throw std::invalid_argument("MsPtr: cannot compare pointers from different MemorySpan buffers!");
    }
    return current_ < other.current_;
}

template <typename T>
bool MsPtr<T>::operator>(const MsPtr& other) const {
    return other < *this;
}

template <typename T>
bool MsPtr<T>::operator<=(const MsPtr& other) const {
    return !(other < *this);
}

template <typename T>
bool MsPtr<T>::operator>=(const MsPtr& other) const {
    return !(*this < other);
}

template <typename T>
T* MsPtr<T>::get() const noexcept {
    return current_;
}

template <typename T>
bool MsPtr<T>::is_end() const noexcept {
    return current_ == end_;
}

} // namespace custom