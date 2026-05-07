import subprocess
import time
import random
import string
import matplotlib.pyplot as plt

EXECUTABLE = "./algo"
SIZES = [1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000]
ITERATIONS = 3

def generate_data(size):
    chars = string.ascii_letters + string.digits
    line1 = ''.join(random.choice(chars) for _ in range(size))
    line2 = ''.join(random.choice(chars) for _ in range(size))
    return f"{line1}\n{line2}\n1 1 1 1\n"

def run_benchmark():
    results = []
    for size in SIZES:
        total_time = 0
        data = generate_data(size)
        for _ in range(ITERATIONS):
            start = time.perf_counter()
            process = subprocess.Popen(
                [EXECUTABLE],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            process.communicate(input=data)
            end = time.perf_counter()
            total_time += (end - start)
        avg_time = total_time / ITERATIONS
        results.append(avg_time)
        print(f"{size:10d} | {avg_time:12.5f}")
    return results

def plot_results(sizes, times):
    plt.figure(figsize=(10, 6))
    plt.plot(sizes, times, marker='o', linestyle='-', color='royalblue', linewidth=2)
    plt.xlabel('Длина строки', fontsize=12)
    plt.ylabel('Время выполнения (сек)', fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.savefig('bench_result.png')

if __name__ == "__main__":
    try:
        execution_times = run_benchmark()
        plot_results(SIZES, execution_times)
    except FileNotFoundError:
        print(f"Ошибка: Файл {EXECUTABLE} не найден. Сначала скомпилируйте его через make.")
