#pragma once

#include <cstddef>
#include <unordered_map>

namespace custom {

class CentralStorage {
private:
    // Хранилище: Адрес объекта в памяти -> Количество активных ссылок
    std::unordered_map<const void*, std::size_t> table_;

    // Приватный конструктор для паттерна Singleton
    CentralStorage() = default;

public:
    CentralStorage(const CentralStorage&) = delete;
    CentralStorage& operator=(const CentralStorage&) = delete;

    // Точка доступа к синглтону
    static CentralStorage& instance();

    // Увеличить счетчик ссылок на адрес ptr
    void add_ref(const void* ptr);

    // Уменьшить счетчик ссылок
    // Возвращает true, если счетчик стал равен 0 (объект пора удалять через delete)
    bool release_ref(const void* ptr);

    // Узнать текущее число ссылок на объект
    std::size_t get_ref_count(const void* ptr) const;

    // Сколько всего объектов сейчас отслеживается реестром (нужно для проверки утечек)
    std::size_t tracked_objects_count() const;

    // Очистить реестр
    void clear();
};

} // namespace custom