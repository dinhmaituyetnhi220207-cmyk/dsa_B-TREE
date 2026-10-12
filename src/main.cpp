// main.cpp - chuong trinh demo. Tuan 1 chi kiem tra khung chuong trinh bien dich duoc.
#include "btree.h"
#include <iostream>

int main() {
    BTree tree(5);
    std::cout << "B-Tree bac " << tree.order() << " - khung chuong trinh Tuan 1\n";
    std::cout << "Chieu cao cay rong: " << tree.height() << "\n";
    std::cout << "Cac thao tac search/insert/remove se duoc cai dat o Tuan 2.\n";
    return 0;
}
