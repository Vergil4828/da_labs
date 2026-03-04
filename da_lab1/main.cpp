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

    void Print() {
        std::cout << std::fixed << std::setprecision(6) << key << '\t' << value << "\n";
    }
};

struct TNode {
    TPair data;
    TNode* next;
};

void InsertionSort(TNode*& head, TPair item) {
    TNode* newNode = new TNode;
    newNode->data = item;

    if (!head || head->data.key > item.key) {
        newNode->next = head;
        head = newNode;
    } else {
        TNode* curr = head;
        while (curr->next && curr->next->data.key <= item.key) {
            curr = curr->next;
        }
        newNode->next = curr->next;
        curr->next = newNode;
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    TPair* raw = nullptr;
    int startSize = 10;
    int realSize = 0;
    raw = new TPair[startSize];

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
            for (int i = 0; i < realSize; ++i) temp[i] = raw[i];
            delete[] raw;
            raw = temp;
        }
        raw[realSize].key = inputKey;
        raw[realSize].value = inputValue;
        realSize++;
    }

    if (realSize == 0) {
        delete[] raw;
        return 0;
    }

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    TNode** buckets = new TNode*[realSize];
    for (int i = 0; i < realSize; ++i) {
        buckets[i] = nullptr;
    }
    for (int i = 0; i < realSize; ++i) {
        int idx = (int)((raw[i].key - MIN_KEY) / RANGE * (realSize - 1));
        if (idx < 0) {
            idx = 0;
        }
        if (idx >= realSize) {
            idx = realSize - 1;
        }
        InsertionSort(buckets[idx], raw[i]);
    }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> time = end - start;
    std::cerr << "Sorting time: " << std::fixed << std::setprecision(3) << time.count() << " ms" << std::endl;

    for (int i = 0; i < realSize; ++i) {
        TNode* curr = buckets[i];
        while (curr) {
            curr->data.Print();
            TNode* deleteCurr = curr;
            curr = curr->next;
            delete deleteCurr;
        }
    }

    delete[] buckets;
    delete[] raw;

    return 0;
}