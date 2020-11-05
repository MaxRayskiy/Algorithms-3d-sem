/*
 Заданы две строки s, t и целое число k. Рассмотрим множество всех таких непустых строк,
 которые встречаются как подстроки в s и t одновременно.
 Найдите k-ую в лексикографическом порядке строку из этого множества.
 */
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

using std::string;
using std::vector;

class SuffArray {
 public:
    explicit SuffArray(const string& str1, const string& str2);
    void BuildSuffArray();
    string OrderStat(int64_t k);

 private:
    int64_t Prepare();
    void Build(int64_t class_cnt);
    vector<uint64_t> UpdateClasses(uint64_t half_len);

    const string str;
    const uint64_t separator;
    const uint64_t str_len;

    vector<uint64_t> p;
    vector<uint64_t> str_class;
    vector<uint64_t> cnt_alpha_pos;
};

class LCP {
 public:
    LCP(const string &str, const vector<uint64_t > &suff_array);
    vector<int64_t> GetLCP() const;

 private:
    vector<int64_t> lcp_array;
};

int main() {
    std::string s, t;
    std::cin >> s >> t;
    int64_t num;
    std::cin >> num;
    SuffArray SuffAray(s, t);
    SuffAray.BuildSuffArray();
    string res = SuffAray.OrderStat(num);
    if (res.empty()) {
        std::cout << -1 << std::endl;
    } else {
        std::cout << res << std::endl;
    }
    return 0;
}

SuffArray::SuffArray(const string& str1, const string& str2)
    : str(str1 + '{' + str2 + '$')
    , separator(str1.size())
    , str_len(str1.length() + str2.length() + 2)
    , p(str1.length() + str2.length() + 2)
    , str_class(str1.length() + str2.length() + 2)
    , cnt_alpha_pos(26 + 2)
{}

LCP::LCP(const string &str, const vector<uint64_t> &suff_array)
    : lcp_array(str.length())
{
    uint64_t str_len = str.length();

    vector<uint64_t > pos(str_len);  // reverse array
    for (uint64_t i = 0; i < str_len; ++i) {
        pos[suff_array[i]] = i;
    }

    int64_t prev_lcp = 0;
    for (uint64_t i = 0; i < str_len - 1; ++i) {
        if (prev_lcp > 0) {
            --prev_lcp;
        }
        if (pos[i] == str_len - 1) {  // if str[pos[i]] == '$"
            lcp_array[0] = - 1;
            prev_lcp = 0;
        } else {
            uint64_t next_str_beg = suff_array[pos[i] + 1];
            while (std::max(i, next_str_beg) + prev_lcp < str_len
                && str[i + prev_lcp] == str[next_str_beg + prev_lcp]) {
                ++prev_lcp;
            }
            lcp_array[pos[i] + 1] = prev_lcp;
        }
    }
}

vector<int64_t> LCP::GetLCP() const {
    return lcp_array;
}

void SuffArray::BuildSuffArray() {
    uint64_t cnt = Prepare();  //
    Build(cnt);  //
}

int64_t SuffArray::Prepare() {
    // counting sort
    int64_t num_by_ch = 0;
    for (uint64_t i = 0; i < str_len; ++i) {
        num_by_ch = str[i] - 'a' + 1;
        if (num_by_ch < 0) {
            num_by_ch = 0;
        }
        ++cnt_alpha_pos[num_by_ch];
    }

    for (uint64_t i = 1; i < cnt_alpha_pos.size(); ++i) {
        cnt_alpha_pos[i] += cnt_alpha_pos[i - 1];
    }

    for (int64_t i = str_len - 1; i >= 0; --i) {
        num_by_ch = str[i] - 'a' + 1;
        if (num_by_ch < 0) {
            num_by_ch = 0;
        }
        p[--cnt_alpha_pos[num_by_ch]]  = i;
    }

    // define classes
    uint64_t class_cnt = 0;
    str_class[p[0]] = 0;
    for (uint64_t i = 1; i < str_len; ++i) {
        if (str[p[i]] != str[p[i - 1]]) {
            ++class_cnt;
        }
        str_class[p[i]] = class_cnt;
    }
    return class_cnt;
}

void SuffArray::Build(int64_t class_cnt) {
    vector<uint64_t> pn(str_len);

    uint64_t half_len = 1;
    while (half_len < str_len) {
        // counting sort by LSD
        for (uint64_t i = 0; i < str_len; ++i) {
            pn[i] = (str_len + p[i] - half_len) % str_len;
        }

        cnt_alpha_pos.clear();
        cnt_alpha_pos.resize(class_cnt + 1);
        for (auto str_beg : pn) {
            ++cnt_alpha_pos[str_class[str_beg]];
        }
        for (uint64_t i = 1; i < cnt_alpha_pos.size(); ++i) {
            cnt_alpha_pos[i] += cnt_alpha_pos[i - 1];
        }

        for (int64_t i = static_cast<int64_t>(str_len) - 1; i >= 0; --i) {
            uint64_t current_class = str_class[pn[i]];
            p[--cnt_alpha_pos[current_class]] = pn[i];
        }

        str_class = std::move(UpdateClasses(half_len));
        class_cnt = static_cast<int>(str_class.size() + 1);

        half_len *= 2;
    }
}

vector<uint64_t> SuffArray::UpdateClasses(uint64_t half_len) {
    int class_cnt = 0;
    vector<uint64_t> temp_str_class(str_len);
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
    return temp_str_class;
}

string SuffArray::OrderStat(int64_t k) {
    LCP LCP(str, p);
    vector<int64_t> lcp = LCP.GetLCP();
    int64_t passed_str_cnt = 0;

    for (uint64_t i = 2; i < lcp.size(); ++i) {  // lcp looks like [-1; 0; ....]
        passed_str_cnt = std::min(passed_str_cnt, lcp[i]);  // if lcp decrease
        if ((p[i - 1] < separator && p[i] > separator) || (p[i] < separator && p[i - 1] > separator)) {
            if (k > lcp[i] - passed_str_cnt) {
                if (lcp[i] - passed_str_cnt >= 0) {
                    k -= lcp[i] - passed_str_cnt;
                }
                passed_str_cnt = lcp[i];
            } else {  // str was found
                return str.substr(p[i], passed_str_cnt + k);
            }
        }
    }
    return "-1";
}

