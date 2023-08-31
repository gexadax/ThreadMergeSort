#include <cstdlib>

void generateRandomArray(int* arr, const size_t size, const int limit)
{
    for (size_t i = 0; i < size; ++i)
    {
        arr[i] = std::rand() % limit;
    }
}
