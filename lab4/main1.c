#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "derivative.h"
#include "lib_sort.h"

int main()
{
    int choice;
    printf("Введите: 1 для расчета производной, 2 для сортировки массива: ");
    scanf("%d", &choice);

    if (choice == 1)
    {
        float A, deltaX;
        printf("Введите A и deltaX для производной:\n");
        scanf("%f %f", &A, &deltaX);

        float result1 = derivative_1(A, deltaX);
        printf("Результат производной 1: %f\n", result1);

        float result2 = derivative_2(A, deltaX);
        printf("Результат производной 2: %f\n", result2);
    }
    else if (choice == 2)
    {
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

        if (choice == 1)
        {
            bubble_sort(array, n);
        }
        else if (choice == 2)
        {
            quick_sort(array, 0, n - 1);
        }

        printf("Отсортированный массив:\n");
        for (i = 0; i < n; i++)
        {
            printf("%d ", array[i]);
        }
        printf("\n");

        free(array);
    }
    return 0;
}
