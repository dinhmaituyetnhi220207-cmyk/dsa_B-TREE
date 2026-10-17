#pragma once
#include <vector>
#include <string>

class BTree {
public:
    explicit BTree(int order = 5);
    ~BTree();

    BTree(const BTree&) = delete;
    BTree& operator=(const BTree&) = delete;

    bool search(int key, std::vector<std::vector<int>>* path = nullptr) const;
    bool insert(int key);
    bool remove(int key);
    std::vector<int> traverse() const;
    bool validate(std::string* error = nullptr) const;
    int height() const;

    int order() const { return m_; }
    long nodesVisited() const { return visited_; }

private:
    struct Node {
        std::vector<int> keys;
        std::vector<Node*> children;
        bool isLeaf() const { return children.empty(); }
    };

    int m_;
    Node* root_ = nullptr;
    mutable long visited_ = 0;

    void destroy(Node* node);
    void splitChild(Node* parent, int i, Node* fullChild);
    void insertNonFull(Node* node, int key);
    void inOrderHelper(Node* node, std::vector<int>& result) const;
};