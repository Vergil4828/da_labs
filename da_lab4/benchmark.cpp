#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

vector<int> computeZ(const vector<string>& s) {
    int n = s.size();
    vector<int> z(n, 0);
    int l = 0, r = 0;
    for (int i = 1; i < n; ++i) {
        if (i <= r) z[i] = min(r - i + 1, z[i - l]);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) z[i]++;
        if (i + z[i] - 1 > r) {
            l = i;
            r = i + z[i] - 1;
        }
    }
    return z;
}

void toLowerCase(string& s) {
    for (char& c : s)
        if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 'a');
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    string patternLine, word;
    if (!getline(cin, patternLine)) return 0;

    vector<string> pattern;
    stringstream ssP(patternLine);
    while (ssP >> word) {
        toLowerCase(word);
        pattern.push_back(word);
    }

    vector<string> textWords;
    string textLine;
    while (getline(cin, textLine)) {
        stringstream ssT(textLine);
        while (ssT >> word) {
            toLowerCase(word);
            textWords.push_back(word);
        }
    }

    if (pattern.empty() || textWords.empty()) return 0;

    std::chrono::time_point<std::chrono::high_resolution_clock> startZ = std::chrono::high_resolution_clock::now();

    vector<string> combined;
    combined.reserve(pattern.size() + textWords.size() + 1);
    combined.insert(combined.end(), pattern.begin(), pattern.end());
    combined.push_back("\0");
    combined.insert(combined.end(), textWords.begin(), textWords.end());

    vector<int> z = computeZ(combined);
    int countZ = 0;
    int m = pattern.size();
    for (int i = 0; i < (int)textWords.size(); ++i) {
        if (z[i + m + 1] == m) countZ++;
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> endZ = std::chrono::high_resolution_clock::now();

    std::chrono::time_point<std::chrono::high_resolution_clock> startNaive = std::chrono::high_resolution_clock::now();

    int countNaive = 0;
    if (textWords.size() >= pattern.size()) {
        for (size_t i = 0; i <= textWords.size() - pattern.size(); ++i) {
            bool match = true;
            for (size_t j = 0; j < pattern.size(); ++j) {
                if (textWords[i + j] != pattern[j]) {
                    match = false;
                    break;
                }
            }
            if (match) countNaive++;
        }
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> endNaive = std::chrono::high_resolution_clock::now();

    chrono::duration<double, milli> durZ = endZ - startZ;
    chrono::duration<double, milli> durNaive = endNaive - startNaive;

    cerr << fixed << setprecision(3);
    cerr << "Z-Algorithm:    " << durZ.count() << " ms (found " << countZ << ")\n";
    cerr << "Naive Search:   " << durNaive.count() << " ms (found " << countNaive << ")\n";

    return 0;
}