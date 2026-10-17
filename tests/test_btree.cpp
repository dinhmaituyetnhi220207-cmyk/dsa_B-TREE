// test_btree.cpp - unit test T1-T16 (docs/week1/test-plan.md).
// Ket qua mong doi duoc tinh tay o Tuan 1 theo quy uoc Muc 3.6 cua bao cao.
// Bien dich: g++ -std=c++17 -Wall -Wextra -Isrc -o test_btree tests/test_btree.cpp src/btree.cpp
#include "btree.h"
#include <algorithm>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>

static int g_pass = 0, g_fail = 0;

template <typename A, typename B>
static void check(const std::string& id, const A& got, const B& want) {
    if (got == want) { ++g_pass; std::cout << "  [PASS] " << id << "\n"; }
    else { ++g_fail; std::cout << "  [FAIL] " << id << "\n         got:  " << got << "\n         want: " << want << "\n"; }
}
static std::string str(const std::vector<int>& v) {
    std::string s = "{";
    for (std::size_t i = 0; i < v.size(); ++i) s += (i ? "," : "") + std::to_string(v[i]);
    return s + "}";
}
static std::string pathStr(const std::vector<std::vector<int>>& p) {
    std::string s;
    for (std::size_t i = 0; i < p.size(); ++i) s += (i ? " -> " : "") + str(p[i]);
    return s;
}
static void insertAll(BTree& t, const std::vector<int>& keys) { for (int k : keys) t.insert(k); }

// Thu tu chen nay tao ra dung cay o Hinh 1 (B-Tree bac 5, 19 khoa).
static const std::vector<int> FIG1_ORDER = {70, 260, 40, 110, 100, 10, 180, 20, 140, 190,
                                            160, 35, 130, 50, 120, 65, 80, 240, 90};
static const std::string FIG1 =
    "[100] | [35 65] [130 180] | [10 20] [40 50] [70 80 90] [110 120] [140 160] [190 240 260]";
static void buildFig1(BTree& t) { insertAll(t, FIG1_ORDER); }

int main() {
    std::cout << "=== Unit test B-Tree bac 5 ===\n";

    {   std::cout << "T0 Dung cay Hinh 1\n";
        BTree t(5); buildFig1(t);
        check("T0 levels", t.levels(), FIG1);
        check("T0 validate", t.validate(), true); }

    {   std::cout << "T1 Cay rong\n";
        BTree t(5);
        check("T1 search(5)", t.search(5), false);
        check("T1 traverse", str(t.traverse()), std::string("{}"));
        check("T1 height", t.height(), -1);
        check("T1 validate", t.validate(), true); }

    {   std::cout << "T2 Chen chua day\n";
        BTree t(5); insertAll(t, {10, 20, 30, 40});
        check("T2 levels", t.levels(), std::string("[10 20 30 40]"));
        check("T2 height", t.height(), 0); }

    {   std::cout << "T3 Tach goc\n";
        BTree t(5); insertAll(t, {10, 20, 30, 40, 50});
        check("T3 levels", t.levels(), std::string("[30] | [10 20] [40 50]"));
        check("T3 height", t.height(), 1);
        check("T3 validate", t.validate(), true); }

    {   std::cout << "T4 Khop chay tay Tuan 1\n";
        BTree t(5); insertAll(t, {20, 40, 10, 30, 15, 35, 45, 50});
        check("T4 levels", t.levels(), std::string("[20 40] | [10 15] [30 35] [45 50]"));
        check("T4 validate", t.validate(), true); }

    {   std::cout << "T5 Khoa trung\n";
        BTree t(5); insertAll(t, {20, 40, 10, 30, 15, 35, 45, 50});
        std::string before = t.levels();
        check("T5 insert(35)", t.insert(35), false);
        check("T5 cay khong doi", t.levels(), before); }

    {   std::cout << "T6 Tim o goc\n";
        BTree t(5); buildFig1(t); t.resetVisited();
        check("T6 search(100)", t.search(100), true);
        check("T6 so nut tham", t.nodesVisited(), 1L); }

    {   std::cout << "T7 Tim o la\n";
        BTree t(5); buildFig1(t);
        std::vector<std::vector<int>> path;
        check("T7 search(120)", t.search(120, &path), true);
        check("T7 duong di", pathStr(path), std::string("{100} -> {130,180} -> {110,120}")); }

    {   std::cout << "T8 Khoa khong ton tai\n";
        BTree t(5); buildFig1(t);
        for (int k : {125, 5, 300}) {
            t.resetVisited();
            check("T8 search(" + std::to_string(k) + ")", t.search(k), false);
            check("T8 so nut tham (" + std::to_string(k) + ")", t.nodesVisited(), 3L);
        } }

    {   std::cout << "T9 Duyet theo thu tu\n";
        BTree t(5); buildFig1(t);
        std::vector<int> want = FIG1_ORDER; std::sort(want.begin(), want.end());
        check("T9 traverse", str(t.traverse()), str(want)); }

    {   std::cout << "T10 Xoa o la con du\n";
        BTree t(5); buildFig1(t);
        check("T10 remove(260)", t.remove(260), true);
        check("T10 levels", t.levels(), std::string(
            "[100] | [35 65] [130 180] | [10 20] [40 50] [70 80 90] [110 120] [140 160] [190 240]"));
        check("T10 validate", t.validate(), true); }

    {   std::cout << "T11 Xoa phai muon khoa\n";
        BTree t(5); buildFig1(t);
        check("T11 remove(40)", t.remove(40), true);
        check("T11 levels", t.levels(), std::string(
            "[100] | [35 70] [130 180] | [10 20] [50 65] [80 90] [110 120] [140 160] [190 240 260]"));
        check("T11 validate", t.validate(), true); }

    {   std::cout << "T12 Xoa o nut trong\n";
        BTree t(5); buildFig1(t);
        check("T12 remove(100)", t.remove(100), true);
        check("T12 levels", t.levels(), std::string(
            "[90] | [35 65] [130 180] | [10 20] [40 50] [70 80] [110 120] [140 160] [190 240 260]"));
        check("T12 validate", t.validate(), true); }

    {   std::cout << "T13 Gop day chuyen, cay thap di\n";
        BTree t(5); buildFig1(t);
        check("T13 remove(110)", t.remove(110), true);
        check("T13 levels", t.levels(), std::string(
            "[35 65 100 180] | [10 20] [40 50] [70 80 90] [120 130 140 160] [190 240 260]"));
        check("T13 height", t.height(), 1);
        check("T13 validate", t.validate(), true); }

    {   std::cout << "T14 Xoa khoa khong co\n";
        BTree t(5); buildFig1(t);
        check("T14 remove(125)", t.remove(125), false);
        check("T14 cay khong doi", t.levels(), FIG1); }

    {   std::cout << "T15 Du lieu tang dan 1..1000\n";
        BTree t(5); bool allValid = true;
        for (int k = 1; k <= 1000; ++k) { t.insert(k); allValid = allValid && t.validate(); }
        check("T15 validate sau moi lan chen", allValid, true);
        check("T15 4 <= h <= 5", t.height() >= 4 && t.height() <= 5, true); }

    {   std::cout << "T16 Ngau nhien, doi chieu std::set\n";
        std::mt19937 rng(2026);
        std::vector<int> pool(10000); for (int i = 0; i < 10000; ++i) pool[i] = i;
        std::shuffle(pool.begin(), pool.end(), rng);
        std::vector<int> keys(pool.begin(), pool.begin() + 300);
        BTree t(5); std::set<int> ref; bool allValid = true;
        for (int k : keys) { t.insert(k); ref.insert(k); allValid = allValid && t.validate(); }
        std::shuffle(keys.begin(), keys.end(), rng);
        for (int i = 0; i < 200; ++i) { t.remove(keys[i]); ref.erase(keys[i]); allValid = allValid && t.validate(); }
        check("T16 validate sau moi thao tac", allValid, true);
        check("T16 traverse == std::set", str(t.traverse()), str(std::vector<int>(ref.begin(), ref.end()))); }

    std::cout << "\nKet qua: " << g_pass << " passed, " << g_fail << " failed\n";
    return g_fail == 0 ? 0 : 1;
}
