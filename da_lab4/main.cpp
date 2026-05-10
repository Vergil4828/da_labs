#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct WordInfo {
    int line;
    int pos;
};

void toLowerCase(string& s) {
    for (char& c : s) {
        if (c >= 'A' && c <= 'Z') {
            c = (char)(c - 'A' + 'a');
        }
    }
}

vector<int> computeZ(const vector<string>& s) {
    int n = s.size();
    vector<int> z(n, 0);
    int l = 0, r = 0;
    for (int i = 1; i < n; ++i) {
        if (i <= r) {
            z[i] = min(r - i + 1, z[i - l]);
        }
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
            z[i]++;
        }
        if (i + z[i] - 1 > r) {
            l = i;
            r = i + z[i] - 1;
        }
    }
    return z;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    string patternLine;
    if (!getline(cin, patternLine)) {
        return 0;
    }

    vector<string> pattern;
    stringstream ssP(patternLine);
    string word;
    while (ssP >> word) {
        toLowerCase(word);
        pattern.push_back(word);
    }

    if (pattern.empty()) {
        return 0;
    }

    vector<string> textWords;
    vector<WordInfo> info;
    string textLine;
    int currentLine = 1;
    while (getline(cin, textLine)) {
        stringstream ssT(textLine);
        int currentWordPos = 1;
        while (ssT >> word) {
            string lowWord = word;
            toLowerCase(lowWord);
            textWords.push_back(lowWord);
            info.push_back({currentLine, currentWordPos});
            currentWordPos++;
        }
        currentLine++;
    }

    if (textWords.empty()) {
        return 0;
    }

    vector<string> combined;
    combined.reserve(pattern.size() + textWords.size() + 1);
    for (const auto& s : pattern) {
        combined.push_back(s);
    }
    combined.push_back("\0");
    for (const auto& s : textWords) {
        combined.push_back(s);
    }

    vector<int> z = computeZ(combined);
    int m = (int)pattern.size();

    for (int i = 0; i < (int)textWords.size(); ++i) {
        if (z[i + m + 1] == m) {
            cout << info[i].line << ", " << info[i].pos << "\n";
        }
    }

    return 0;
}
