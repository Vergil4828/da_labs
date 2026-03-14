#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

const double MIN_KEY = -100.0;
const double MAX_KEY = 100.0;
const double RANGE = MAX_KEY - MIN_KEY;

class TPair {
   public:
    double key;
    std::string value;

    TPair() : key(0.0), value("") {}

    void Print() const {
        std::cout << std::fixed << std::setprecision(6) << key << '\t' << value << "\n";
    }
};

void ArrayInsertionSort(TPair* arr, int n) {
    for (int i = 1; i < n; ++i) {
        TPair key = std::move(arr[i]);
        int j = i - 1;
        while (j >= 0 && arr[j].key > key.key) {
            arr[j + 1] = std::move(arr[j]);
            j--;
        }
        arr[j + 1] = std::move(key);
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int startSize = 1024;
    int realSize = 0;
    TPair* raw = new TPair[startSize];

    double inputKey;
    std::string inputValue;

    while (std::cin >> inputKey) {
        std::cin.ignore(1, '\t');
        if (!std::getline(std::cin, inputValue)) {
            break;
        }

        if (realSize >= startSize) {
            startSize *= 2;
            TPair* temp = new TPair[startSize];
            for (int i = 0; i < realSize; ++i) {
                temp[i] = std::move(raw[i]);
            }
            delete[] raw;
            raw = temp;
        }
        raw[realSize].key = inputKey;
        raw[realSize].value = std::move(inputValue);
        realSize++;
    }

    if (realSize == 0) {
        delete[] raw;
        return 0;
    }

    TPair* stlCopy = new TPair[realSize];
    for (int i = 0; i < realSize; ++i) {
        stlCopy[i] = raw[i];
    }

    std::chrono::high_resolution_clock::time_point startBucket = std::chrono::high_resolution_clock::now();

    int bucketCount = realSize;
    int* counts = new int[bucketCount]();

    for (int i = 0; i < realSize; ++i) {
        int idx = (int)((raw[i].key - MIN_KEY) / RANGE * (bucketCount - 1));
        if (idx < 0) {
            idx = 0;
        }
        if (idx >= bucketCount) {
            idx = bucketCount - 1;
        }
        counts[idx]++;
    }

    int* offsets = new int[bucketCount];
    offsets[0] = 0;
    for (int i = 1; i < bucketCount; ++i) {
        offsets[i] = offsets[i - 1] + counts[i - 1];
    }

    TPair* sorted = new TPair[realSize];
    int* currentOffset = new int[bucketCount];
    for (int i = 0; i < bucketCount; ++i) {
        currentOffset[i] = offsets[i];
    }

    for (int i = 0; i < realSize; ++i) {
        int idx = (int)((raw[i].key - MIN_KEY) / RANGE * (bucketCount - 1));
        if (idx < 0) {
            idx = 0;
        }
        if (idx >= bucketCount) {
            idx = bucketCount - 1;
        }
        sorted[currentOffset[idx]++] = std::move(raw[i]);
    }

    for (int i = 0; i < bucketCount; ++i) {
        if (counts[i] > 1) {
            ArrayInsertionSort(sorted + offsets[i], counts[i]);
        }
    }

    std::chrono::high_resolution_clock::time_point endBucket = std::chrono::high_resolution_clock::now();

    std::chrono::high_resolution_clock::time_point startStl = std::chrono::high_resolution_clock::now();
    std::stable_sort(stlCopy, stlCopy + realSize, [](const TPair& a, const TPair& b) {
        return a.key < b.key;
    });
    std::chrono::high_resolution_clock::time_point endStl = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> bucketTime = endBucket - startBucket;
    std::chrono::duration<double, std::milli> stlTime = endStl - startStl;

    bool match = true;
    for (int i = 0; i < realSize; ++i) {
        if (sorted[i].key != stlCopy[i].key || sorted[i].value != stlCopy[i].value) {
            match = false;
            break;
        }
    }
    std::cerr << "Sort Match: " << (match ? "OK" : "ERROR") << "\n";

    std::cerr << std::fixed << std::setprecision(3);
    std::cerr << "Bucket Sort: " << bucketTime.count() << " ms\n";
    std::cerr << "STL Stable Sort: " << stlTime.count() << " ms\n";

    for (int i = 0; i < realSize; ++i) {
        sorted[i].Print();
    }

    delete[] counts;
    delete[] offsets;
    delete[] currentOffset;
    delete[] sorted;
    delete[] raw;
    delete[] stlCopy;

    return 0;
}