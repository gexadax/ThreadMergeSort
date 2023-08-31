#include <fstream>
#include <random>
#include <string>
#include "sort.h"
#include "random.h" 

void writeArrayToFile(const int* array, const size_t size, const std::string& filename)
{
    std::ofstream outFile(filename);

    for (size_t i = 0; i < size; ++i)
    {
        if (i)
        {
            outFile << ", ";
        }
        outFile << *(array + i);
    }

    outFile << std::endl;
    outFile.close();
}

int main(int argc, char** argv)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    const size_t size = (argc > 1) ? std::stoull(argv[1]) : ARRAY_SIZE;

    int* randomArray = new int[size];

    generateRandomArray(randomArray, size, RANDOM_LIMIT);


    std::ofstream beforeSortFile("before_sort.txt");
    std::ofstream afterSortFile("after_sort.txt");


    beforeSortFile << "Before sort:" << std::endl;
    writeArrayToFile(randomArray, size, "before_sort.txt");

    mergeSort(randomArray, size);

    afterSortFile << "After sort:" << std::endl;
    writeArrayToFile(randomArray, size, "after_sort.txt");

    delete[] randomArray;

    return 0;
}
