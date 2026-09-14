#pragma once

#include <cstddef>
#include <stdexcept>
#include "UnqPtr.hpp"
#include "ShrdPtr.hpp"

namespace custom {

template <typename T>
class DynamicArraySequence {
private:
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
    UnqPtr<T[]> buffer_; 

    // Вспомогательный метод для изменения емкости массива
    void reserve(std::size_t new_capacity);

public:
    using value_type = T;

    // Конструкторы
    DynamicArraySequence();
    explicit DynamicArraySequence(std::size_t count);
    DynamicArraySequence(const T* items, std::size_t count);

    // Копирование (глубокое копирование)
    DynamicArraySequence(const DynamicArraySequence& other);
    DynamicArraySequence& operator=(const DynamicArraySequence& other);

    // Перемещение
    DynamicArraySequence(DynamicArraySequence&& other) noexcept = default;
    DynamicArraySequence& operator=(DynamicArraySequence&& other) noexcept = default;

    // Деструктор (по умолчанию: buffer_ очистится сам)
    ~DynamicArraySequence() = default;

    // Доступ к элементам
    T& GetFirst();
    const T& GetFirst() const;
    T& GetLast();
    const T& GetLast() const;
    T& Get(std::size_t index);
    const T& Get(std::size_t index) const;
    T& operator[](std::size_t index);
    const T& operator[](std::size_t index) const;

    // Получение умных указателей на элементы
    UnqPtr<T> GetUnique(std::size_t index) const;
    ShrdPtr<T> GetShared(std::size_t index) const;

    // Размеры
    std::size_t GetLength() const noexcept;
    std::size_t GetCapacity() const noexcept;
    bool IsEmpty() const noexcept;

    // Модификаторы
    void Append(const T& item);
    void Prepend(const T& item);
    void InsertAt(const T& item, std::size_t index);
};

} // namespace custom

#include "DynamicArraySequence.tpp"