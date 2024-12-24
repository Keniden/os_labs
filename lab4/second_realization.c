#include <stdio.h>
#include "function.h"
#include <math.h>


float Derivative(float a, float dx)
{
    float angle_rad = (a * PI) / 180.0f;
    return (cosf(angle_rad + dx) - cosf(angle_rad - dx)) / (2 * dx);
}

void min_max(int a, int b, int c, int *mn, int *mx)
{
    *mn = fminf(fminf(a, b), c);
    *mx = fmaxf(fmaxf(a, b), c);
}

void swap(int *array, int i1, int i2)
{
    if (i1 != i2)
    {
        int tmp = array[i1];
        array[i1] = array[i2];
        array[i2] = tmp;
    }
}

int find_pivot(int *array, int size)
{
    int a = array[0];
    int b = array[size - 1];
    int c = array[size / 2];
    int mn, mx;
    min_max(a, b, c, &mn, &mx);
    return a + b + c - mx - mn;
}

void partition(int *array, int *kf_i, int *kf_k, int size)
{
    int pivot = find_pivot(array, size);
    int i = 0, j = 0, k = 0;

    while (j < size)
    {
        if (array[j] < pivot)
        {
            swap(array, i++, j++);
            swap(array, k++, j);
        }
        else if (array[j] == pivot)
        {
            swap(array, k++, j++);
        }
        else
        {
            j++;
        }
    }

    *kf_k = k;
    *kf_i = i;
}

void quicksort(int *array, int size)
{
    if (size < 2)
        return;

    int i, k;
    partition(array, &i, &k, size);

    quicksort(array, i);
    quicksort(&array[k], size - k);
}

void Sort(int *array, int size)
{
    quicksort(array, size);
}
