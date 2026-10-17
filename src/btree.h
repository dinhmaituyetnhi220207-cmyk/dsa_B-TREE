// btree.h - B-Tree bac m theo dinh nghia Knuth (moi nut toi da m con, m-1 khoa).
// Quy uoc (bao cao Tuan 1, Muc 3.6):
//   - Chen: luon chen vao la; nut co m khoa (tran) thi tach, day khoa giua len cha.
//   - Xoa: khoa o nut trong thi thay bang khoa lien truoc; nut thieu khoa thi
//          muon anh em trai, roi anh em phai; khong muon duoc thi gop.
#pragma once
#include <string>
#include <vector>

class BTree {
public:
    explicit BTree(int order = 5);   // order = m, yeu cau m >= 3
    ~BTree();
    BTree(const BTree&) = delete;
    BTree& operator=(const BTree&) = delete;

    // Tim khoa; neu path != nullptr thi ghi lai khoa cua tung nut da tham.
    bool search(int key, std::vector<std::vector<int>>* path = nullptr) const;
    bool insert(int key);                        // false neu khoa da ton tai
    bool remove(int key);                        // false neu khoa khong ton tai
    std::vector<int> traverse() const;           // duyet in-order (tang dan)
    bool validate(std::string* error = nullptr) const;  // kiem tra du 5 tinh chat
    int  height() const;                         // so canh tu goc den la; cay rong: -1
    std::string levels() const;                  // vd "[20 40] | [10 15] [30 35] [45 50]"

    int  order() const { return m_; }
    long nodesVisited() const { return visited_; }   // so nut da tham (mo phong doc dia)
    void resetVisited() const { visited_ = 0; }

private:
    struct Node {
        std::vector<int>   keys;      // tang dan; toi da m-1 khoa (tam thoi m khi tran)
        std::vector<Node*> children;  // rong neu la la; nguoc lai co keys.size()+1 con
        bool isLeaf() const { return children.empty(); }
    };
    struct Split {                    // ket qua khi mot nut bi tach
        bool  happened = false;
        int   median = 0;             // khoa giua day len cha
        Node* right = nullptr;        // nut moi ben phai
    };

    int   m_;
    Node* root_ = nullptr;
    mutable long visited_ = 0;

    std::size_t minKeys() const { return static_cast<std::size_t>((m_ + 1) / 2 - 1); }  // ceil(m/2) - 1
    static std::size_t lowerIndex(const Node* n, int key);

    void  destroy(Node* node);
    Split insertRec(Node* node, int key, bool& inserted);
    Split splitNode(Node* node);
    bool  removeRec(Node* node, int key);
    void  fixChild(Node* parent, std::size_t i);
    void  mergeChildren(Node* parent, std::size_t i);
    void  inOrderHelper(const Node* node, std::vector<int>& result) const;
    bool  validateRec(const Node* node, bool isRoot, const long long* lo, const long long* hi,
                      int depth, int& leafDepth, std::string& why) const;
};
