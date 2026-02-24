import matplotlib.pyplot as plt


n_values = []
time_values = []
with open('results.txt') as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) == 2:
            n, t = parts
            n_values.append(int(n))
            time_values.append(float(t))


plt.plot(n_values, time_values, 'o-', markersize=4)
plt.xlabel('N')
plt.ylabel('Время (секунды)')
plt.title('Зависимость времени выполнения от N')
plt.grid(True)

plt.yscale('log')

plt.show()