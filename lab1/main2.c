#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <float.h>

// Компиляция: gcc -o morph_filter main.c -lpthread
// Запуск: ./morph_filter 5 5 200 2

// Структура для представления матрицы
typedef struct
{
    int rows;     // Количество строк
    int cols;     // Количество столбцов
    float **data; // Двумерный массив данных матрицы
} matrix_t;

// Структура для передачи аргументов в поток
typedef struct
{
    matrix_t *input;  // Указатель на входную матрицу
    matrix_t *output; // Указатель на выходную матрицу
    int start_row;    // Начальная строка для обработки
    int end_row;      // Конечная строка для обработки
} thread_args_t;

// Семфор для ограничения числа одновременно работающих потоков
sem_t semaphore;

// Функция для инициализации матрицы случайными числами
matrix_t *initialize_matrix(int rows, int cols)
{
    matrix_t *mat = malloc(sizeof(matrix_t));   // Выделение памяти под структуру матрицы
    mat->rows = rows;                           // Установка количества строк
    mat->cols = cols;                           // Установка количества столбцов
    mat->data = malloc(rows * sizeof(float *)); // Выделение памяти под строки матрицы
    for (int i = 0; i < rows; i++)
    {
        mat->data[i] = malloc(cols * sizeof(float)); // Выделение памяти под каждый столбец
        for (int j = 0; j < cols; j++)
        {
            mat->data[i][j] = ((float)rand()) / RAND_MAX * 100.0f; // Заполнение случайными значениями от 0 до 100
        }
    }
    return mat; // Возврат указателя на инициализированную матрицу
}

// Функция для освобождения памяти матрицы
void free_matrix(matrix_t *mat)
{
    for (int i = 0; i < mat->rows; i++)
    {
        free(mat->data[i]); // Освобождение памяти каждой строки
    }
    free(mat->data); // Освобождение памяти массива строк
    free(mat);       // Освобождение памяти самой структуры матрицы
}

// Функция для вывода матрицы на экран
void print_matrix(matrix_t *mat)
{
    for (int i = 0; i < mat->rows; i++) // Проход по каждой строке
    {
        for (int j = 0; j < mat->cols; j++) // Проход по каждому столбцу
        {
            printf("%.2f ", mat->data[i][j]); // Печать значения элемента с двумя знаками после запятой
        }
        printf("\n"); // Перевод строки после каждой строки матрицы
    }
    printf("\n"); // Дополнительный перевод строки после всей матрицы
}

// Функция для выполнения операции эрозии на одном элементе
float min_neighbor(matrix_t *input, int i, int j)
{
    float min_val = FLT_MAX;         // Инициализация минимального значения максимально возможным
    for (int di = -1; di <= 1; di++) // Проход по соседям по вертикали
    {
        for (int dj = -1; dj <= 1; dj++) // Проход по соседям по горизонтали
        {
            int ni = i + di;                                                // Вычисление индекса соседней строки
            int nj = j + dj;                                                // Вычисление индекса соседнего столбца
            if (ni >= 0 && ni < input->rows && nj >= 0 && nj < input->cols) // Проверка границ матрицы
            {
                if (input->data[ni][nj] < min_val) // Поиск минимального значения среди соседей
                    min_val = input->data[ni][nj];
            }
        }
    }
    return min_val; // Возврат минимального значения
}

// Функция для выполнения операции наращивания на одном элементе
float max_neighbor(matrix_t *input, int i, int j)
{
    float max_val = -FLT_MAX;        // Инициализация максимального значения минимально возможным
    for (int di = -1; di <= 1; di++) // Проход по соседям по вертикали
    {
        for (int dj = -1; dj <= 1; dj++) // Проход по соседям по горизонтали
        {
            int ni = i + di;                                                // Вычисление индекса соседней строки
            int nj = j + dj;                                                // Вычисление индекса соседнего столбца
            if (ni >= 0 && ni < input->rows && nj >= 0 && nj < input->cols) // Проверка границ матрицы
            {
                if (input->data[ni][nj] > max_val) // Поиск максимального значения среди соседей
                    max_val = input->data[ni][nj];
            }
        }
    }
    return max_val; // Возврат максимального значения
}

// Функция, выполняемая потоком для эрозии
void *thread_erosion(void *args)
{
    thread_args_t *targs = (thread_args_t *)args;           // Приведение аргументов к нужному типу
    for (int i = targs->start_row; i < targs->end_row; i++) // Обработка назначенных строк
    {
        for (int j = 0; j < targs->input->cols; j++) // Обработка всех столбцов в строке
        {
            targs->output->data[i][j] = min_neighbor(targs->input, i, j); // Применение операции эрозии
        }
    }
    sem_post(&semaphore); // Освобождение места в семафоре после завершения работы
    pthread_exit(NULL);   // Завершение потока
}

// Функция, выполняемая потоком для наращивания
void *thread_dilation(void *args)
{
    thread_args_t *targs = (thread_args_t *)args;           // Приведение аргументов к нужному типу
    for (int i = targs->start_row; i < targs->end_row; i++) // Обработка назначенных строк
    {
        for (int j = 0; j < targs->input->cols; j++) // Обработка всех столбцов в строке
        {
            targs->output->data[i][j] = max_neighbor(targs->input, i, j); // Применение операции наращивания
        }
    }
    sem_post(&semaphore); // Освобождение места в семафоре после завершения работы
    pthread_exit(NULL);   // Завершение потока
}

int main(int argc, char *argv[])
{
    if (argc != 5) // Проверка правильности количества аргументов
    {
        printf("Usage: %s <rows> <cols> <K> <max_threads>\n", argv[0]); // Инструкция по использованию
        return 1;                                                       // Завершение программы с ошибкой
    }

    int rows = atoi(argv[1]);        // Чтение количества строк из аргументов
    int cols = atoi(argv[2]);        // Чтение количества столбцов из аргументов
    int K = atoi(argv[3]);           // Чтение количества итераций из аргументов
    int max_threads = atoi(argv[4]); // Чтение максимального числа потоков из аргументов

    if (rows <= 0 || cols <= 0 || K <= 0 || max_threads <= 0) // Проверка, что все аргументы положительные
    {
        printf("All arguments must be positive integers.\n"); // Сообщение об ошибке
        return 1;                                             // Завершение программы с ошибкой
    }

    srand(time(NULL)); // Инициализация генератора случайных чисел

    // Инициализация матриц
    matrix_t *A = initialize_matrix(rows, cols); // Исходная матрица A
    matrix_t *B = initialize_matrix(rows, cols); // Временная матрица B для эрозии
    matrix_t *C = initialize_matrix(rows, cols); // Временная матрица C для наращивания

    // printf("Initial Matrix A:\n");
    // print_matrix(A);                                    // Вывод исходной матрицы (закомментировано)

    // Инициализация семафора с начальным значением max_threads
    sem_init(&semaphore, 0, max_threads);

    struct timespec start_time, end_time;        // Структуры для измерения времени
    clock_gettime(CLOCK_MONOTONIC, &start_time); // Запись времени начала выполнения

    for (int k = 0; k < K; k++) // Цикл по количеству итераций
    {
        // Эрозия: A -> B
        // Разделим строки на части для потоков
        int num_threads = max_threads;                                // Количество потоков
        int rows_per_thread = (rows + num_threads - 1) / num_threads; // Расчет строк на поток
        pthread_t threads[num_threads];                               // Массив идентификаторов потоков
        thread_args_t targs[num_threads];                             // Массив структур с аргументами для потоков

        for (int t = 0; t < num_threads; t++) // Создание потоков для эрозии
        {
            int start_row = t * rows_per_thread;     // Начальная строка для текущего потока
            int end_row = (t + 1) * rows_per_thread; // Конечная строка для текущего потока
            if (end_row > rows)
                end_row = rows; // Корректировка, если выход за пределы матрицы
            if (start_row >= rows)
                break; // Прерывание, если все строки уже распределены

            targs[t].input = A;             // Установка входной матрицы
            targs[t].output = B;            // Установка выходной матрицы
            targs[t].start_row = start_row; // Установка начальной строки
            targs[t].end_row = end_row;     // Установка конечной строки

            sem_wait(&semaphore);                                                  // Ожидание, если достигнут максимум потоков
            if (pthread_create(&threads[t], NULL, thread_erosion, &targs[t]) != 0) // Создание потока
            {
                perror("Failed to create thread"); // Обработка ошибки создания потока
                return 1;                          // Завершение программы с ошибкой
            }
        }

        // Ожидание завершения всех потоков эрозии
        for (int t = 0; t < num_threads; t++)
        {
            if (targs[t].start_row < rows) // Проверка, был ли поток создан для обработки строк
            {
                pthread_join(threads[t], NULL); // Ожидание завершения потока
            }
        }

        // Наращивание: B -> C
        for (int t = 0; t < num_threads; t++) // Создание потоков для наращивания
        {
            int start_row = t * rows_per_thread;     // Начальная строка для текущего потока
            int end_row = (t + 1) * rows_per_thread; // Конечная строка для текущего потока
            if (end_row > rows)
                end_row = rows; // Корректировка, если выход за пределы матрицы
            if (start_row >= rows)
                break; // Прерывание, если все строки уже распределены

            targs[t].input = B;             // Установка входной матрицы
            targs[t].output = C;            // Установка выходной матрицы
            targs[t].start_row = start_row; // Установка начальной строки
            targs[t].end_row = end_row;     // Установка конечной строки

            sem_wait(&semaphore);                                                   // Ожидание, если достигнут максимум потоков
            if (pthread_create(&threads[t], NULL, thread_dilation, &targs[t]) != 0) // Создание потока
            {
                perror("Failed to create thread"); // Обработка ошибки создания потока
                return 1;                          // Завершение программы с ошибкой
            }
        }

        // Ожидание завершения всех потоков наращивания
        for (int t = 0; t < num_threads; t++)
        {
            if (targs[t].start_row < rows) // Проверка, был ли поток создан для обработки строк
            {
                pthread_join(threads[t], NULL); // Ожидание завершения потока
            }
        }

        // Перекопировать результат для следующей итерации
        matrix_t *temp = A; // Временное хранение указателя на A
        A = C;              // A теперь указывает на результат наращивания
        C = temp;           // C указывает на предыдущую матрицу A для последующей обработки
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time); // Запись времени окончания выполнения

    // Расчет времени выполнения
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec);   // Разница в секундах
    elapsed_time += (end_time.tv_nsec - start_time.tv_nsec) / 1e9; // Добавление разницы в наносекундах

    // Вывод результатов
    printf("Эрозия и наращивание выполнены %d итераций.\n", K); // Вывод количества итераций
    printf("Время выполнения: %.6f секунд\n", elapsed_time);    // Вывод времени выполнения
    printf("Число потоков: %d \n", max_threads);                // Вывод числа потоков

    // Вывод итоговых матриц (закомментировано)
    // printf("Resulting Matrix after Erosion (A):\n");
    // print_matrix(A);
    // printf("Resulting Matrix after Dilation (C):\n");
    // print_matrix(C);

    // Освобождение памяти
    free_matrix(A); // Освобождение матрицы A
    free_matrix(B); // Освобождение матрицы B
    free_matrix(C); // Освобождение матрицы C

    sem_destroy(&semaphore); // Уничтожение семафора

    return 0; // Завершение программы успешно
}
