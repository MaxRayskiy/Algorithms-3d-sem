/*
Дана строка длины n. Найти количество ее различных подстрок. Используйте суффиксный массив.
Построение суффиксного массива выполняйте за O(n log n).
Вычисление количества различных подстрок выполняйте за O(n).
 */

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>

using std::string;
using std::vector;

class SuffArray {
 public:
    explicit SuffArray(string &str);
    void BuildSuffArray();
    unsigned int NumOfDiffStr();

 private:
    int Prepare();
    void Build(int class_cnt);

    const string str;
    const unsigned int str_len;

    vector<unsigned int> p;
    vector<unsigned int> str_class;
    vector<unsigned int> cnt_alpha_pos;
};

class LCP {
 public:
    LCP(const string &str, const vector<unsigned int> &suff_array);
    vector<int> GetLCP() const;

 private:
    vector<int> lcp_array;
};

int main() {
    string str;
    std::cin >> str;
    SuffArray SuffArray(str);
    std::cout << SuffArray.NumOfDiffStr() << std::endl;
    return 0;
}

SuffArray::SuffArray(string &str)
    : str_len(str.length() + 1)
    , str(str + '$')
    , p(str.length() + 1)
    , str_class(str.length() + 1)
    , cnt_alpha_pos(26 + 1)
{}

LCP::LCP(const string &str, const vector<unsigned int> &suff_array)
    : lcp_array(str.length())
{
    int str_len = str.length();

    vector<unsigned int> pos(str_len);  // reverse array
    for (int i = 0; i < str_len; ++i) {
        pos[suff_array[i]] = i;
    }

    int prev_lcp = 0;
    for (int i = 0; i < str_len - 1; ++i) {
        if (prev_lcp > 0) {
            --prev_lcp;
        }
        if (pos[i] == str_len - 1) {  // if str[pos[i]] == '$"
            lcp_array[str_len - 1] = - 1;
            prev_lcp = 0;
        } else {
            int next_str_beg = suff_array[pos[i] + 1];
            while (std::max(i, next_str_beg) + prev_lcp < str_len
                && str[i + prev_lcp] == str[next_str_beg + prev_lcp]) {
                ++prev_lcp;
            }
            lcp_array[pos[i]] = prev_lcp;
        }
    }
}

vector<int> LCP::GetLCP() const {
    return lcp_array;
}

void SuffArray::BuildSuffArray() {
    int cnt = Prepare();  //
    Build(cnt);  //
}

unsigned int SuffArray::NumOfDiffStr() {
    if (str_len == 1) {
        return 0;
    }
    BuildSuffArray();
    LCP LCP(str, p);
    vector<int> lcp = LCP.GetLCP();
    unsigned int input_str_len = str_len - 1;  // except '$'

    unsigned int result = 0;
    for (unsigned int i = 1; i < str_len; ++i) {  // except $ on p[0]; lcp[0] = 0
        result += input_str_len - p[i] - lcp[i];
    }
    result -= 1;  // except '$' in lcp_array[str_len - 1]
    return result;
}

int SuffArray::Prepare() {
    // counting sort
    int num_by_ch = 0;
    for (unsigned int i = 0; i < str_len; ++i) {
        num_by_ch = str[i] - 'a' + 1;
        if (num_by_ch < 0) {
            num_by_ch = 0;
        }
        ++cnt_alpha_pos[num_by_ch];
    }

    for (unsigned int i = 1; i < cnt_alpha_pos.size(); ++i) {
        cnt_alpha_pos[i] += cnt_alpha_pos[i - 1];
    }

    for (unsigned int i = 0; i < str_len; ++i) {
        num_by_ch = str[i] - 'a' + 1;
        if (num_by_ch < 0) {
            num_by_ch = 0;
        }
        p[--cnt_alpha_pos[num_by_ch]]  = i;
    }

    // define classes
    unsigned int class_cnt = 0;
    str_class[p[0]] = 0;
    for (unsigned int i = 1; i < str_len; ++i) {
        if (str[p[i]] != str[p[i - 1]]) {
            ++class_cnt;
        }
        str_class[p[i]] = class_cnt;
    }
    return class_cnt;
}

void SuffArray::Build(int class_cnt) {
    vector<unsigned int> pn(str_len);
    vector<unsigned int> temp_str_class;

    unsigned int half_len = 1;
    while (half_len < str_len) {
        // counting sort by LSD
        for (unsigned int i = 0; i < str_len; ++i) {
            pn[i] = (str_len + p[i] - half_len) % str_len;
        }

        cnt_alpha_pos.clear();
        cnt_alpha_pos.resize(class_cnt + 1);
        for (auto str_beg : pn) {
            ++cnt_alpha_pos[str_class[str_beg]];
        }
        for (unsigned int i = 1; i < cnt_alpha_pos.size(); ++i) {
            cnt_alpha_pos[i] += cnt_alpha_pos[i - 1];
        }

        for (int i = static_cast<int>(str_len) - 1; i >= 0; --i) {
            unsigned int current_class = str_class[pn[i]];
            p[--cnt_alpha_pos[current_class]] = pn[i];
        }

        // define classes
        class_cnt = 0;
        temp_str_class.resize(str_len);
        temp_str_class[p[0]] = class_cnt;

        for (int i = 1; i < str_len; ++i) {
            if (str_class[p[i]] == str_class[p[i - 1]]) {
                unsigned int lhv_str_sec_pos = (p[i - 1] + half_len) % str_len;
                unsigned int rhv_str_sec_pos = (p[i] + half_len) % str_len;

                if (str_class[lhv_str_sec_pos] == str_class[rhv_str_sec_pos]) {
                    temp_str_class[p[i]] = class_cnt;
                    continue;
                }
            }
            temp_str_class[p[i]] = ++class_cnt;
        }

        str_class = std::move(temp_str_class);
        temp_str_class.clear();

        half_len *= 2;
    }
}
