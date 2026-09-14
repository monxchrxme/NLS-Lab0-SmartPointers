#pragma once

#include <cstddef>
#include <utility>
#include <type_traits>
#include "Concepts.hpp"

namespace custom {

template <typename T>
class ShrdPtr;

// ============================================================================
// Основной шаблон ShrdPtr<T> (Одиночные объекты)
// ============================================================================
template <typename T>
class ShrdPtr {
private:
    T* ptr_ = nullptr;
    std::size_t* ref_count_ = nullptr;

    // Дружественный доступ для шаблонных копирований между разными типами
    template <typename U>
    friend class ShrdPtr;

    // Вспомогательный метод для декремента и освобождения ресурсов
    void release_internal() noexcept;

public:
    using element_type = T;
    using pointer = T*;

    // Конструкторы
    constexpr ShrdPtr() noexcept = default;
    constexpr ShrdPtr(std::nullptr_t) noexcept : ptr_(nullptr), ref_count_(nullptr) {}
    explicit ShrdPtr(T* ptr);

    // Деструктор
    ~ShrdPtr();

    // Копирование (увеличивает счетчик)
    ShrdPtr(const ShrdPtr& other) noexcept;
    ShrdPtr& operator=(const ShrdPtr& other) noexcept;

    // Перемещение (забирает владение без изменения счетчика)
    ShrdPtr(ShrdPtr&& other) noexcept;
    ShrdPtr& operator=(ShrdPtr&& other) noexcept;

    // Конструкторы с подтипизацией (Upcasting: Derived -> Base)
    template <typename U>
        requires ConvertibleToPointer<U, T>
    ShrdPtr(const ShrdPtr<U>& other) noexcept;

    template <typename U>
        requires ConvertibleToPointer<U, T>
    ShrdPtr(ShrdPtr<U>&& other) noexcept;

    template <typename U>
        requires ConvertibleToPointer<U, T>
    ShrdPtr& operator=(const ShrdPtr<U>& other) noexcept;

    template <typename U>
        requires ConvertibleToPointer<U, T>
    ShrdPtr& operator=(ShrdPtr<U>&& other) noexcept;

    // Модификаторы
    void reset() noexcept;
    void reset(T* new_ptr);
    void swap(ShrdPtr& other) noexcept;

    T* get() const noexcept;
    std::size_t use_count() const noexcept;
    explicit operator bool() const noexcept;
    T& operator*() const;
    T* operator->() const noexcept;
};

// ============================================================================
// Частичная специализация ShrdPtr<T[]> (Динамические массивы)
// ============================================================================
template <typename T>
class ShrdPtr<T[]> {
private:
    T* ptr_ = nullptr;
    std::size_t* ref_count_ = nullptr;

    template <typename U>
    friend class ShrdPtr;

    void release_internal() noexcept;

public:
    using element_type = T;
    using pointer = T*;

    constexpr ShrdPtr() noexcept = default;
    constexpr ShrdPtr(std::nullptr_t) noexcept : ptr_(nullptr), ref_count_(nullptr) {}
    explicit ShrdPtr(T* ptr);

    ~ShrdPtr();

    ShrdPtr(const ShrdPtr& other) noexcept;
    ShrdPtr& operator=(const ShrdPtr& other) noexcept;

    ShrdPtr(ShrdPtr&& other) noexcept;
    ShrdPtr& operator=(ShrdPtr&& other) noexcept;

    void reset() noexcept;
    void reset(T* new_ptr);
    void swap(ShrdPtr& other) noexcept;

    T* get() const noexcept;
    std::size_t use_count() const noexcept;
    explicit operator bool() const noexcept;

    T& operator[](std::size_t index) const;
};

// Фабричная функция MakeShrd (аналог std::make_shared)
template <typename T, typename... Args>
    requires (!std::is_array_v<T>)
ShrdPtr<T> MakeShrd(Args&&... args);

template <typename T>
    requires std::is_unbounded_array_v<T>
ShrdPtr<T> MakeShrd(std::size_t size);

// Вспомогательные функции
template <typename T>
void swap(ShrdPtr<T>& lhs, ShrdPtr<T>& rhs) noexcept;

template <typename T>
bool operator==(const ShrdPtr<T>& lhs, std::nullptr_t) noexcept;

template <typename T>
bool operator!=(const ShrdPtr<T>& lhs, std::nullptr_t) noexcept;

} // namespace custom

#include "ShrdPtr.tpp"