// bst.h - cay nhi phan tim kiem KHONG can bang (Tuan 3), dung de so sanh voi B-Tree
// tren cung du lieu va cung thao tac. Moi ham deu viet bang vong lap (khong de quy)
// vi voi du lieu tang dan cay suy bien thanh danh sach, chieu cao ~ n.
#pragma once
#include <vector>

class BST {
public:
    BST() = default;
    ~BST();
    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    bool search(int key) const;
    bool insert(int key);              // false neu khoa da ton tai
    bool remove(int key);              // false neu khoa khong ton tai
    std::vector<int> traverse() const; // in-order
    int  height() const;               // so canh tu goc den la sau nhat; cay rong: -1
    long size() const { return size_; }
    long long heapBytes() const { return size_ * static_cast<long long>(sizeof(Node)); }

    long nodesVisited() const { return visited_; }   // moi nut = 1 lan doc dia (cung mo hinh voi BTree)
    void resetVisited() const { visited_ = 0; }

private:
    struct Node {
        int key;
        Node* left = nullptr;
        Node* right = nullptr;
        explicit Node(int k) : key(k) {}
    };
    Node* root_ = nullptr;
    long size_ = 0;
    mutable long visited_ = 0;
};
