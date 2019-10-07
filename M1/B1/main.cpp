/*
 * Найти лексикографически-минимальную строку, построенную по префикс-функции, в алфавите a-z.
 */
#include <iostream>
#include <string>
#include <vector>

using std::string;
using std::vector;


vector<int> PrefixFunctionFromString(const string &text);

vector<int> ZFunctionFromString(const string &text);

vector<int> PrefixFunctionFromZFunction(const vector<int> &z_function);

vector<int> ZFunctionFromPrefixFunction(const vector<int> &prefix_function);

string StringFromPrefixFunction(const vector<int> &prefix_function);

string StringFromZFunction(const vector<int> &z_function);


int main() {
    vector<int> prefix_function;

    int elem;
    while (std::cin >> elem) {
        prefix_function.push_back(elem);
    }

    if (!prefix_function.empty()) {
        prefix_function[0] = 0;
        std::cout << StringFromPrefixFunction(prefix_function) << std::endl;
    }
    return 0;
}

string StringFromPrefixFunction(const vector<int> &prefix_function) {
    vector<char> used(26, false);
    string result_string = "a";

    for (unsigned int i = 1; i < prefix_function.size(); ++i) {
        if (prefix_function[i] == 0) {
            int k = prefix_function[i - 1];
            while (k > 0) {
                used[result_string[k] - 'a'] = 1;
                k = prefix_function[k - 1];
            }
            for (unsigned int j = 1; j < used.size(); ++j) {
                if (!used[j]) {
                    result_string += ('a'+ j);

                    used.clear();
                    used.resize(26, false);
                    break;
                }
            }
        } else {
            result_string += result_string[prefix_function[i] - 1];
        }
    }
    return result_string;
}

vector<int> PrefixFunctionFromZFunction(const vector<int> &z_func) {
    vector<int> prefix_func(z_func.size());
    for (unsigned int i = 0; i < z_func.size(); ++i) {
        for (int j = z_func[i] - 1; j >= 0; --j)
            if (prefix_func[i + j] > 0) {
                break;
            } else {
                prefix_func[i + j] = j + 1;
            }
    }
    return prefix_func;
}

vector<int> ZFunctionFromString(const string &text) {
    int text_length = text.length();

    vector<int> z_function(text.size());
    z_function[0] = text_length;

    int left = 0, right = 0;
    for (int i = 1; i < text_length; ++i) {
        z_function[i] = std::max(0, std::min( right - i, z_function[i - left]));
        while (i + z_function[i] < text_length && text[z_function[i] + i] == text[z_function[i]]) {
            ++z_function[i];
        }
        if (i + z_function[i] > right) {
            left = i;
            right = i + z_function[i];
        }
    }
    return z_function;
}

vector<int> PrefixFunctionFromString(const string &text) {
    return PrefixFunctionFromZFunction(ZFunctionFromString(text));
}

vector<int> ZFunctionFromPrefixFunction(const vector<int> &prefix_function) {
    return ZFunctionFromString(StringFromPrefixFunction(prefix_function));
}

string StringFromZFunction(const vector<int> &z_function) {
    return StringFromPrefixFunction((PrefixFunctionFromZFunction(z_function)));
}