#include <iostream>
#include <concepts>

template <typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template <Numeric T>
void print_number(T val) {
    std::cout << "Number: " << val << " (C++20 Concepts work!)\n";
}

int main() {
    print_number(42);
    print_number(3.14);

    return 0;
}