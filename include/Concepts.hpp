#pragma once

#include <concepts>
#include <type_traits>

namespace custom {

/**
 * @brief Концепт проверяет, что указатель типа From* может быть неявно
 *        преобразован в указатель типа To*.
 * 
 * Покрывает:
 *  - Приведение Derived* к Base* (Upcasting);
 *  - Добавление const/volatile (T* к const T*);
 *  - Блокирует несвязанные типы на этапе компиляции с читаемой ошибкой.
 */
template <typename From, typename To>
concept ConvertibleToPointer = std::convertible_to<From*, To*>;

/**
 * @brief Концепт, строго требующий прямого или косвенного наследования
 *        (Derived унаследован от Base).
 */
template <typename Derived, typename Base>
concept DerivedFrom = std::derived_from<Derived, Base>;

} // namespace custom