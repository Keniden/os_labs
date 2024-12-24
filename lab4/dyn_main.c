#include <stdio.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <stddef.h>
#include <stdlib.h>

int main()
{
    void *hld = dlopen("./libfirst_realization.so", RTLD_LAZY);
    if (hld == NULL)
    {
        fputs(dlerror(), stderr);
        exit(-1);
    }
    int reliz_numb = 1;
    int com_numb;
    char *error;
    while (true)
    {
        printf("\nВыберите команду. Нажмите: \n- 0 для смены реализации \n- 1 для производной от косинуса\n- 2 для сортировки массива\n");

        if (scanf("%d", &com_numb) == EOF)
        {
            printf("\nПрограмма завершена. До свидания!\n");
            break;
        }

        switch (com_numb)
        {
        case 0:
        {
            if (dlclose(hld) != 0)
            {
                fputs(dlerror(), stderr);
                exit(-1);
            }
            int last_reliz_numb = reliz_numb;
            if (reliz_numb == 1)
            {
                hld = dlopen("./libsecond_realization.so", RTLD_LAZY);
                reliz_numb = 2;
            }
            else
            {
                hld = dlopen("./libfirst_realization.so", RTLD_LAZY);
                reliz_numb = 1;
            }
            if (hld == NULL)
            {
                fputs(dlerror(), stderr);
                exit(-1);
            }
            printf("Реализация была изменена с реализации %d на реализацию %d.\n", last_reliz_numb, reliz_numb);
            break;
        }
        case 1:
        {
            float angle, dx;
            printf("Введите угол в градусах: ");
            scanf("%f", &angle);
            printf("Введите Dx в радианах: ");
            scanf("%f", &dx);

            float (*Derivative)(float, float);
            *(float **)(&Derivative) = dlsym(hld, "Derivative");
            if ((error = dlerror()) != NULL)
            {
                fprintf(stderr, "%s\n", error); 
                exit(1);
            }

            float answ = Derivative(angle, dx); 
            printf("Ответ: %f\n", answ);       
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

            void (*Sort)(int *, int);
            *(void **)(&Sort) = dlsym(hld, "Sort");
            if ((error = dlerror()) != NULL)
            {
                fprintf(stderr, "%s\n", error); 
                exit(1);
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

    if (dlclose(hld) != 0)
    {
        perror("dlclose");
        exit(1);
    }
}
