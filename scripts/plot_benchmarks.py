import os

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# 1. Поиск CSV файла
csv_path = "benchmark_results.csv"
if not os.path.exists(csv_path):
    csv_path = "../benchmark_results.csv"

if not os.path.exists(csv_path):
    print(f"[ERROR] {csv_path} not found! Run ./build/benchmarks first.")
    exit(1)

df = pd.read_csv(csv_path)

plt.style.use('seaborn-v0_8-whitegrid' if 'seaborn-v0_8-whitegrid' in plt.style.available else 'default')
fig, axs = plt.subplots(2, 2, figsize=(16, 12))
plt.subplots_adjust(wspace=0.25, hspace=0.3)

palette = {
    'Raw Pointer': '#2ca02c',           # Зеленый (эталон)
    'custom::UnqPtr': '#1f77b4',        # Синий
    'std::unique_ptr': '#17becf',       # Голубой
    'std::make_shared': '#9467bd',      # Фиолетовый
    'custom::ShrdPtr': '#ff7f0e',       # Оранжевый
    'std::shared_ptr (new)': '#d62728', # Красный
    'custom::SmrtPtr': '#8c564b'        # Коричневый
}

# =============================================================================
# ГРАФИК 1 (Вверху слева): Время выполнения vs N (Log-Log)
# =============================================================================
ax1 = axs[0, 0]
for approach, group in df.groupby('Approach'):
    color = palette.get(approach, '#333333')
    ax1.plot(group['N'], group['Time_ms'], marker='o', linewidth=2.2, label=approach, color=color)

ax1.set_xscale('log')
ax1.set_yscale('log')
ax1.set_title('1. Execution Time Scaling (Log-Log Scale)', fontsize=13, fontweight='bold')
ax1.set_xlabel('Number of Objects (N)', fontsize=11)
ax1.set_ylabel('Total Time (ms)', fontsize=11)
ax1.legend(fontsize=9, loc='upper left')
ax1.grid(True, which="both", ls="--", alpha=0.5)

# =============================================================================
# ГРАФИК 2 (Вверху справа): Пропускная способность (Throughput на 1M объектов)
# =============================================================================
ax2 = axs[0, 1]
df_1m = df[df['N'] == 1000000].copy()
# Считаем миллионы операций в секунду: (N / (Time_ms / 1000)) / 1_000_000 = N / (Time_ms * 1000)
df_1m['Throughput_Mops'] = (df_1m['N'] / (df_1m['Time_ms'] / 1000.0)) / 1e6
df_1m = df_1m.sort_values(by='Throughput_Mops', ascending=True)

colors_2 = [palette.get(app, '#333333') for app in df_1m['Approach']]
bars2 = ax2.barh(df_1m['Approach'], df_1m['Throughput_Mops'], color=colors_2, alpha=0.85)

ax2.set_title('2. Throughput at N = 1,000,000 (Higher is Better)', fontsize=13, fontweight='bold')
ax2.set_xlabel('Throughput (Million ops / second)', fontsize=11)
for bar in bars2:
    w = bar.get_width()
    ax2.text(w + 0.3, bar.get_y() + bar.get_height()/2, f'{w:.1f} M/s', va='center', fontsize=9, fontweight='bold')

# =============================================================================
# ГРАФИК 3 (Внизу слева): Потребление памяти в куче (Heap Memory на 1M)
# =============================================================================
ax3 = axs[1, 0]
df_1m_mem = df[df['N'] == 1000000].sort_values(by='HeapMemory_B', ascending=True)
colors_3 = [palette.get(app, '#333333') for app in df_1m_mem['Approach']]
bars3 = ax3.barh(df_1m_mem['Approach'], df_1m_mem['HeapMemory_B'] / (1024 * 1024), color=colors_3, alpha=0.85)

ax3.set_title('3. Heap Memory Footprint at N = 1,000,000 (Lower is Better)', fontsize=13, fontweight='bold')
ax3.set_xlabel('Heap Memory (Megabytes, MB)', fontsize=11)
for bar in bars3:
    w = bar.get_width()
    ax3.text(w + 0.5, bar.get_y() + bar.get_height()/2, f'{w:.1f} MB', va='center', fontsize=9, fontweight='bold')

# =============================================================================
# ГРАФИК 4 (Внизу справа): Архитектурная цена (Стек vs Кратность аллокаций)
# =============================================================================
ax4 = axs[1, 1]
# Берем данные с 1K масштаба (структура одинакова для всех)
df_types = df[df['N'] == 1000].copy()
approaches = df_types['Approach'].tolist()
stack_sizes = df_types['StackSize_B'].tolist()
alloc_ratios = (df_types['AllocCount'] / df_types['N']).tolist()

x = np.arange(len(approaches))
width = 0.35

ax4.bar(x - width/2, stack_sizes, width, label='Stack Size (Bytes)', color='#34495e', alpha=0.85)
ax4_twin = ax4.twinx()
ax4_twin.bar(x + width/2, alloc_ratios, width, label='Allocs per Object', color='#e74c3c', alpha=0.85)

ax4.set_title('4. Architecture Overhead: Stack Size vs Allocation Ratio', fontsize=13, fontweight='bold')
ax4.set_xticks(x)
ax4.set_xticklabels(approaches, rotation=35, ha='right', fontsize=9)
ax4.set_ylabel('Stack Size (Bytes)', fontsize=11, color='#34495e')
ax4_twin.set_ylabel('Allocations / Object', fontsize=11, color='#e74c3c')
ax4_twin.set_ylim(0, 2.5)
ax4_twin.grid(False) # Отключаем сетку у второй оси, чтобы не двоилось

# Сборка общей легенды для двух шкал
lines4_1, labels4_1 = ax4.get_legend_handles_labels()
lines4_2, labels4_2 = ax4_twin.get_legend_handles_labels()
ax4.legend(lines4_1 + lines4_2, labels4_1 + labels4_2, loc='upper left', fontsize=9)

# 3. Сохранение
plt.tight_layout()
output_img = "benchmark_plots.png"
plt.savefig(output_img, dpi=300)
print(f"[ OK ] Scientific 4-panel dashboard successfully saved to '{output_img}'!")
plt.show()