#include <iostream>
#include "btree.h"

int main() {
    BTree tree(3); // Khởi tạo B-Tree bậc m = 3

    // Chèn các phần tử test
    int keys[] = {10, 20, 5, 6, 12, 30, 7};
    for (int k : keys) {
        tree.insert(k);
    }

    // Duyệt In-order
    std::cout << "Duyet cay In-order: ";
    for (int val : tree.traverse()) {
        std::cout << val << " ";
    }
    std::cout << "\nChieu cao cay: " << tree.height() << std::endl;

    // Test tim kiem
    std::cout << "Tim 12: " << (tree.search(12) ? "Found" : "Not Found") << std::endl;
    std::cout << "So node da truy cap (I/O): " << tree.nodesVisited() << std::endl;

    return 0;
}