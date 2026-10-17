// main.cpp - demo: chay lai vi du chay tay Tuan 1 (B-Tree bac 5).
#include "btree.h"
#include <iostream>

int main() {
    BTree tree(5);
    std::cout << "Chen lan luot 20 40 10 30 15 35 45 50 vao B-Tree bac " << tree.order() << ":\n";
    for (int k : {20, 40, 10, 30, 15, 35, 45, 50}) {
        tree.insert(k);
        std::cout << "  chen " << k << ":\t" << tree.levels() << "\n";
    }
    std::cout << "Chieu cao: " << tree.height() << "\n";

    std::vector<std::vector<int>> path;
    tree.resetVisited();
    bool found = tree.search(35, &path);
    std::cout << "search(35): " << (found ? "tim thay" : "khong thay") << ", tham " << tree.nodesVisited() << " nut\n";

    tree.remove(35);
    std::cout << "Sau remove(35):\t" << tree.levels() << "\n";
    std::string err;
    std::cout << "validate(): " << (tree.validate(&err) ? "hop le" : "VI PHAM: " + err) << "\n";
    return 0;
}
