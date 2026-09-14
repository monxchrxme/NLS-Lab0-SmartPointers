#pragma once

#include "MemorySpan.hpp"

namespace custom {

template <typename T>
MemorySpan<T>::MemorySpan(std::size_t size)
    : size_(size), buffer_(MakeUnq<T[]>(size)) {}

template <typename T>
MemorySpan<T>::MemorySpan(std::size_t size, const T& default_val)
    : size_(size), buffer_(MakeUnq<T[]>(size)) {
    for (std::size_t i = 0; i < size_; ++i) {
        buffer_[i] = default_val;
    }
}

template <typename T>
std::size_t MemorySpan<T>::size() const noexcept {
    return size_;
}

template <typename T>
UnqPtr<T> MemorySpan<T>::Get(std::size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("MemorySpan::Get index out of range!");
    }
    return MakeUnq<T>(buffer_[index]);
}

template <typename T>
ShrdPtr<T> MemorySpan<T>::Copy(std::size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("MemorySpan::Copy index out of range!");
    }
    return MakeShrd<T>(buffer_[index]);
}

template <typename T>
MsPtr<T> MemorySpan<T>::Locate(std::size_t index) {
    if (index > size_) { // Разрешаем index == size_ для получения end()
        throw std::out_of_range("MemorySpan::Locate index out of range!");
    }
    T* raw = buffer_.get();
    return MsPtr<T>(raw + index, raw, raw + size_);
}

template <typename T>
MsPtr<T> MemorySpan<T>::begin() {
    return Locate(0);
}

template <typename T>
MsPtr<T> MemorySpan<T>::end() {
    return Locate(size_);
}

} // namespace custom