#pragma once

#include "DynamicArraySequence.hpp"

namespace custom {

template <typename T>
void DynamicArraySequence<T>::reserve(std::size_t new_capacity) {
    if (new_capacity <= capacity_) return;

    // Выделяем новый буфер через MakeUnq
    auto new_buffer = MakeUnq<T[]>(new_capacity);

    // Переносим существующие элементы в новый буфер
    for (std::size_t i = 0; i < size_; ++i) {
        new_buffer[i] = std::move(buffer_[i]);
    }

    // Передаем владение
    buffer_ = std::move(new_buffer);
    capacity_ = new_capacity;
}

template <typename T>
DynamicArraySequence<T>::DynamicArraySequence()
    : size_(0), capacity_(0), buffer_(nullptr) {}

template <typename T>
DynamicArraySequence<T>::DynamicArraySequence(std::size_t count)
    : size_(count), capacity_(count), buffer_(MakeUnq<T[]>(count)) {}

template <typename T>
DynamicArraySequence<T>::DynamicArraySequence(const T* items, std::size_t count)
    : size_(count), capacity_(count), buffer_(MakeUnq<T[]>(count)) {
    for (std::size_t i = 0; i < count; ++i) {
        buffer_[i] = items[i];
    }
}

template <typename T>
DynamicArraySequence<T>::DynamicArraySequence(const DynamicArraySequence& other)
    : size_(other.size_), capacity_(other.size_), buffer_(MakeUnq<T[]>(other.size_)) {
    for (std::size_t i = 0; i < size_; ++i) {
        buffer_[i] = other.buffer_[i];
    }
}

template <typename T>
DynamicArraySequence<T>& DynamicArraySequence<T>::operator=(const DynamicArraySequence& other) {
    if (this != &other) {
        DynamicArraySequence temp(other);
        // Обмениваем поля
        std::swap(size_, temp.size_);
        std::swap(capacity_, temp.capacity_);
        buffer_.swap(temp.buffer_);
    }
    return *this;
}

template <typename T>
T& DynamicArraySequence<T>::GetFirst() {
    if (size_ == 0) throw std::out_of_range("Sequence is empty!");
    return buffer_[0];
}

template <typename T>
const T& DynamicArraySequence<T>::GetFirst() const {
    if (size_ == 0) throw std::out_of_range("Sequence is empty!");
    return buffer_[0];
}

template <typename T>
T& DynamicArraySequence<T>::GetLast() {
    if (size_ == 0) throw std::out_of_range("Sequence is empty!");
    return buffer_[size_ - 1];
}

template <typename T>
const T& DynamicArraySequence<T>::GetLast() const {
    if (size_ == 0) throw std::out_of_range("Sequence is empty!");
    return buffer_[size_ - 1];
}

template <typename T>
T& DynamicArraySequence<T>::Get(std::size_t index) {
    if (index >= size_) throw std::out_of_range("Index out of range!");
    return buffer_[index];
}

template <typename T>
const T& DynamicArraySequence<T>::Get(std::size_t index) const {
    if (index >= size_) throw std::out_of_range("Index out of range!");
    return buffer_[index];
}

template <typename T>
T& DynamicArraySequence<T>::operator[](std::size_t index) {
    return buffer_[index];
}

template <typename T>
const T& DynamicArraySequence<T>::operator[](std::size_t index) const {
    return buffer_[index];
}

template <typename T>
UnqPtr<T> DynamicArraySequence<T>::GetUnique(std::size_t index) const {
    if (index >= size_) throw std::out_of_range("Index out of range!");
    return MakeUnq<T>(buffer_[index]);
}

template <typename T>
ShrdPtr<T> DynamicArraySequence<T>::GetShared(std::size_t index) const {
    if (index >= size_) throw std::out_of_range("Index out of range!");
    return MakeShrd<T>(buffer_[index]);
}

template <typename T>
std::size_t DynamicArraySequence<T>::GetLength() const noexcept {
    return size_;
}

template <typename T>
std::size_t DynamicArraySequence<T>::GetCapacity() const noexcept {
    return capacity_;
}

template <typename T>
bool DynamicArraySequence<T>::IsEmpty() const noexcept {
    return size_ == 0;
}

template <typename T>
void DynamicArraySequence<T>::Append(const T& item) {
    if (size_ == capacity_) {
        reserve(capacity_ == 0 ? 4 : capacity_ * 2);
    }
    buffer_[size_] = item;
    ++size_;
}

template <typename T>
void DynamicArraySequence<T>::Prepend(const T& item) {
    InsertAt(item, 0);
}

template <typename T>
void DynamicArraySequence<T>::InsertAt(const T& item, std::size_t index) {
    if (index > size_) throw std::out_of_range("Insert index out of range!");

    if (size_ == capacity_) {
        reserve(capacity_ == 0 ? 4 : capacity_ * 2);
    }

    for (std::size_t i = size_; i > index; --i) {
        buffer_[i] = std::move(buffer_[i - 1]);
    }

    buffer_[index] = item;
    ++size_;
}

} // namespace custom