#include <stdio.h>
#include "function.h"
#include <stdbool.h>

int main()
{
    int com_numb;

    while (true)
    {
        printf("\nВыберите команду. Нажмите: \n- 1 для производной от косинуса\n- 2 для сортировки массива\n");

        if (scanf("%d", &com_numb) == EOF)
        {
            printf("\nПрограмма завершена. До свидания!\n");
            break;
        }

        switch (com_numb)
        {
        case 1:
        {
            float angle, dx;
            printf("Введите угол в градусах: ");
            scanf("%f", &angle);
            printf("Введите Dx в радианах: ");
            scanf("%f", &dx);
            float answ = Derivative(angle, dx);
            printf("Ответ: %f\n", answ);
            break;
        }
        case 2:
        {
            int a_size;
            printf("Введите размер массива: ");
            scanf("%d", &a_size);
            int array[a_size];
            printf("\nВведите элементы массива: ");
            for (int i = 0; i < a_size; ++i)
            {
                int tmp;
                scanf("%d", &tmp);
                array[i] = tmp;
            }
            Sort(array, a_size);
            printf("Массив был отсортирован: ");
            for (int i = 0; i < a_size; ++i)
            {
                printf("% d", array[i]);
            }
            break;
        }
        default:
            printf("Некорректный ввод. Попробуйте снова\n");
            break;
        }
    }
}
