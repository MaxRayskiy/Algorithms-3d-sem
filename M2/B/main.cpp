/*
 Даны строки s и t. Постройте сжатое суффиксное дерево, которое содержит все суффиксы строки s и строки t.
 Найдите такое дерево, которое содержит минимальное количество вершин.
 */

#include <iostream>
#include <map>
#include <queue>
#include <vector>
#include <stack>
#include <string>

using std::vector;

class SuffTree {
 public:
    explicit SuffTree(const std::string& str1, const std::string& str2);
    ~SuffTree();
    void BuildSuffTree();
    void DumpTree(vector<vector<int>>& result);

 private:
    struct Node {
      int lf;   //  left (or first) index of char in the str
      int rg;   //  right (or last) index of char in the str
      bool is_terminal;

      Node* parent;
      Node* suff_link;
      int num;
      std::map<char, Node*> next;

      Node(int lf, int rg, Node* parent);
      Node();
    };

    struct ActiveNode {
      Node* node;
      char edge;
      int len;

      ActiveNode(Node *active_node, char edge, int len);
    };

    Node* AddChar(const char& ch);
    Node* InsertNode(Node* parent, Node* child, int offset);
    SuffTree::Node* InsertLeafNode(char ch);
    Node* GetNextNode(ActiveNode& act_node, char ch) const;
    Node* ProceedToActive(const char& ch);
    Node* ExtendEdge();
    Node* BranchEdge(Node* next_node, const char& ch);
    int Len(Node* node);

    ActiveNode active;
    int reminder;
    int current_size;
    Node* root;
    const std::string str;
    const int separator;
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::string in;

    std::string s, t;
    std::cin >> s >> t;
    SuffTree Tree(s, t);
    Tree.BuildSuffTree();

    std::vector<std::vector<int>> result;
    Tree.DumpTree(result);

    std::cout << result.size() + 1 << std::endl;
    for (const auto& line : result) {
        for (const auto& item : line) {
            printf("%d ", item);
        }
        printf("\n");
    }
    return 0;
}
SuffTree::ActiveNode::ActiveNode(Node* active_node, char edge, int len)
    : node(active_node)
    , edge(edge)
    , len(len)
{}

SuffTree::Node::Node(int lf, int rg, SuffTree::Node* parent)
    : lf(lf)
    , rg(rg)
    , is_terminal(true)
    , parent(parent)
    , suff_link(nullptr)
    , num(-1)
{}

SuffTree::Node::Node()
    : lf(-1)
    , rg(-1)
    , is_terminal(true)
    , parent(nullptr)
    , suff_link(nullptr)
    , num(-1)
{}

SuffTree::SuffTree(const std::string& str1, const std::string& str2)
    : str(str1 + str2)
    , active(nullptr, '\0', 0)
    , reminder(0)
    , current_size(0)
    , root(nullptr)
    , separator(static_cast<int>(str1.size()) - 1)
{}

void SuffTree::BuildSuffTree() {
    root = new Node(0, 0, nullptr);
    root->is_terminal = false;
    active.node = root;

    for (const auto& ch : str) {
        ++current_size;
        AddChar(ch);
    }
}

SuffTree::Node* SuffTree::AddChar(const char& ch) {
    if (reminder == 0) {
        active.edge = '\0';
    }
    Node *next_node = ProceedToActive(ch);  // move to real active node

    if (next_node == nullptr) {
        return InsertLeafNode(ch);
    }
    if (str[next_node->lf + active.len] == ch) {  // ch is on the edge
        active.edge = str[next_node->lf];
        return ExtendEdge();
    } else {
        return BranchEdge(next_node, ch);
    }
}

SuffTree::Node* SuffTree::InsertNode(Node* parent, Node* child, int offset) {
    if (child == nullptr) {  // add another one leaf to the Node "parent"
        Node* new_node = new Node(current_size - 1, current_size, parent);
        parent->next[str[current_size - 1]] = new_node;
        return parent;
    } else { // create new Node on the edge  : parent-child => parent-new_parent-child
        //                                                                \_ new_child
        Node* new_parent = new Node(child->lf, child->lf + offset - 1, parent);
        parent->next[str[child->lf]] = new_parent;
        new_parent->is_terminal = false;

        child->lf += offset;
        child->parent = new_parent;
        new_parent->next[str[child->lf]] = child;

        Node* new_child = new Node(current_size - 1, current_size, new_parent);  // is leaf
        new_parent->next[str[new_child->lf]] = new_child;

        return new_parent;
    }
}

int SuffTree::Len(SuffTree::Node *node) {
    if (node->is_terminal) {
        return (current_size - node->lf + 1);
    } else {
        return (node->rg - node->lf + 1);
    }
}

SuffTree::Node* SuffTree::GetNextNode(ActiveNode& act_node, char ch) const {
    char search = ch;
    if (act_node.edge != '\0') {
        search = act_node.edge;
    }
    auto next = act_node.node->next.find(search);
    if (next == act_node.node->next.end()) {
        return nullptr;
    }
    return next->second;   // map<char, Node*>
}

void SuffTree::DumpTree(vector<vector<int>>& result) {
    result.clear();
    int cnt = 0;

    std::stack<Node*> st;
    st.push(root);
    while (!st.empty()) {
        Node* current_node = st.top();
        st.pop();
        current_node->num = cnt;
        ++cnt;

        if (!current_node->next.empty()) {
            auto it = current_node->next.end();
            do {
                --it;
                st.push(it->second);
            } while (it != current_node->next.begin());
        }
        if (current_node == root) {
            continue;
        }
        int str_num = 0;
        int lf = current_node->lf;
        int rg = current_node->rg;
        if (current_node->is_terminal) {
            rg = separator;
        }
        if (current_node->lf > separator) {  // str2
            if (current_node->is_terminal) {
                rg = current_size - 1;
            }
            str_num = 1;
            lf -= separator + 1;
            rg -= separator + 1;
        }

        result.emplace_back(std::initializer_list<int> {current_node->parent->num, str_num, lf, rg + 1});
    }
}

SuffTree::~SuffTree() {
    std::queue<Node*> queue;
    queue.push(root);

    while (!queue.empty()) {
        Node *current_node = queue.front();
        queue.pop();

        for (auto & child : current_node->next) {
            if (child.second != nullptr) {
                queue.push(child.second);
            }
        }

        delete current_node;
    }
}

SuffTree::Node* SuffTree::ProceedToActive(const char& ch) {
    Node* next_node = GetNextNode(active, ch);
    while (next_node != nullptr && active.len >= Len(next_node)) {  // if active points to a next edge
        active.node = next_node;
        active.len -= Len(next_node);
        if (active.len == 0) {
            active.edge = '\0';
        } else {
            active.edge = str[current_size - 1 - active.len ];
        }
        next_node = GetNextNode(active, ch);
    }
    return next_node;
}

SuffTree::Node* SuffTree::InsertLeafNode(char ch) {
    Node* inserted_node = InsertNode(active.node, nullptr, 0);
    if (active.node == root) {
        if (active.len > 0) {
            --active.len;
        }
        if (reminder > 0) {
            active.edge = str[current_size - active.len - 1];
        }
        return root;
    } else {
        --reminder;
        Node* save_node = active.node;
        if (active.node->suff_link == nullptr || active.node->suff_link == root) {
            active.node = root;
        } else {
            active.node = active.node->suff_link;
        }

        if (reminder == 0) {
            active.len = 0;
        }
        if (reminder >= 0) {
            if (inserted_node->suff_link == nullptr) {
                inserted_node->suff_link = AddChar(ch);
                if (inserted_node->suff_link == root) {
                    inserted_node->suff_link = nullptr;
                }
            } else {
                AddChar(ch);
            }
        }
        return save_node;
    }
}

SuffTree::Node* SuffTree::ExtendEdge() {
    Node* return_node = nullptr;
    if (active.len == 0) {
        return_node = active.node;
    }
    ++reminder;
    ++active.len;
    return return_node;
}

SuffTree::Node* SuffTree::BranchEdge(Node* next_node, const char& ch) {
    Node* inserted_node = InsertNode(active.node, next_node, active.len);
    --reminder;

    if (active.node == root && active.len > 0) {
        --active.len;
    }

    if (active.node->suff_link == nullptr || active.node->suff_link == root) {
        active.node = root;
    } else if (active.node != root) {
        active.node = active.node->suff_link;
    }

    if (reminder == 0) {
        active.len = 0;
    }
    active.edge = str[current_size - active.len - 1];

    if (reminder > 0 || active.node == root) {
        if (inserted_node->suff_link == nullptr) {
            inserted_node->suff_link = AddChar(ch);
            if (inserted_node->suff_link == root) {
                inserted_node->suff_link = nullptr;
            }
        } else {
            AddChar(ch);
        }
    }
    return inserted_node;
}
