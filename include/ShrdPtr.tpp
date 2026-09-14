#pragma once

#include "ShrdPtr.hpp"

namespace custom {

// ============================================================================
// Реализация ShrdPtr<T>
// ============================================================================

template <typename T>
void ShrdPtr<T>::release_internal() noexcept {
    if (ref_count_) {
        --(*ref_count_);
        if (*ref_count_ == 0) {
            delete ptr_;
            delete ref_count_;
        }
        ptr_ = nullptr;
        ref_count_ = nullptr;
    }
}

template <typename T>
ShrdPtr<T>::ShrdPtr(T* ptr) : ptr_(ptr), ref_count_(nullptr) {
    if (ptr_) {
        ref_count_ = new std::size_t(1);
    }
}

template <typename T>
ShrdPtr<T>::~ShrdPtr() {
    release_internal();
}

template <typename T>
ShrdPtr<T>::ShrdPtr(const ShrdPtr& other) noexcept
    : ptr_(other.ptr_), ref_count_(other.ref_count_) {
    if (ref_count_) {
        ++(*ref_count_);
    }
}

template <typename T>
ShrdPtr<T>& ShrdPtr<T>::operator=(const ShrdPtr& other) noexcept {
    if (this != &other) {
        ShrdPtr(other).swap(*this);
    }
    return *this;
}

template <typename T>
ShrdPtr<T>::ShrdPtr(ShrdPtr&& other) noexcept
    : ptr_(other.ptr_), ref_count_(other.ref_count_) {
    other.ptr_ = nullptr;
    other.ref_count_ = nullptr;
}

template <typename T>
ShrdPtr<T>& ShrdPtr<T>::operator=(ShrdPtr&& other) noexcept {
    if (this != &other) {
        ShrdPtr(std::move(other)).swap(*this);
    }
    return *this;
}

template <typename T>
template <typename U>
    requires ConvertibleToPointer<U, T>
ShrdPtr<T>::ShrdPtr(const ShrdPtr<U>& other) noexcept
    : ptr_(other.ptr_), ref_count_(other.ref_count_) {
    if (ref_count_) {
        ++(*ref_count_);
    }
}

template <typename T>
template <typename U>
    requires ConvertibleToPointer<U, T>
ShrdPtr<T>::ShrdPtr(ShrdPtr<U>&& other) noexcept
    : ptr_(other.ptr_), ref_count_(other.ref_count_) {
    other.ptr_ = nullptr;
    other.ref_count_ = nullptr;
}

template <typename T>
template <typename U>
    requires ConvertibleToPointer<U, T>
ShrdPtr<T>& ShrdPtr<T>::operator=(const ShrdPtr<U>& other) noexcept {
    ShrdPtr(other).swap(*this);
    return *this;
}

template <typename T>
template <typename U>
    requires ConvertibleToPointer<U, T>
ShrdPtr<T>& ShrdPtr<T>::operator=(ShrdPtr<U>&& other) noexcept {
    ShrdPtr(std::move(other)).swap(*this);
    return *this;
}

template <typename T>
void ShrdPtr<T>::reset() noexcept {
    release_internal();
}

template <typename T>
void ShrdPtr<T>::reset(T* new_ptr) {
    if (ptr_ != new_ptr) {
        release_internal();
        if (new_ptr) {
            ptr_ = new_ptr;
            ref_count_ = new std::size_t(1);
        }
    }
}

template <typename T>
void ShrdPtr<T>::swap(ShrdPtr& other) noexcept {
    std::swap(ptr_, other.ptr_);
    std::swap(ref_count_, other.ref_count_);
}

template <typename T>
T* ShrdPtr<T>::get() const noexcept {
    return ptr_;
}

template <typename T>
std::size_t ShrdPtr<T>::use_count() const noexcept {
    return ref_count_ ? *ref_count_ : 0;
}

template <typename T>
ShrdPtr<T>::operator bool() const noexcept {
    return ptr_ != nullptr;
}

template <typename T>
T& ShrdPtr<T>::operator*() const {
    return *ptr_;
}

template <typename T>
T* ShrdPtr<T>::operator->() const noexcept {
    return ptr_;
}

// ============================================================================
// Реализация ShrdPtr<T[]> (Массивы)
// ============================================================================

template <typename T>
void ShrdPtr<T[]>::release_internal() noexcept {
    if (ref_count_) {
        --(*ref_count_);
        if (*ref_count_ == 0) {
            delete[] ptr_; 
            delete ref_count_;
        }
        ptr_ = nullptr;
        ref_count_ = nullptr;
    }
}

template <typename T>
ShrdPtr<T[]>::ShrdPtr(T* ptr) : ptr_(ptr), ref_count_(nullptr) {
    if (ptr_) {
        ref_count_ = new std::size_t(1);
    }
}

template <typename T>
ShrdPtr<T[]>::~ShrdPtr() {
    release_internal();
}

template <typename T>
ShrdPtr<T[]>::ShrdPtr(const ShrdPtr& other) noexcept
    : ptr_(other.ptr_), ref_count_(other.ref_count_) {
    if (ref_count_) {
        ++(*ref_count_);
    }
}

template <typename T>
ShrdPtr<T[]>& ShrdPtr<T[]>::operator=(const ShrdPtr& other) noexcept {
    if (this != &other) {
        ShrdPtr(other).swap(*this);
    }
    return *this;
}

template <typename T>
ShrdPtr<T[]>::ShrdPtr(ShrdPtr&& other) noexcept
    : ptr_(other.ptr_), ref_count_(other.ref_count_) {
    other.ptr_ = nullptr;
    other.ref_count_ = nullptr;
}

template <typename T>
ShrdPtr<T[]>& ShrdPtr<T[]>::operator=(ShrdPtr&& other) noexcept {
    if (this != &other) {
        ShrdPtr(std::move(other)).swap(*this);
    }
    return *this;
}

template <typename T>
void ShrdPtr<T[]>::reset() noexcept {
    release_internal();
}

template <typename T>
void ShrdPtr<T[]>::reset(T* new_ptr) {
    if (ptr_ != new_ptr) {
        release_internal();
        if (new_ptr) {
            ptr_ = new_ptr;
            ref_count_ = new std::size_t(1);
        }
    }
}

template <typename T>
void ShrdPtr<T[]>::swap(ShrdPtr& other) noexcept {
    std::swap(ptr_, other.ptr_);
    std::swap(ref_count_, other.ref_count_);
}

template <typename T>
T* ShrdPtr<T[]>::get() const noexcept {
    return ptr_;
}

template <typename T>
std::size_t ShrdPtr<T[]>::use_count() const noexcept {
    return ref_count_ ? *ref_count_ : 0;
}

template <typename T>
ShrdPtr<T[]>::operator bool() const noexcept {
    return ptr_ != nullptr;
}

template <typename T>
T& ShrdPtr<T[]>::operator[](std::size_t index) const {
    return ptr_[index];
}

// ============================================================================
// Вспомогательные функции и операторы
// ============================================================================

template <typename T, typename... Args>
    requires (!std::is_array_v<T>)
ShrdPtr<T> MakeShrd(Args&&... args) {
    return ShrdPtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
    requires std::is_unbounded_array_v<T>
ShrdPtr<T> MakeShrd(std::size_t size) {
    using ElementType = std::remove_extent_t<T>;
    return ShrdPtr<T>(new ElementType[size]());
}

template <typename T>
void swap(ShrdPtr<T>& lhs, ShrdPtr<T>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename T>
bool operator==(const ShrdPtr<T>& lhs, std::nullptr_t) noexcept {
    return !lhs;
}

template <typename T>
bool operator!=(const ShrdPtr<T>& lhs, std::nullptr_t) noexcept {
    return !(lhs == nullptr);
}

} // namespace custom