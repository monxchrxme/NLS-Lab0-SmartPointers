#pragma once

#include <cstddef>
#include <utility>
#include "Concepts.hpp"
#include "CentralStorage.hpp"

namespace custom {

template <typename T>
class SmrtPtr;

template <typename T>
class SmrtPtr {
private:
    T* ptr_ = nullptr; 

    template <typename U>
    friend class SmrtPtr;

public:
    using element_type = T;
    using pointer = T*;

    // Конструкторы
    constexpr SmrtPtr() noexcept = default;
    constexpr SmrtPtr(std::nullptr_t) noexcept : ptr_(nullptr) {}
    explicit SmrtPtr(T* ptr);

    // Деструктор
    ~SmrtPtr();

    // Копирование
    SmrtPtr(const SmrtPtr& other);
    SmrtPtr& operator=(const SmrtPtr& other);

    // Перемещение
    SmrtPtr(SmrtPtr&& other) noexcept;
    SmrtPtr& operator=(SmrtPtr&& other) noexcept;

    // Конструкторы подтипизации (Upcasting: Derived -> Base)
    template <typename U>
        requires ConvertibleToPointer<U, T>
    SmrtPtr(const SmrtPtr<U>& other);

    template <typename U>
        requires ConvertibleToPointer<U, T>
    SmrtPtr(SmrtPtr<U>&& other) noexcept;

    template <typename U>
        requires ConvertibleToPointer<U, T>
    SmrtPtr& operator=(const SmrtPtr<U>& other);

    template <typename U>
        requires ConvertibleToPointer<U, T>
    SmrtPtr& operator=(SmrtPtr<U>&& other) noexcept;

    // Модификаторы
    void reset(T* new_ptr = nullptr);
    void swap(SmrtPtr& other) noexcept;

    // Наблюдатели
    T* get() const noexcept;
    std::size_t use_count() const noexcept;
    explicit operator bool() const noexcept;
    T& operator*() const;
    T* operator->() const noexcept;
};

// Фабричная функция MakeSmrt
template <typename T, typename... Args>
SmrtPtr<T> MakeSmrt(Args&&... args);

template <typename T>
void swap(SmrtPtr<T>& lhs, SmrtPtr<T>& rhs) noexcept;

template <typename T>
bool operator==(const SmrtPtr<T>& lhs, std::nullptr_t) noexcept;

template <typename T>
bool operator!=(const SmrtPtr<T>& lhs, std::nullptr_t) noexcept;

} // namespace custom

#include "SmrtPtr.tpp"