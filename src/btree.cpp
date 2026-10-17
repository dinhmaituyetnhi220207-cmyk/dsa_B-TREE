// btree.cpp - cai dat B-Tree bac m (Tuan 2).
#include "btree.h"
#include <algorithm>
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

// Vi tri i dau tien co keys[i] >= key (tim nhi phan trong mot nut).
std::size_t BTree::lowerIndex(const Node* n, int key) {
    return static_cast<std::size_t>(std::lower_bound(n->keys.begin(), n->keys.end(), key) - n->keys.begin());
}

// ---------------------------------------------------------------- SEARCH
bool BTree::search(int key, std::vector<std::vector<int>>* path) const {
    const Node* cur = root_;
    while (cur) {
        ++visited_;                                   // moi nut tham = 1 lan doc dia
        if (path) path->push_back(cur->keys);
        std::size_t i = lowerIndex(cur, key);
        if (i < cur->keys.size() && cur->keys[i] == key) return true;
        if (cur->isLeaf()) return false;
        cur = cur->children[i];
    }
    return false;
}

// ---------------------------------------------------------------- INSERT
// Nut co m khoa: giu m/2 khoa dau, day khoa thu m/2 len cha, phan con lai sang nut moi.
// Voi m = 5: [a b c d e] -> trai [a b], len c, phai [d e].
BTree::Split BTree::splitNode(Node* node) {
    std::size_t mid = static_cast<std::size_t>(m_ / 2);
    Split s;
    s.happened = true;
    s.median = node->keys[mid];
    s.right = new Node();
    s.right->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
    node->keys.resize(mid);
    if (!node->isLeaf()) {
        s.right->children.assign(node->children.begin() + mid + 1, node->children.end());
        node->children.resize(mid + 1);
    }
    return s;
}

BTree::Split BTree::insertRec(Node* node, int key, bool& inserted) {
    ++visited_;
    std::size_t i = lowerIndex(node, key);
    if (i < node->keys.size() && node->keys[i] == key) {   // khoa trung
        inserted = false;
        return Split();
    }
    if (node->isLeaf()) {
        node->keys.insert(node->keys.begin() + i, key);
        inserted = true;
    } else {
        Split child = insertRec(node->children[i], key, inserted);
        if (child.happened) {                               // con bi tach: nhan khoa giua
            node->keys.insert(node->keys.begin() + i, child.median);
            node->children.insert(node->children.begin() + i + 1, child.right);
        }
    }
    if (node->keys.size() == static_cast<std::size_t>(m_)) return splitNode(node);   // tran
    return Split();
}

bool BTree::insert(int key) {
    if (!root_) {
        root_ = new Node();
        root_->keys.push_back(key);
        return true;
    }
    bool inserted = false;
    Split s = insertRec(root_, key, inserted);
    if (s.happened) {                                       // goc bi tach: cay cao them 1
        Node* newRoot = new Node();
        newRoot->keys.push_back(s.median);
        newRoot->children.push_back(root_);
        newRoot->children.push_back(s.right);
        root_ = newRoot;
    }
    return inserted;
}

// ---------------------------------------------------------------- REMOVE
// Gop con i+1 vao con i cung khoa phan cach parent->keys[i].
void BTree::mergeChildren(Node* parent, std::size_t i) {
    Node* left = parent->children[i];
    Node* right = parent->children[i + 1];
    left->keys.push_back(parent->keys[i]);
    left->keys.insert(left->keys.end(), right->keys.begin(), right->keys.end());
    left->children.insert(left->children.end(), right->children.begin(), right->children.end());
    parent->keys.erase(parent->keys.begin() + i);
    parent->children.erase(parent->children.begin() + i + 1);
    right->children.clear();
    delete right;
}

// Neu con i thieu khoa: muon trai -> muon phai -> gop.
void BTree::fixChild(Node* parent, std::size_t i) {
    Node* c = parent->children[i];
    if (c->keys.size() >= minKeys()) return;

    if (i > 0 && parent->children[i - 1]->keys.size() > minKeys()) {           // muon tu anh em trai
        Node* left = parent->children[i - 1];
        c->keys.insert(c->keys.begin(), parent->keys[i - 1]);
        parent->keys[i - 1] = left->keys.back();
        left->keys.pop_back();
        if (!left->isLeaf()) {
            c->children.insert(c->children.begin(), left->children.back());
            left->children.pop_back();
        }
    } else if (i + 1 < parent->children.size() && parent->children[i + 1]->keys.size() > minKeys()) {  // muon tu anh em phai
        Node* right = parent->children[i + 1];
        c->keys.push_back(parent->keys[i]);
        parent->keys[i] = right->keys.front();
        right->keys.erase(right->keys.begin());
        if (!right->isLeaf()) {
            c->children.push_back(right->children.front());
            right->children.erase(right->children.begin());
        }
    } else if (i > 0) {
        mergeChildren(parent, i - 1);                                            // gop voi anh em trai
    } else {
        mergeChildren(parent, i);                                                // gop voi anh em phai
    }
}

bool BTree::removeRec(Node* node, int key) {
    ++visited_;
    std::size_t i = lowerIndex(node, key);
    bool here = i < node->keys.size() && node->keys[i] == key;

    if (here && node->isLeaf()) {
        node->keys.erase(node->keys.begin() + i);
        return true;
    }
    if (here) {                                   // khoa o nut trong: thay bang khoa lien truoc
        const Node* p = node->children[i];
        while (!p->isLeaf()) p = p->children.back();
        int pred = p->keys.back();
        node->keys[i] = pred;
        removeRec(node->children[i], pred);
        fixChild(node, i);
        return true;
    }
    if (node->isLeaf()) return false;             // khong tim thay
    bool removed = removeRec(node->children[i], key);
    fixChild(node, i);
    return removed;
}

bool BTree::remove(int key) {
    if (!root_) return false;
    bool removed = removeRec(root_, key);
    if (root_->keys.empty()) {                    // goc het khoa
        Node* old = root_;
        root_ = old->isLeaf() ? nullptr : old->children[0];   // cay thap di 1 (hoac rong)
        old->children.clear();
        delete old;
    }
    return removed;
}

// ---------------------------------------------------------------- TRAVERSE, HEIGHT, LEVELS
void BTree::inOrderHelper(const Node* node, std::vector<int>& result) const {
    if (!node) return;
    for (std::size_t i = 0; i < node->keys.size(); ++i) {
        if (!node->isLeaf()) inOrderHelper(node->children[i], result);
        result.push_back(node->keys[i]);
    }
    if (!node->isLeaf()) inOrderHelper(node->children.back(), result);
}

std::vector<int> BTree::traverse() const {
    std::vector<int> result;
    inOrderHelper(root_, result);
    return result;
}

int BTree::height() const {
    if (!root_) return -1;
    int h = 0;
    for (const Node* n = root_; !n->isLeaf(); n = n->children[0]) ++h;
    return h;
}

std::string BTree::levels() const {
    if (!root_) return "";
    std::string out;
    std::vector<const Node*> level{root_};
    while (!level.empty()) {
        std::vector<const Node*> next;
        for (std::size_t j = 0; j < level.size(); ++j) {
            out += (j ? " [" : "[");
            for (std::size_t k = 0; k < level[j]->keys.size(); ++k)
                out += (k ? " " : "") + std::to_string(level[j]->keys[k]);
            out += "]";
            for (const Node* c : level[j]->children) next.push_back(c);
        }
        if (!next.empty()) out += " | ";
        level = next;
    }
    return out;
}

// ---------------------------------------------------------------- VALIDATE (5 tinh chat)
bool BTree::validateRec(const Node* n, bool isRoot, const long long* lo, const long long* hi,
                        int depth, int& leafDepth, std::string& why) const {
    // (2) so khoa toi da / toi thieu
    if (n->keys.size() > static_cast<std::size_t>(m_ - 1)) { why = "node has more than m-1 keys"; return false; }
    if (isRoot ? n->keys.empty() : n->keys.size() < minKeys()) { why = "node has too few keys"; return false; }
    for (std::size_t i = 0; i < n->keys.size(); ++i) {
        // (1) tang dan trong nut, (5) nam dung khoang cua nut cha
        if (i > 0 && n->keys[i] <= n->keys[i - 1]) { why = "keys in a node are not strictly increasing"; return false; }
        if ((lo && n->keys[i] <= *lo) || (hi && n->keys[i] >= *hi)) { why = "key outside the range given by its parent"; return false; }
    }
    if (n->isLeaf()) {
        // (4) moi la cung do sau
        if (leafDepth < 0) leafDepth = depth;
        else if (leafDepth != depth) { why = "leaves are not all at the same depth"; return false; }
        return true;
    }
    // (3) k khoa thi k+1 con
    if (n->children.size() != n->keys.size() + 1) { why = "internal node does not have keys+1 children"; return false; }
    for (std::size_t i = 0; i < n->children.size(); ++i) {
        long long a = i > 0 ? n->keys[i - 1] : 0, b = i < n->keys.size() ? n->keys[i] : 0;
        const long long* L = i > 0 ? &a : lo;
        const long long* H = i < n->keys.size() ? &b : hi;
        if (!validateRec(n->children[i], false, L, H, depth + 1, leafDepth, why)) return false;
    }
    return true;
}

bool BTree::validate(std::string* error) const {
    if (!root_) return true;                      // cay rong hop le
    int leafDepth = -1;
    std::string why;
    bool ok = validateRec(root_, true, nullptr, nullptr, 0, leafDepth, why);
    if (!ok && error) *error = why;
    return ok;
}
