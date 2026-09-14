#pragma once

#include <cstddef>
#include <stdexcept>

namespace custom {

template <typename T>
class MemorySpan;

template <typename T>
class MsPtr {
private:
    T* current_ = nullptr;
    T* begin_   = nullptr;
    T* end_     = nullptr; // Указывает на позицию за последним элементом [begin_, end_)

    // Только MemorySpan может создавать MsPtr с конкретными границами
    friend class MemorySpan<T>;

    // Приватный конструктор (создание доступно только через MemorySpan::Locate)
    MsPtr(T* current, T* begin, T* end) noexcept;

public:
    using element_type = T;
    using pointer = T*;

    constexpr MsPtr() noexcept = default;

    // Разыменование
    T& operator*() const;
    T* operator->() const;
    T& operator[](std::ptrdiff_t index) const;

    // Инкремент и декремент
    MsPtr& operator++();       // Префиксный ++
    MsPtr operator++(int);     // Постфиксный ++
    MsPtr& operator--();       // Префиксный --
    MsPtr operator--(int);     // Постфиксный --

    // Арифметика со смещением
    MsPtr operator+(std::ptrdiff_t offset) const;
    MsPtr operator-(std::ptrdiff_t offset) const;
    MsPtr& operator+=(std::ptrdiff_t offset);
    MsPtr& operator-=(std::ptrdiff_t offset);

    // Разность двух указателей (возвращает расстояние)
    std::ptrdiff_t operator-(const MsPtr& other) const;

    // Операторы сравнения
    bool operator==(const MsPtr& other) const noexcept;
    bool operator!=(const MsPtr& other) const noexcept;
    bool operator<(const MsPtr& other) const;
    bool operator>(const MsPtr& other) const;
    bool operator<=(const MsPtr& other) const;
    bool operator>=(const MsPtr& other) const;

    // Геттеры
    T* get() const noexcept;
    bool is_end() const noexcept;
};

} // namespace custom

#include "MsPtr.tpp"