#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <math.h>

typedef float (*derivative_t)(float, float);
typedef void (*bubble_sort_t)(int *, int);
typedef void (*quick_sort_t)(int *, int, int);

int main()
{
    void *lib_handle = NULL;
    derivative_t derivative_1 = NULL, derivative_2 = NULL;
    bubble_sort_t bubble_sort = NULL;
    quick_sort_t quick_sort = NULL;
    char *error;
    int choice;

    while (1)
    {
        printf("Введите: 1 для расчета производной, 2 для сортировки массива, 0 для смены библиотеки: ");
        scanf("%d", &choice);

        if (choice == 0)
        {
            // Смена библиотеки
            printf("Смена реализации: ");
            char lib[20];
            scanf("%s", lib);

            if (lib_handle)
            {
                dlclose(lib_handle);
            }

            lib_handle = dlopen(lib, RTLD_LAZY);
            if (!lib_handle)
            {
                fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
                return 1;
            }

            // Загрузка функций из библиотеки
            derivative_1 = dlsym(lib_handle, "derivative_1");
            derivative_2 = dlsym(lib_handle, "derivative_2");
            bubble_sort = dlsym(lib_handle, "bubble_sort");
            quick_sort = dlsym(lib_handle, "quick_sort");

            if ((error = dlerror()) != NULL)
            {
                fprintf(stderr, "%s\n", error);
                return 1;
            }

            // Проверка на инициализацию функций
            if (!derivative_1 || !derivative_2 || !bubble_sort || !quick_sort)
            {
                fprintf(stderr, "Ошибка инициализации функций\n");
                return 1;
            }

            printf("Библиотека загружена: %s\n", lib);
        }
        else if (choice == 1 || choice == 2)
        {
            if (choice == 1)
            {
                // Вычисление производной
                float A, deltaX;
                printf("Введите A и deltaX для производной:\n");
                scanf("%f %f", &A, &deltaX);
                if (derivative_1 && derivative_2)
                {
                    printf("Результат производной 1: %.6f\n", derivative_1(A, deltaX));
                    printf("Результат производной 2: %.6f\n", derivative_2(A, deltaX));
                }
            }
            else if (choice == 2)
            {
                // Сортировка массива
                int n, i;
                printf("Введите размер массива:\n");
                scanf("%d", &n);
                int *array = malloc(n * sizeof(int));

                printf("Введите элементы массива:\n");
                for (i = 0; i < n; i++)
                {
                    scanf("%d", &array[i]);
                }

                printf("1: Пузырьковая сортировка\n");
                printf("2: Быстрая сортировка\n");
                scanf("%d", &choice);

                if (bubble_sort && quick_sort)
                {
                    if (choice == 1)
                    {
                        bubble_sort(array, n); // Пузырьковая сортировка
                    }
                    else if (choice == 2)
                    {
                        quick_sort(array, 0, n - 1); // Быстрая сортировка
                    }

                    // Вывод отсортированного массива
                    printf("Отсортированный массив:\n");
                    for (i = 0; i < n; i++)
                    {
                        printf("%d ", array[i]);
                    }
                    printf("\n");
                }

                free(array);
            }
        }
    }

    if (lib_handle)
    {
        dlclose(lib_handle);
    }
    return 0;
}
