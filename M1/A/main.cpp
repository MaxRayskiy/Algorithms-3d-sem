/*! @file
Найдите все вхождения шаблона в строку. Длина шаблона – p, длина строки – n.
p <= 30000, n <= 300000.

Использовать один из методов:
- С помощью z-функции.

Время: O(n + p),
Доп. память: O(p).
 */
#include <iostream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

void SubstringPositionByPrefixFunction(string &pattern, string &text, vector<int> &answers);

int main() {
    string pattern, text;
    cin >> pattern >> text;

    vector<int> answers;
    SubstringPositionByPrefixFunction(pattern, text, answers);

    for (const int &answer: answers)
        cout << answer << " ";
    cout << endl;

    return 0;
}


void SubstringPositionByPrefixFunction(string &pattern, string &text, vector<int> &answers) {
    unsigned int pattern_size = pattern.size();

    vector<int> pattern_prefix_function(pattern_size);
    pattern_prefix_function[0] = 0;

    int previous_value = 0;
    // pattern prefix function
    for (unsigned int i = 1; i < pattern.size(); ++i) {
        while (previous_value > 0 && pattern[i] != pattern[previous_value])
            previous_value = pattern_prefix_function[previous_value - 1];

        if (pattern[i] == pattern[previous_value])
            previous_value++;

        pattern_prefix_function[i] = previous_value;
    }

    int current_value = 0;
    // (pattern + "#" + text) prefix function
    for (unsigned int i = 0; i < text.size(); ++i) {
        while (current_value > 0 && text[i] != pattern[current_value])
            current_value = pattern_prefix_function[current_value - 1];

        if (text[i] == pattern[current_value])
            current_value++;

        // if
        if (current_value == pattern_size)
            answers.push_back(i - pattern_size + 1);
    }
}