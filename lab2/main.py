import matplotlib.pyplot as plt

matrix_sizes = [500, 1000, 1500, 2000, 2500, 3000, 5000]
times_fixed_threads = [
    0.724061,
    2.746054,
    5.521306,
    9.418440,
    14.489353,
    21.930033,
    70.908353,
]


num_threads = [1, 2, 4, 6, 8, 10, 16, 20, 24, 30, 40]
times_fixed_size = [
    52.846762,
    26.196156,
    14.322920,
    10.657271,
    9.360735,
    8.326914,
    6.292618,
    5.498924,
    7.014123,
    6.527596,
    5.590720
]


plt.figure(figsize=(10, 6))

plt.plot(
    matrix_sizes,
    times_fixed_threads,
    marker="o",
    linestyle="-",
    color="b",
    label="Время выполнения",
)

plt.title(
    "Зависимость времени выполнения от размера матрицы при равном количестве итераций(200) и потоков(8)"
)
plt.xlabel("Размер матрицы (строки x столбцы)")
plt.ylabel("Время выполнения (секунды)")
plt.grid(True)
plt.legend()


plt.savefig("time_vs_matrix_size.png", dpi=300)

plt.show()


plt.figure(figsize=(10, 6))

plt.plot(
    num_threads,
    times_fixed_size,
    marker="s",
    linestyle="-",
    color="g",
    label="Время выполнения",
)

plt.title(
    "Зависимость времени выполнения от количества потоков для матрицы 2000x2000 и 200 итераций"
)
plt.xlabel("Количество потоков")
plt.ylabel("Время выполнения (секунды)")
plt.grid(True)
plt.legend()

plt.savefig("time_vs_num_threads.png", dpi=300)

plt.show()
