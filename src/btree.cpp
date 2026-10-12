// btree.cpp - Tuan 1: khung chuong trinh. Cac ham TODO se duoc cai dat o Tuan 2.
#include "btree.h"
#include <stdexcept>

BTree::BTree(int order) : m_(order) {
    if (order < 3) throw std::invalid_argument("B-Tree order must be >= 3");
}

BTree::~BTree() { destroy(root_); }

void BTree::destroy(Node* node) {
    if (!node) return;
    for (Node* child : node->children) destroy(child);
    delete node;
}

bool BTree::search(int /*key*/, std::vector<std::vector<int>>* /*path*/) const {
    // TODO (Tuan 2): di tu goc xuong la, moi nut tim vi tri i dau tien co keys[i] >= key.
    return false;
}

bool BTree::insert(int /*key*/) {
    // TODO (Tuan 2): chen vao la; neu nut co m khoa thi tach, day khoa giua len cha.
    return false;
}

bool BTree::remove(int /*key*/) {
    // TODO (Tuan 2): xoa o la (hoac thay bang khoa lien truoc), roi muon / gop neu thieu khoa.
    return false;
}

std::vector<int> BTree::traverse() const {
    // TODO (Tuan 2): duyet in-order.
    return {};
}

bool BTree::validate(std::string* error) const {
    // TODO (Tuan 2): kiem tra 5 tinh chat trong docs/week1/test-plan.md.
    if (error) *error = "validate() chua duoc cai dat (Tuan 2)";
    return root_ == nullptr;
}

int BTree::height() const {
    if (!root_) return -1;
    int h = 0;
    for (const Node* n = root_; !n->isLeaf(); n = n->children[0]) ++h;
    return h;
}
