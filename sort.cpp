#include "sort.h"
#include <algorithm>
#include <future>

const size_t THRESHOLD = 10000;

void merge(int* left, int* middle, int* right);

void mergeSort(int* array, const size_t size)
{
    mergeSort(array, array + size);
}

void mergeSort(int* begin, int* end)
{
    if (end - 1 <= begin)
    {
        return;
    }

    int* middle = begin + (end - begin) / 2;

    if (end - begin >= THRESHOLD)
    {
        auto futureLeft = std::async(std::launch::async, [begin, middle]() { mergeSort(begin, middle); });
        mergeSort(middle, end);
        futureLeft.get();
    }
    else
    {
        mergeSort(begin, middle);
        mergeSort(middle, end);
    }

    merge(begin, middle, end);
}

void merge(int* left, int* middle, int* right)
{
    const size_t size = right - left;

    int* temp = new int[size];

    int* first = left, * second = middle;
    size_t i = 0;

    while (first < middle && second < right)
    {
        if (*first < *second)
        {
            *(temp + i) = *first;
            ++first;
        }
        else
        {
            *(temp + i) = *second;
            ++second;
        }
        ++i;
    }

    while (first < middle)
    {
        *(temp + i) = *first;
        ++first;
        ++i;
    }

    while (second < right)
    {
        *(temp + i) = *second;
        ++second;
        ++i;
    }

    for (size_t i = 0; i < size; ++i)
    {
        *(left + i) = *(temp + i);
    }

    delete[] temp;
}
