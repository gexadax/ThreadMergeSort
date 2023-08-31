#include <iostream>
#include <fstream>
//#include <algorithm>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <future>

using namespace std;

const string FILE_NAME = "numbers.txt";
const int CHUNK_SIZE = 1000000; // размер фрагмента файла
const int MAX_CHUNKS = 100; // максимальное количество фрагментов
const string TEMP_FILE_PREFIX = "temp_chunk_"; // префикс имени временного файла
const int MAX_NUM = 1000000; // максимальное значение числа
const int NUM_THREADS = 4; // количество запускаемых потоков

// функция для генерации случайного числа
int random(int min, int max) {
    return min + rand() % (max - min + 1);
}

// функция для генерации файла с числами
void generateFile(int numCount) {
    ofstream out(FILE_NAME);
    srand(time(nullptr));
    for (int i = 0; i < numCount; i++) {
        out << random(0, MAX_NUM) << endl;
    }
    out.close();
}

// функция для чтения чисел из файла
vector<int> readChunk(ifstream& in) {
    vector<int> chunk;
    chunk.reserve(CHUNK_SIZE);
    int num;
    while (chunk.size() < CHUNK_SIZE && in >> num) {
        chunk.push_back(num);
    }
    return chunk;
}

// функция для записи чисел в файл
void writeChunk(const vector<int>& chunk, const string& fileName) {
    ofstream out(fileName);
    for (int num : chunk) {
        out << num << endl;
    }
    out.close();
}

// функция для объединения двух файлов
void mergeFiles(const string& fileName1, const string& fileName2, const string& outFileName) {
    ifstream in1(fileName1);
    ifstream in2(fileName2);
    ofstream out(outFileName);
    int num1, num2;
    bool hasNum1 = bool(in1 >> num1);
    bool hasNum2 = bool(in2 >> num2);
    while (hasNum1 || hasNum2) {
        if (hasNum1 && (!hasNum2 || num1 <= num2)) {
            out << num1 << endl;
            hasNum1 = bool(in1 >> num1);
        }
        else {
            out << num2 << endl;
            hasNum2 = bool(in2 >> num2);
        }
    }
    in1.close();
    in2.close();
    out.close();
}

// функция для сортировки фрагмента файла
void sortChunk(const string& fileName) {
    ifstream in(fileName);
    vector<int> chunk = readChunk(in);
    sort(chunk.begin(), chunk.end());
    in.close();
    writeChunk(chunk, fileName);
}

// функция для выполнения сортировки фрагмента в отдельном потоке
void sortChunkAsync(const string& fileName, promise<void>&& prms) {
    sortChunk(fileName);
    prms.set_value();
}

// функция для сортировки файла
void sortFile() {
    // создаем временные файлы и сортируем фрагменты
    int chunkNum = 0;
    ifstream in(FILE_NAME);

    vector<future<void>> futures;

    while (!in.eof()) {
        vector<int> chunk = readChunk(in);
        string tempFileName = TEMP_FILE_PREFIX + to_string(chunkNum++);
        writeChunk(chunk, tempFileName);
        if (chunkNum <= MAX_CHUNKS) {
            promise<void> prms;
            futures.push_back(prms.get_future());
            thread t(sortChunkAsync, tempFileName, move(prms));
            t.detach();
        }
    }
    in.close();

    // ожидаем завершения потоков
    for (int i = 0; i < NUM_THREADS && i < futures.size(); i++) {
        futures[i].wait();
    }

    // объединяем временные файлы
    queue<string> tempFileNames;
    for (int i = 0; i < chunkNum; i++) {
        tempFileNames.push(TEMP_FILE_PREFIX + to_string(i));
    }
    while (tempFileNames.size() > 1) {
        string fileName1 = tempFileNames.front();
        tempFileNames.pop();
        string fileName2 = tempFileNames.front();
        tempFileNames.pop();
        string mergedFileName = TEMP_FILE_PREFIX + to_string(chunkNum++);
        mergeFiles(fileName1, fileName2, mergedFileName);
        tempFileNames.push(mergedFileName);
        remove(fileName1.c_str());
        remove(fileName2.c_str());
    }

    // переименовываем полученный файл
    string finalFileName = tempFileNames.front();
    rename(finalFileName.c_str(), FILE_NAME.c_str());
}

int main() {
    int numCount = 100; // 100 млн чисел
    generateFile(numCount);
    sortFile();
    return 0;
}