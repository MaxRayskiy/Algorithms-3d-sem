/*! @file main.cpp
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

/*!
 * @function SubstringSearch
 * @param pattern - шаблон
 * @param text - обрабатываемый текст
 * @param answers - вектор ответов
 */
void SubstringSearch(string &pattern, string &text, vector<int> &answers);

/*!
 * @function ZFunction
 * @param text - анализируемый текст
 * @param z_function - результат z-функции на этом тектсе
 */
void ZFunction(string &text, vector<int> &z_function);

int main() {
    string pattern, text;
    cin >> pattern >> text;

    vector<int> answers; // сюда занесем ответы
    SubstringSearch(pattern, text, answers);

    for (const auto answer: answers)
        cout << answer << " ";
    cout << endl;

    return 0;
}

void SubstringSearch(string &pattern, string &text, vector<int> &answers) {
    string full_text = pattern + '#' + text;
    vector<int> z_function_result(full_text.size());
    ZFunction(full_text, z_function_result);

    int pattern_len = pattern.length();
    for (unsigned int i = pattern_len + 1; i < z_function_result.size(); ++i)
        if (z_function_result[i] == pattern_len)
            answers.push_back(i - pattern_len - 1);
}

void ZFunction(string &text, vector<int> &z_function) {
    int text_length = text.length();

    z_function[0] = text_length;

    int left = 0, right = 0;
    for (int i = 1; i < text_length; ++i) {
        z_function[i] = std::max(0, std::min( right - i, z_function[i - left]));
        while (i + z_function[i] < text_length && text[z_function[i] + i] == text[z_function[i]])
            ++z_function[i];
        if (i + z_function[i] > right) {
            left = i;
            right = i + z_function[i];
        }
    }
}