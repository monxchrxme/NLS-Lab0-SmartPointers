#pragma once

#include <cstddef>
#include "UnqPtr.hpp"
#include "ShrdPtr.hpp"
#include "MsPtr.hpp"

namespace custom {

template <typename T>
class MemorySpan {
private:
    std::size_t size_ = 0;
    UnqPtr<T[]> buffer_; // Управление непрерывным буфером через UnqPtr

public:
    // Конструкторы
    MemorySpan() = default;
    explicit MemorySpan(std::size_t size);
    MemorySpan(std::size_t size, const T& default_val);

    // Запрет копирования буфера (каждый буфер уникален)
    MemorySpan(const MemorySpan&) = delete;
    MemorySpan& operator=(const MemorySpan&) = delete;

    // Перемещение
    MemorySpan(MemorySpan&&) noexcept = default;
    MemorySpan& operator=(MemorySpan&&) noexcept = default;

    // Размер
    std::size_t size() const noexcept;

    // Методы доступа
    UnqPtr<T> Get(std::size_t index) const;     // Передача по значению (копия в UnqPtr)
    ShrdPtr<T> Copy(std::size_t index) const;   // Передача по ссылке (копия в ShrdPtr)
    MsPtr<T> Locate(std::size_t index);         // Указатель с безопасной арифметикой

    // Итераторы начала и конца
    MsPtr<T> begin();
    MsPtr<T> end();
};

} // namespace custom

#include "MemorySpan.tpp"