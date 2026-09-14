#pragma once

#include "SmrtPtr.hpp"

namespace custom {

template <typename T>
SmrtPtr<T>::SmrtPtr(T* ptr) : ptr_(ptr) {
    if (ptr_) {
        CentralStorage::instance().add_ref(ptr_);
    }
}

template <typename T>
SmrtPtr<T>::~SmrtPtr() {
    if (ptr_) {
        if (CentralStorage::instance().release_ref(ptr_)) {
            delete ptr_;
        }
        ptr_ = nullptr;
    }
}

template <typename T>
SmrtPtr<T>::SmrtPtr(const SmrtPtr& other) : ptr_(other.ptr_) {
    if (ptr_) {
        CentralStorage::instance().add_ref(ptr_);
    }
}

template <typename T>
SmrtPtr<T>& SmrtPtr<T>::operator=(const SmrtPtr& other) {
    if (this != &other) {
        // Сначала освобождаем наш текущий объект
        if (ptr_ && CentralStorage::instance().release_ref(ptr_)) {
            delete ptr_;
        }
        // Захватываем новый
        ptr_ = other.ptr_;
        if (ptr_) {
            CentralStorage::instance().add_ref(ptr_);
        }
    }
    return *this;
}

template <typename T>
SmrtPtr<T>::SmrtPtr(SmrtPtr&& other) noexcept : ptr_(other.ptr_) {
    other.ptr_ = nullptr; 
}

template <typename T>
SmrtPtr<T>& SmrtPtr<T>::operator=(SmrtPtr&& other) noexcept {
    if (this != &other) {
        if (ptr_ && CentralStorage::instance().release_ref(ptr_)) {
            delete ptr_;
        }
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
    }
    return *this;
}

template <typename T>
template <typename U>
    requires ConvertibleToPointer<U, T>
SmrtPtr<T>::SmrtPtr(const SmrtPtr<U>& other) : ptr_(other.ptr_) {
    if (ptr_) {
        CentralStorage::instance().add_ref(ptr_);
    }
}

template <typename T>
template <typename U>
    requires ConvertibleToPointer<U, T>
SmrtPtr<T>::SmrtPtr(SmrtPtr<U>&& other) noexcept : ptr_(other.ptr_) {
    other.ptr_ = nullptr;
}

template <typename T>
template <typename U>
    requires ConvertibleToPointer<U, T>
SmrtPtr<T>& SmrtPtr<T>::operator=(const SmrtPtr<U>& other) {
    if (ptr_ != other.ptr_) {
        if (ptr_ && CentralStorage::instance().release_ref(ptr_)) {
            delete ptr_;
        }
        ptr_ = other.ptr_;
        if (ptr_) {
            CentralStorage::instance().add_ref(ptr_);
        }
    }
    return *this;
}

template <typename T>
template <typename U>
    requires ConvertibleToPointer<U, T>
SmrtPtr<T>& SmrtPtr<T>::operator=(SmrtPtr<U>&& other) noexcept {
    if (ptr_ != other.ptr_) {
        if (ptr_ && CentralStorage::instance().release_ref(ptr_)) {
            delete ptr_;
        }
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
    }
    return *this;
}

template <typename T>
void SmrtPtr<T>::reset(T* new_ptr) {
    if (ptr_ != new_ptr) {
        if (ptr_ && CentralStorage::instance().release_ref(ptr_)) {
            delete ptr_;
        }
        ptr_ = new_ptr;
        if (ptr_) {
            CentralStorage::instance().add_ref(ptr_);
        }
    }
}

template <typename T>
void SmrtPtr<T>::swap(SmrtPtr& other) noexcept {
    std::swap(ptr_, other.ptr_);
}

template <typename T>
T* SmrtPtr<T>::get() const noexcept {
    return ptr_;
}

template <typename T>
std::size_t SmrtPtr<T>::use_count() const noexcept {
    return CentralStorage::instance().get_ref_count(ptr_);
}

template <typename T>
SmrtPtr<T>::operator bool() const noexcept {
    return ptr_ != nullptr;
}

template <typename T>
T& SmrtPtr<T>::operator*() const {
    return *ptr_;
}

template <typename T>
T* SmrtPtr<T>::operator->() const noexcept {
    return ptr_;
}

// Вспомогательные функции
template <typename T, typename... Args>
SmrtPtr<T> MakeSmrt(Args&&... args) {
    return SmrtPtr<T>(new T(std::forward<Args>(args)...));
}

template <typename T>
void swap(SmrtPtr<T>& lhs, SmrtPtr<T>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename T>
bool operator==(const SmrtPtr<T>& lhs, std::nullptr_t) noexcept {
    return !lhs;
}

template <typename T>
bool operator!=(const SmrtPtr<T>& lhs, std::nullptr_t) noexcept {
    return !(lhs == nullptr);
}

} // namespace custom