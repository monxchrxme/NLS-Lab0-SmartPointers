#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <memory>
#include <string>

#include "UnqPtr.hpp"
#include "ShrdPtr.hpp"
#include "SmrtPtr.hpp"
#include "MsPtr.hpp"

// =============================================================================
// Барьер оптимизации (аналог benchmark::DoNotOptimize из Google Benchmark)
// Запрещает компилятору вырезать аллокации через Dead Code Elimination
// =============================================================================
template <typename T>
inline void do_not_optimize(T* p) {
#if defined(__GNUC__) || defined(__clang__)
    asm volatile("" : : "g"(p) : "memory");
#endif
}

// =============================================================================
// Функция прогрева памяти
// =============================================================================
void warmup_allocator(size_t warmup_count = 100'000) {
    std::cout << "[INFO] Warming up memory allocator & CPU caches (" 
              << warmup_count << " dummy allocations)...\n";
    for (size_t i = 0; i < warmup_count; ++i) {
        int* p = new int(static_cast<int>(i));
        do_not_optimize(p);
        delete p;
    }
    std::cout << "[INFO] Warmup complete. Starting benchmarks with hot cache.\n\n";
}

struct BenchmarkResult {
    std::string scale_name;
    size_t n;
    std::string approach;
    size_t stack_size;
    size_t alloc_count;
    size_t heap_memory;
    double time_us;
    double time_ms;
};

template <typename Func>
double measure_time_us(Func&& func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> elapsed = end - start;
    return elapsed.count();
}

void print_table_header(const std::string& title, size_t n) {
    std::cout << "========================================================================================\n";
    std::cout << " BENCHMARK: " << title << " (N = " << n << " objects)\n";
    std::cout << "========================================================================================\n";
    std::cout << std::left 
              << std::setw(24) << "Approach"
              << std::right 
              << std::setw(12) << "Stack Size"
              << std::setw(14) << "Alloc Count"
              << std::setw(16) << "Heap Memory"
              << std::setw(12) << "Time (us)"
              << std::setw(12) << "Time (ms)"
              << "\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
}

void print_row(const BenchmarkResult& r) {
    std::cout << std::left  << std::setw(24) << r.approach
              << std::right 
              << std::setw(10) << r.stack_size << " B"
              << std::setw(14) << r.alloc_count
              << std::setw(14) << r.heap_memory << " B"
              << std::setw(12) << std::fixed << std::setprecision(1) << r.time_us
              << std::setw(12) << std::fixed << std::setprecision(3) << r.time_ms
              << "\n";
}

int main() {
    warmup_allocator();

    std::vector<BenchmarkResult> all_results;

    const std::vector<std::pair<std::string, size_t>> test_scales = {
        {"Small Scale", 1000},
        {"Medium Scale", 100000},
        {"Large Scale", 1000000}
    };

    for (const auto& [scale_title, n] : test_scales) {
        print_table_header(scale_title, n);

        // 1. Raw Pointer
        {
            double us = measure_time_us([n]() {
                for (size_t i = 0; i < n; ++i) {
                    int* p = new int(static_cast<int>(i));
                    do_not_optimize(p);
                    delete p;
                }
            });
            BenchmarkResult r{
                scale_title, n, "Raw Pointer",
                sizeof(int*),
                n,
                n * sizeof(int),
                us, us / 1000.0
            };
            all_results.push_back(r);
            print_row(r);
        }

        // 2. custom::UnqPtr
        {
            double us = measure_time_us([n]() {
                for (size_t i = 0; i < n; ++i) {
                    auto p = custom::MakeUnq<int>(static_cast<int>(i));
                    do_not_optimize(p.get());
                }
            });
            BenchmarkResult r{
                scale_title, n, "custom::UnqPtr",
                sizeof(custom::UnqPtr<int>),
                n,
                n * sizeof(int),
                us, us / 1000.0
            };
            all_results.push_back(r);
            print_row(r);
        }

        // 3. std::unique_ptr
        {
            double us = measure_time_us([n]() {
                for (size_t i = 0; i < n; ++i) {
                    auto p = std::make_unique<int>(static_cast<int>(i));
                    do_not_optimize(p.get());
                }
            });
            BenchmarkResult r{
                scale_title, n, "std::unique_ptr",
                sizeof(std::unique_ptr<int>),
                n,
                n * sizeof(int),
                us, us / 1000.0
            };
            all_results.push_back(r);
            print_row(r);
        }

        // 4. custom::ShrdPtr
        {
            double us = measure_time_us([n]() {
                for (size_t i = 0; i < n; ++i) {
                    auto p1 = custom::MakeShrd<int>(static_cast<int>(i));
                    do_not_optimize(p1.get());
                }
            });
            BenchmarkResult r{
                scale_title, n, "custom::ShrdPtr",
                sizeof(custom::ShrdPtr<int>),
                n * 2,
                n * (sizeof(int) + sizeof(size_t)),
                us, us / 1000.0
            };
            all_results.push_back(r);
            print_row(r);
        }

        // 5. std::shared_ptr (new int)
        {
            double us = measure_time_us([n]() {
                for (size_t i = 0; i < n; ++i) {
                    std::shared_ptr<int> p(new int(static_cast<int>(i)));
                    do_not_optimize(p.get());
                }
            });
            BenchmarkResult r{
                scale_title, n, "std::shared_ptr",
                sizeof(std::shared_ptr<int>),
                n * 2,
                n * (sizeof(int) + 24),
                us, us / 1000.0
            };
            all_results.push_back(r);
            print_row(r);
        }

        // 6. std::make_shared
        {
            double us = measure_time_us([n]() {
                for (size_t i = 0; i < n; ++i) {
                    auto p = std::make_shared<int>(static_cast<int>(i));
                    do_not_optimize(p.get());
                }
            });
            BenchmarkResult r{
                scale_title, n, "std::make_shared",
                sizeof(std::shared_ptr<int>),
                n,
                n * (sizeof(int) + 20),
                us, us / 1000.0
            };
            all_results.push_back(r);
            print_row(r);
        }

        // 7. custom::SmrtPtr
        {
            double us = measure_time_us([n]() {
                for (size_t i = 0; i < n; ++i) {
                    auto p = custom::MakeSmrt<int>(static_cast<int>(i));
                    do_not_optimize(p.get());
                }
            });
            BenchmarkResult r{
                scale_title, n, "custom::SmrtPtr",
                sizeof(custom::SmrtPtr<int>),
                n * 2,
                n * (sizeof(int) + 24),
                us, us / 1000.0
            };
            all_results.push_back(r);
            print_row(r);
        }

        std::cout << "========================================================================================\n\n";
    }

    // Сохранение в CSV
    const std::string filename = "benchmark_results.csv";
    std::ofstream csv(filename);
    if (csv.is_open()) {
        csv << "Scale,N,Approach,StackSize_B,AllocCount,HeapMemory_B,Time_us,Time_ms\n";
        for (const auto& r : all_results) {
            csv << r.scale_name << ","
                << r.n << ","
                << r.approach << ","
                << r.stack_size << ","
                << r.alloc_count << ","
                << r.heap_memory << ","
                << std::fixed << std::setprecision(2) << r.time_us << ","
                << std::fixed << std::setprecision(4) << r.time_ms << "\n";
        }
        std::cout << "[ OK ] Detailed benchmark results saved to '" << filename << "'!\n";
    }

    return 0;
}