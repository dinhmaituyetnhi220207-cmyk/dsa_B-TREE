// btree.h - B-Tree bac m (dinh nghia Knuth): moi nut toi da m con, m-1 khoa.
// Tuan 1: khai bao cau truc du lieu va giao dien.
// Tuan 2: cai dat cac thao tac trong btree.cpp.
#pragma once
#include <string>
#include <vector>

class BTree {
public:
    explicit BTree(int order = 5);   // order = m, yeu cau m >= 3
    ~BTree();
    BTree(const BTree&) = delete;
    BTree& operator=(const BTree&) = delete;

    // Tim khoa; neu path != nullptr thi ghi lai cac nut da tham (moi nut = danh sach khoa).
    bool search(int key, std::vector<std::vector<int>>* path = nullptr) const;
    bool insert(int key);                       // false neu khoa da ton tai
    bool remove(int key);                       // false neu khoa khong ton tai
    std::vector<int> traverse() const;          // duyet theo thu tu (in-order)
    bool validate(std::string* error = nullptr) const;  // kiem tra 5 tinh chat (test-plan.md)
    int  height() const;                        // so canh tu goc den la; cay rong: -1
    int  order() const { return m_; }
    long nodesVisited() const { return visited_; }  // mo phong so lan doc dia

private:
    struct Node {
        std::vector<int>   keys;      // tang dan; toi da m-1 khoa (tam thoi m khi tran)
        std::vector<Node*> children;  // rong neu la la; nguoc lai co keys.size()+1 con
        bool isLeaf() const { return children.empty(); }
    };

    int   m_;
    Node* root_ = nullptr;
    mutable long visited_ = 0;

    void destroy(Node* node);
    // Ham phu du kien cho Tuan 2:
    //   splitChild(...)  - tach nut tran, day khoa giua len cha
    //   fixUnderflow(...) - muon tu anh em trai/phai hoac gop nut
};
