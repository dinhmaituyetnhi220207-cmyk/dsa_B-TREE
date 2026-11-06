// bst.cpp - BST khong can bang, cai dat bang vong lap.
#include "bst.h"
#include <utility>

BST::~BST() {                                   // xoa bang ngan xep tu quan ly
    std::vector<Node*> st;
    if (root_) st.push_back(root_);
    while (!st.empty()) {
        Node* n = st.back(); st.pop_back();
        if (n->left) st.push_back(n->left);
        if (n->right) st.push_back(n->right);
        delete n;
    }
}

bool BST::search(int key) const {
    for (const Node* cur = root_; cur; cur = key < cur->key ? cur->left : cur->right) {
        ++visited_;
        if (cur->key == key) return true;
    }
    return false;
}

bool BST::insert(int key) {
    Node** link = &root_;
    while (*link) {
        ++visited_;
        if (key == (*link)->key) return false;
        link = key < (*link)->key ? &(*link)->left : &(*link)->right;
    }
    *link = new Node(key);
    ++size_;
    return true;
}

bool BST::remove(int key) {
    Node** link = &root_;
    while (*link && (*link)->key != key) {
        ++visited_;
        link = key < (*link)->key ? &(*link)->left : &(*link)->right;
    }
    if (!*link) return false;
    ++visited_;
    Node* n = *link;
    if (n->left && n->right) {                   // 2 con: thay bang khoa lien truoc (giong B-Tree)
        Node** pl = &n->left;
        while ((*pl)->right) { ++visited_; pl = &(*pl)->right; }
        Node* pred = *pl;
        n->key = pred->key;
        *pl = pred->left;
        delete pred;
    } else {
        *link = n->left ? n->left : n->right;
        delete n;
    }
    --size_;
    return true;
}

std::vector<int> BST::traverse() const {
    std::vector<int> out;
    std::vector<const Node*> st;
    const Node* cur = root_;
    while (cur || !st.empty()) {
        while (cur) { st.push_back(cur); cur = cur->left; }
        cur = st.back(); st.pop_back();
        out.push_back(cur->key);
        cur = cur->right;
    }
    return out;
}

int BST::height() const {
    if (!root_) return -1;
    std::vector<std::pair<const Node*, int>> st{{root_, 0}};
    int h = 0;
    while (!st.empty()) {
        auto [n, d] = st.back(); st.pop_back();
        if (d > h) h = d;
        if (n->left) st.push_back({n->left, d + 1});
        if (n->right) st.push_back({n->right, d + 1});
    }
    return h;
}
