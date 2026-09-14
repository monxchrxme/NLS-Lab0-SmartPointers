#pragma once

#include <utility>
#include "UnqPtr.hpp"

namespace custom {

// ============================================================================
// Реализация UnqPtr<T>
// ============================================================================

template <typename T>
UnqPtr<T>::UnqPtr(T* ptr) noexcept : ptr_(ptr) {}

template <typename T>
UnqPtr<T>::~UnqPtr() {
    delete ptr_;
}

template <typename T>
UnqPtr<T>::UnqPtr(UnqPtr&& other) noexcept : ptr_(other.release()) {}

template <typename T>
UnqPtr<T>& UnqPtr<T>::operator=(UnqPtr&& other) noexcept {
    if (this != &other) {
        reset(other.release());
    }
    return *this;
}

template <typename T>
template <typename U>
    requires ConvertibleToPointer<U, T>
UnqPtr<T>::UnqPtr(UnqPtr<U>&& other) noexcept : ptr_(other.release()) {}

template <typename T>
template <typename U>
    requires ConvertibleToPointer<U, T>
UnqPtr<T>& UnqPtr<T>::operator=(UnqPtr<U>&& other) noexcept {
    reset(other.release());
    return *this;
}

template <typename T>
T* UnqPtr<T>::release() noexcept {
    T* old_ptr = ptr_;
    ptr_ = nullptr;
    return old_ptr;
}

template <typename T>
void UnqPtr<T>::reset(T* new_ptr) noexcept {
    if (ptr_ != new_ptr) {
        delete ptr_;
        ptr_ = new_ptr;
    }
}

template <typename T>
void UnqPtr<T>::swap(UnqPtr& other) noexcept {
    std::swap(ptr_, other.ptr_);
}

template <typename T>
T* UnqPtr<T>::get() const noexcept {
    return ptr_;
}

template <typename T>
UnqPtr<T>::operator bool() const noexcept {
    return ptr_ != nullptr;
}

template <typename T>
T& UnqPtr<T>::operator*() const {
    return *ptr_;
}

template <typename T>
T* UnqPtr<T>::operator->() const noexcept {
    return ptr_;
}

// ============================================================================
// Реализация UnqPtr<T[]> (Массивы)
// ============================================================================

template <typename T>
UnqPtr<T[]>::UnqPtr(T* ptr) noexcept : ptr_(ptr) {}

template <typename T>
UnqPtr<T[]>::~UnqPtr() {
    delete[] ptr_;
}

template <typename T>
UnqPtr<T[]>::UnqPtr(UnqPtr&& other) noexcept : ptr_(other.release()) {}

template <typename T>
UnqPtr<T[]>& UnqPtr<T[]>::operator=(UnqPtr&& other) noexcept {
    if (this != &other) {
        reset(other.release());
    }
    return *this;
}

template <typename T>
T* UnqPtr<T[]>::release() noexcept {
    T* old_ptr = ptr_;
    ptr_ = nullptr;
    return old_ptr;
}

template <typename T>
void UnqPtr<T[]>::reset(T* new_ptr) noexcept {
    if (ptr_ != new_ptr) {
        delete[] ptr_;
        ptr_ = new_ptr;
    }
}

template <typename T>
void UnqPtr<T[]>::swap(UnqPtr& other) noexcept {
    std::swap(ptr_, other.ptr_);
}

template <typename T>
T* UnqPtr<T[]>::get() const noexcept {
    return ptr_;
}

template <typename T>
UnqPtr<T[]>::operator bool() const noexcept {
    return ptr_ != nullptr;
}

template <typename T>
T& UnqPtr<T[]>::operator[](std::size_t index) const {
    return ptr_[index];
}

// ============================================================================
// Фабричные функции и вспомогательные операторы
// ============================================================================

template <typename T, typename... Args>
    requires (!std::is_array_v<T>)
UnqPtr<T> MakeUnq(Args&&... args) {
    return UnqPtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
    requires std::is_unbounded_array_v<T>
UnqPtr<T> MakeUnq(std::size_t size) {
    using ElementType = std::remove_extent_t<T>;
    return UnqPtr<T>(new ElementType[size]());
}

template <typename T>
void swap(UnqPtr<T>& lhs, UnqPtr<T>& rhs) noexcept {
    lhs.swap(rhs);
}

// Операторы сравнения с nullptr
template <typename T>
bool operator==(const UnqPtr<T>& lhs, std::nullptr_t) noexcept {
    return !lhs;
}

template <typename T>
bool operator!=(const UnqPtr<T>& lhs, std::nullptr_t) noexcept {
    return !(lhs == nullptr);
}

} // namespace custom