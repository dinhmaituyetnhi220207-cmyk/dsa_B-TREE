// stress_btree.cpp - kiem tra ap luc: chen/xoa ngau nhien, doi chieu std::set, validate sau moi thao tac.
// Bien dich: g++ -std=c++17 -O2 -Isrc -o stress_btree tests/stress_btree.cpp src/btree.cpp
#include "btree.h"
#include <iostream>
#include <random>
#include <set>

int main() {
    std::mt19937 rng(99);
    long ops = 0;
    for (int m = 3; m <= 9; ++m) {
        for (int round = 0; round < 30; ++round) {
            BTree t(m);
            std::set<int> ref;
            for (int i = 0; i < 3000; ++i) {
                int k = static_cast<int>(rng() % 500);
                bool doInsert = rng() % 2;
                bool got  = doInsert ? t.insert(k) : t.remove(k);
                bool want = doInsert ? ref.insert(k).second : ref.erase(k) > 0;
                std::string err;
                if (got != want || !t.validate(&err) ||
                    t.traverse() != std::vector<int>(ref.begin(), ref.end())) {
                    std::cout << "LOI: m=" << m << ", thao tac " << i << " " << err << "\n";
                    return 1;
                }
                ++ops;
            }
        }
    }
    std::cout << "Stress OK: " << ops << " thao tac ngau nhien, m = 3..9, validate dung sau moi thao tac\n";
    return 0;
}
