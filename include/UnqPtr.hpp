#pragma once

#include <cstddef>
#include <utility>
#include <type_traits>
#include "Concepts.hpp"

namespace custom {

// Forward Declaration
template <typename T>
class UnqPtr;

// ============================================================================
// Основной шаблон UnqPtr<T> (Одиночные объекты)
// ============================================================================
template <typename T>
class UnqPtr {
private:
    T* ptr_ = nullptr;

    // Дружественный доступ (для Derived -> Base)
    template <typename U>
    friend class UnqPtr;

public:
    using element_type = T;
    using pointer = T*;

    // Конструкторы
    constexpr UnqPtr() noexcept = default;
    constexpr UnqPtr(std::nullptr_t) noexcept : ptr_(nullptr) {}
    explicit UnqPtr(T* ptr) noexcept;

    // Деструктор
    ~UnqPtr();

    // Запрет копирования
    UnqPtr(const UnqPtr&) = delete;
    UnqPtr& operator=(const UnqPtr&) = delete;

    // Конструктор и оператор перемещения (Move semantics)
    UnqPtr(UnqPtr&& other) noexcept;
    UnqPtr& operator=(UnqPtr&& other) noexcept;

    // Конструкторы перемещения с подтипизацией (Upcasting: Derived -> Base)
    template <typename U>
        requires ConvertibleToPointer<U, T>
    UnqPtr(UnqPtr<U>&& other) noexcept;

    template <typename U>
        requires ConvertibleToPointer<U, T>
    UnqPtr& operator=(UnqPtr<U>&& other) noexcept;

    // Модификаторы
    T* release() noexcept;
    void reset(T* new_ptr = nullptr) noexcept;
    void swap(UnqPtr& other) noexcept;

    T* get() const noexcept;
    explicit operator bool() const noexcept;
    T& operator*() const;
    T* operator->() const noexcept;
};

// ============================================================================
// Частичная специализация UnqPtr<T[]> (Динамические массивы)
// ============================================================================
template <typename T>
class UnqPtr<T[]> {
private:
    T* ptr_ = nullptr;

public:
    using element_type = T;
    using pointer = T*;

    constexpr UnqPtr() noexcept = default;
    constexpr UnqPtr(std::nullptr_t) noexcept : ptr_(nullptr) {}
    explicit UnqPtr(T* ptr) noexcept;

    ~UnqPtr();

    UnqPtr(const UnqPtr&) = delete;
    UnqPtr& operator=(const UnqPtr&) = delete;

    UnqPtr(UnqPtr&& other) noexcept;
    UnqPtr& operator=(UnqPtr&& other) noexcept;

    T* release() noexcept;
    void reset(T* new_ptr = nullptr) noexcept;
    void swap(UnqPtr& other) noexcept;

    T* get() const noexcept;
    explicit operator bool() const noexcept;

    // Для массивов предоставляется доступ по индексу вместо * и ->
    T& operator[](std::size_t index) const;
};

// Фабричная функция MakeUnq (аналог std::make_unique)
template <typename T, typename... Args>
    requires (!std::is_array_v<T>)
UnqPtr<T> MakeUnq(Args&&... args);

template <typename T>
    requires std::is_unbounded_array_v<T>
UnqPtr<T> MakeUnq(std::size_t size);

// Неэлементная функция swap
template <typename T>
void swap(UnqPtr<T>& lhs, UnqPtr<T>& rhs) noexcept;

} // namespace custom

#include "UnqPtr.tpp"