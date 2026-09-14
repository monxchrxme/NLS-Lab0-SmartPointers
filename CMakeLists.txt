cmake_minimum_required(VERSION 3.20)
project(SmartPointersLab CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build" FORCE)
endif()

# Флаги компилятора
add_compile_options(
    -Wall 
    -Wextra 
    -Wpedantic 
)

# =========================================================================
# Конфигурация санитайзеров (ASan + UBSan)
# Включается ТОЛЬКО для профиля Debug
# Для Valgrind собираем в RelWithDebInfo, где санитайзеры отключены
# =========================================================================

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS ">>> Build type: DEBUG (ASan & UBSan enabled for all targets)")
    # -fno-omit-frame-pointer нужен для идеального стектрейса в отчетах ASan
    add_compile_options(-fsanitize=address,undefined -g -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
else()
    message(STATUS ">>> Build type: ${CMAKE_BUILD_TYPE} (Sanitizers disabled)")
endif()


# Папка с заголовочными файлами
include_directories(include)

# =========================================================================
# Подключение Google Test через CMake FetchContent
# =========================================================================
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Для совместимости на всех платформах
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Основная программа
add_executable(main
    src/CentralStorage.cpp
    main.cpp
)

# Модульные тесты на Google Test
enable_testing()
add_executable(unit_tests 
    tests/test_main.cpp
    src/CentralStorage.cpp
)
target_link_libraries(unit_tests PRIVATE gtest_main)
add_test(NAME AllUnitTests COMMAND unit_tests)


# Модуль нагрузочных тестов (бенчмарки)
add_executable(benchmarks 
    benchmarks/bench_main.cpp
    src/CentralStorage.cpp
)