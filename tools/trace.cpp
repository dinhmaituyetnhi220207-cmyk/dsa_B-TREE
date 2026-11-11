// trace.cpp - in cay sau TUNG thao tac, de doi chieu voi ban chay tay.
// Cach dung:  ./trace <m> <thao tac> ...
//   +k  chen k      -k  xoa k      ?k  tim k (in so nut tham)
// Vi du:      ./trace 4 +10 +20 +30 +40 -20
//             ./trace 5 +1 +2 +3 +4 +5 -5        (truong hop E9)
// Nen chay tay TRUOC tren giay, roi moi dung trace de kiem tra.
#include "btree.h"
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Cach dung: trace <m> +k | -k | ?k ...\n";
        return 1;
    }
    BTree t(std::atoi(argv[1]));
    std::cout << "B-Tree bac m = " << t.order() << "\n";
    for (int i = 2; i < argc; ++i) {
        std::string a = argv[i];
        if (a.size() < 2 || (a[0] != '+' && a[0] != '-' && a[0] != '?')) {
            std::cerr << "Bo qua tham so khong hop le: " << a << "\n";
            continue;
        }
        int k = std::atoi(a.c_str() + 1);
        t.resetCounters();
        std::string what;
        bool ok;
        if (a[0] == '+')      { ok = t.insert(k); what = "chen " + std::to_string(k); }
        else if (a[0] == '-') { ok = t.remove(k); what = "xoa  " + std::to_string(k); }
        else                  { ok = t.search(k); what = "tim  " + std::to_string(k); }
        std::cout << what << (ok ? "" : " (khong doi)") << "\n    " << t.levels() << "\n    h = " << t.height();
        if (t.splits())  std::cout << ", tach " << t.splits();
        if (t.borrows()) std::cout << ", muon " << t.borrows();
        if (t.merges())  std::cout << ", gop " << t.merges();
        if (a[0] == '?') std::cout << ", tham " << t.nodesVisited() << " nut";
        std::string err;
        if (!t.validate(&err)) std::cout << "  VI PHAM: " << err;
        std::cout << "\n";
    }
    return 0;
}
