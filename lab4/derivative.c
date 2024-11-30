#include <stdio.h>
#include <math.h>

float derivative_1(float A, float deltaX)
{
    return (cos(A + deltaX) - cos(A)) / deltaX;
}

float derivative_2(float A, float deltaX)
{
    return (cos(A + deltaX) - cos(A - deltaX)) / (2 * deltaX);
}

