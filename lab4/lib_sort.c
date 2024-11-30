#include <stdio.h>

int partition(int *array, int low, int high)
{
    int pivot = array[high];
    int i = low - 1;
    int j, temp;
    for (j = low; j <= high - 1; j++)
    {
        if (array[j] < pivot)
        {
            i++;
            temp = array[i];
            array[i] = array[j];
            array[j] = temp;
        }
    }
    temp = array[i + 1];
    array[i + 1] = array[high];
    array[high] = temp;
    return i + 1;
}

void bubble_sort(int *array, int size)
{
    int i, j, temp;
    for (i = 0; i < size - 1; i++)
    {
        for (j = 0; j < size - i - 1; j++)
        {
            if (array[j] > array[j + 1])
            {
                temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

void quick_sort(int *array, int low, int high)
{
    if (low < high)
    {
        int pivot = partition(array, low, high);
        quick_sort(array, low, pivot - 1);
        quick_sort(array, pivot + 1, high);
    }
}
