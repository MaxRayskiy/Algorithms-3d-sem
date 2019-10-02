/*!
 * @file main.cpp
 * Найти лексикографически-минимальную строку, построенную по префикс-функции, в алфавите a-z.
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
 * @fucntion StringByPrefixFunction
 * @param prefix_function
 * @return наименьшею лексикограифческую строку
 */
string StringByPrefixFunction(vector<int> prefix_function);

int main() {
    vector<int> prefix_function;

    while (true) {
        int elem;
        cin >> elem;
        if (cin.fail())
            break;
        prefix_function.push_back(elem);
    }

    if (!prefix_function.empty()) {
        prefix_function[0] = 0;
        cout << StringByPrefixFunction(prefix_function);
    }

    return 0;
}

string StringByPrefixFunction(vector<int> prefix_function) {
    vector<char> this_letter_was_used(26, false);
    string result_string = "a";

    for (unsigned int i = 1; i < prefix_function.size(); ++i) {

        if (prefix_function[i] == 0) {
            int k = prefix_function[i - 1];
            while (k > 0) {
                this_letter_was_used[result_string[k] - 'a'] = 1;
                k = prefix_function[k - 1];
            }

            for (unsigned int j = 1; j < this_letter_was_used.size(); ++j) {
                if (!this_letter_was_used[j]) {
                    result_string += ('a'+ j);

                    this_letter_was_used.clear();
                    this_letter_was_used.resize(26, false);
                    break;
                }
            }
        } else
            result_string += result_string[prefix_function[i] - 1];
    }
    return result_string;
}