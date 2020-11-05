/*
 Шаблон поиска задан строкой длины m, в которой кроме обычных символов могут встречаться символы “?”.
 Найти позиции всех вхождений шаблона в тексте длины n. Каждое вхождение шаблона предполагает,
 что все обычные символы совпадают с соответствующими из текста,
 а вместо символа “?” в тексте встречается произвольный символ.

 Время работы - O(n + m + Z),
 где Z - общее -число вхождений подстрок шаблона “между вопросиками” в исходном тексте. m ≤ 5000, n ≤ 2000000.
 */
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>

using std::vector;
using std::string;
using std::queue;
using std::unordered_map;
using std::find;

struct Node {
    Node();
    Node(Node *parent, char parent_char);

    unordered_map<char, Node *> child; // хеш-таблица сыновей
    unordered_map<char, Node *> go;    // хеш-таблица переходов
    Node *parent;                      // вершина родитель
    Node *suff_link;                   // суффиксная ссылка (вычисляем в ленивой рекурсии)
    Node *up;                          // сжатая суффиксная ссылка
    char parent_char;                  // символ, ведущий к родителю
    bool is_terminal;                  // является ли вершина терминалом
    vector<int> leaf_pattern_number;   // номера строк, за которые отвечает терминал
};

class StringSearchAlgorithm {
public:
    StringSearchAlgorithm();
    ~StringSearchAlgorithm();

    void ProcessText(const string &text, string &pattern);

    void DumpPatternSearchResult(vector<int> &results);

private:
    Node *root;

    void AddPattern(const string &pattern, unsigned int pattern_num, unsigned int start_pos);

    Node* GetSuffLink(Node * current_node);
    Node* GetLink(Node *current_node, char c);
    Node* GetUp(Node *current_node);

    vector<int> pattern_begins_cnt;
    vector<int> starting_positions;
    vector<int> pattern_len;
};


int main() {
    std::ios_base::sync_with_stdio(false);
    StringSearchAlgorithm NewSearch;

    string full_pattern;
    std::cin >> full_pattern;

    string text;
    std::cin >> text;
    NewSearch.ProcessText(text, full_pattern);

    vector<int> results;
    NewSearch.DumpPatternSearchResult(results);
    for (const auto& result : results) {
        std::cout << result << " ";
    }
    std::cout << std::endl;

    return 0;
}

Node::Node()
        : parent(nullptr)
        , suff_link(nullptr)
        , up(nullptr)
        , parent_char('\0')
        , is_terminal(false)
{}

Node::Node(Node *parent, char parent_char)
        : parent(parent)
        , suff_link(nullptr)
        , up (nullptr)
        , parent_char(parent_char)
        , is_terminal(false)
{}

StringSearchAlgorithm::StringSearchAlgorithm() {
    root = new Node;
}

StringSearchAlgorithm::~StringSearchAlgorithm() {
    queue<Node*> queue;
    queue.push(root);

    while (!queue.empty()) {
        Node *current_node = queue.front();
        queue.pop();

        for (auto &child : current_node->child) {
            queue.push(child.second);
        }

        delete current_node;
    }
}

Node* StringSearchAlgorithm::GetSuffLink(Node *current_node) {
    if (current_node->suff_link == nullptr) {
        if (current_node == root || current_node->parent == root) {
            current_node->suff_link = root;
        } else {
            current_node->suff_link = GetLink(GetSuffLink(current_node->parent), current_node->parent_char);
        }
    }
    return current_node->suff_link;
}

Node* StringSearchAlgorithm::GetLink(Node *current_node, char c) {
    if (current_node->go.find(c) == current_node->go.end()) {
        if (current_node->child.find(c) != current_node->child.end()) {
            current_node->go[c] = current_node->child[c];
        } else if (current_node == root) {
            current_node->go[c] = root;
        } else {
            current_node->go[c] = GetLink(GetSuffLink(current_node), c);
        }
    }
    return current_node->go[c];
}

Node* StringSearchAlgorithm::GetUp(Node *current_node) {
    if (current_node->up == nullptr) {
        if (GetSuffLink(current_node)->is_terminal) {
            current_node->up = GetSuffLink(current_node);
        } else if (GetSuffLink(current_node) == root) {
            current_node->up = root;
        } else {
            current_node->up = GetUp(GetSuffLink(current_node));
        }
    }
    return current_node->up;
}

void StringSearchAlgorithm::AddPattern(const string &pattern, unsigned int pattern_num, unsigned int start_pos) {
    Node *current_node = root;
    for (char c : pattern) {
        if (current_node->child.find(c) == current_node->child.end()) {
            Node *new_child = new Node(current_node, c);
            current_node->child[c] = new_child;
        }
        current_node = current_node->child[c];
    }
    current_node->is_terminal = true;
    current_node->leaf_pattern_number.push_back(pattern_num);

    starting_positions.push_back(start_pos);
    pattern_len.push_back(pattern.length());
}

void StringSearchAlgorithm::ProcessText(const string &text, string &full_pattern) {
    pattern_begins_cnt.resize(text.length(), 0);

    string pattern;
    int pattern_num = 0;
    int cycle_counter = 0;
    int pattern_begin_position = 0;

    for(char pattern_char : full_pattern) {
        if (pattern_char == '?') {
            if (pattern.length() > 0) {
                AddPattern(pattern, pattern_num, pattern_begin_position);
                ++pattern_num;
                pattern.clear();
            }
            pattern_begin_position = cycle_counter + 1;
        } else {
            pattern += pattern_char;
        }
        ++cycle_counter;
    }

    if (pattern.length() > 0) {
        AddPattern(pattern, pattern_num, pattern_begin_position);
    }

    Node *current_node = root;
    for (int i = 0; i < text.size(); ++i) {
        current_node = GetLink(current_node, text[i]);

        Node *check_node = current_node;
        while (check_node != root) {
            if (check_node->is_terminal) {
                for (const auto num : check_node->leaf_pattern_number) {
                    if (i - starting_positions[num] - pattern_len[num]  + 1 <= (int)pattern_begins_cnt.size() - (int)full_pattern.size() &&
                        i - starting_positions[num] - pattern_len[num]  + 1 >= 0) {
                        ++pattern_begins_cnt[i - starting_positions[num] - pattern_len[num] + 1];
                    }
                }
            }
            check_node = GetUp(check_node);
        }
    }
}

void StringSearchAlgorithm::DumpPatternSearchResult(vector<int> &results) {
    for (unsigned int i = 0; i < pattern_begins_cnt.size(); ++i) {
        if (pattern_begins_cnt[i] == starting_positions.size())
            results.push_back(i);
    }
}
