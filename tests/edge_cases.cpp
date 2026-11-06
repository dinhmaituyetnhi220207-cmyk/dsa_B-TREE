// edge_cases.cpp - danh sach truong hop bien va dau vao "kho" (Tuan 3).
// Moi truong hop in: ma, mo ta, ket qua mong doi, ket qua thuc te, PASS/FAIL.
// Bien dich: g++ -std=c++17 -O2 -Wall -Wextra -Isrc -o edge_cases tests/edge_cases.cpp src/btree.cpp src/bst.cpp
#include "btree.h"
#include "bst.h"
#include <algorithm>
#include <climits>
#include <iostream>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

static int g_pass = 0, g_fail = 0;
static void report(const std::string& id, const std::string& what, const std::string& want,
                   const std::string& got, bool ok) {
    (ok ? g_pass : g_fail)++;
    std::cout << "| " << id << " | " << what << " | " << want << " | " << got << " | "
              << (ok ? "PASS" : "**FAIL**") << " |\n";
}
static std::vector<int> range(int a, int b) { std::vector<int> v; for (int i = a; i <= b; ++i) v.push_back(i); return v; }

int main() {
    std::cout << "| Ma | Truong hop | Mong doi | Thuc te | KQ |\n|---|---|---|---|---|\n";

    {   // E1 cay rong
        BTree t(5); std::ostringstream g;
        bool s = t.search(7), r = t.remove(7);
        g << "search=" << s << ", remove=" << r << ", h=" << t.height() << ", size=" << t.traverse().size()
          << ", valid=" << t.validate();
        report("E1", "Cay rong: search, remove, height, traverse, validate",
               "0, 0, -1, 0, 1", g.str(), !s && !r && t.height() == -1 && t.traverse().empty() && t.validate());
    }
    {   // E2 mot khoa roi xoa
        BTree t(5); t.insert(42); bool r = t.remove(42);
        std::ostringstream g; g << "remove=" << r << ", h=" << t.height() << ", search=" << t.search(42);
        report("E2", "Chen 1 khoa roi xoa no -> cay rong lai", "remove=1, h=-1, search=0", g.str(),
               r && t.height() == -1 && !t.search(42));
    }
    {   // E3 khoa trung lap lai nhieu lan
        BTree t(5); int ok = 0; for (int i = 0; i < 100000; ++i) ok += t.insert(7);
        std::ostringstream g; g << "insert true " << ok << " lan, size=" << t.traverse().size();
        report("E3", "Chen cung mot khoa 100.000 lan", "true 1 lan, size=1", g.str(),
               ok == 1 && t.traverse().size() == 1 && t.validate());
    }
    {   // E4 khoa cuc tri cua int
        BTree t(3); std::vector<int> ks = {INT_MAX, INT_MIN, 0, -1, 1, INT_MAX - 1, INT_MIN + 1};
        for (int k : ks) t.insert(k);
        bool all = true; for (int k : ks) all = all && t.search(k);
        std::vector<int> want = ks; std::sort(want.begin(), want.end());
        bool ok = all && t.traverse() == want && t.validate();
        report("E4", "Khoa INT_MIN, INT_MAX, 0, -1, 1 (m = 3)", "tim thay het, thu tu dung, valid",
               ok ? "tim thay het, thu tu dung, valid" : "sai", ok);
    }
    {   // E5 bac khong hop le
        int thrown = 0;
        for (int m : {2, 1, 0, -5}) { try { BTree t(m); } catch (const std::invalid_argument&) { ++thrown; } }
        report("E5", "m = 2, 1, 0, -5 (khong hop le)", "nem invalid_argument 4/4",
               "nem " + std::to_string(thrown) + "/4", thrown == 4);
    }
    {   // E6 m = 3 (nho nhat) va m chan, du lieu tang/giam, validate sau MOI lan chen
        bool ok = true; std::string why;
        for (int m : {3, 4, 6})
            for (int dir = 0; dir < 2 && ok; ++dir) {
                BTree t(m);
                for (int i = 0; i < 5000 && ok; ++i) { t.insert(dir ? 5000 - i : i); ok = t.validate(&why); }
            }
        report("E6", "m = 3, 4, 6; chen 5.000 khoa tang roi giam, validate sau moi lan chen",
               "luon valid", ok ? "luon valid" : why, ok);
    }
    {   // E7 xoa het theo 3 thu tu, roi chen lai
        bool ok = true;
        for (int mode = 0; mode < 3 && ok; ++mode) {
            BTree t(5); auto ks = range(1, 20000); for (int k : ks) t.insert(k);
            if (mode == 1) std::reverse(ks.begin(), ks.end());
            if (mode == 2) { std::mt19937 r(1); std::shuffle(ks.begin(), ks.end(), r); }
            for (int k : ks) ok = ok && t.remove(k);
            ok = ok && t.height() == -1 && t.validate();
            for (int k : range(1, 100)) t.insert(k);
            ok = ok && t.traverse() == range(1, 100) && t.validate();
        }
        report("E7", "Xoa het 20.000 khoa (tang / giam / ngau nhien) roi chen lai 100 khoa",
               "cay rong, h = -1, chen lai dung", ok ? "dung ca 3 thu tu" : "sai", ok);
    }
    {   // E8 xoa khoa khong ton tai: nho hon min, lon hon max, nam giua hai khoa
        BTree t(5); for (int i = 0; i < 1000; ++i) t.insert(2 * i);
        auto before = t.levels();
        bool r = t.remove(-1) || t.remove(5000) || t.remove(501);
        report("E8", "Xoa -1, 5000, 501 (khong co) tren cay 1.000 khoa chan", "false, cay khong doi",
               std::string(r ? "true" : "false") + (t.levels() == before ? ", cay khong doi" : ", cay DOI"),
               !r && t.levels() == before);
    }
    {   // E9 dau vao doi khang: chen/xoa lien tuc dung o nguong tach-gop
        BTree t(5); for (int k : {1, 2, 3, 4}) t.insert(k);    // goc la la day (4 khoa)
        t.resetCounters();
        const int R = 100000;
        for (int i = 0; i < R; ++i) { t.insert(5); t.remove(5); }   // chen 5 -> tach goc; xoa 5 -> gop, cay thap lai
        std::ostringstream g; g << t.splits() << " tach, " << t.merges() << " gop, h=" << t.height();
        report("E9", "Doi khang: lap 100.000 lan {chen 5; xoa 5} tren cay [1 2 3 4], m = 5",
               "100000 tach, 100000 gop, h=0", g.str(),
               t.splits() == R && t.merges() == R && t.height() == 0 && t.validate());
    }
    {   // E10 doi khang voi cay lon: tim khoa gay tach day chuyen nhieu tang nhat, roi lap {chen; xoa} khoa do
        // (chen tang dan khong dung duoc: m = 3 tang dan cho moi nut dung 1 khoa, khong nut nao day)
        BTree t(3); std::vector<int> ev; for (int i = 0; i < 2047; ++i) ev.push_back(2 * i);
        std::mt19937 rr(12); std::shuffle(ev.begin(), ev.end(), rr);
        for (int k : ev) t.insert(k);                                  // m = 3, khoa chan, thu tu ngau nhien
        int h0 = t.height(), worstKey = 1; long worst = -1;
        for (int k = 1; k < 4094; k += 2) {                           // thu moi khe (khoa le)
            t.resetCounters(); t.insert(k);
            if (t.splits() > worst) { worst = t.splits(); worstKey = k; }
            t.remove(k);
        }
        t.resetCounters();
        const int R = 10000; int hMaxSeen = h0;
        for (int i = 0; i < R; ++i) { t.insert(worstKey); hMaxSeen = std::max(hMaxSeen, t.height()); t.remove(worstKey); }
        std::ostringstream g; g << "khoa " << worstKey << ": " << double(t.splits()) / R << " tach/lan chen, "
                                << double(t.merges()) / R << " gop/lan xoa; h " << h0 << "->" << hMaxSeen;
        report("E10", "Doi khang: m = 3, 2.047 khoa chan ngau nhien; tim khoa le gay tach nhieu nhat, lap {chen; xoa} 10.000 lan",
               "tach day chuyen nhieu tang; cay van valid", g.str() + " (lan thu dau: " + std::to_string(worst) + " tach)",
               worst >= 2 && t.validate());
    }
    {   // E11 duyet tuan tu (cai tien Tuan 3) cho cung cay va cung so nut tham voi tim nhi phan
        std::mt19937 r(5); std::vector<int> ks; for (int i = 0; i < 100000; ++i) ks.push_back(int(r() % 1000000));
        bool ok = true;
        for (int m : {3, 5, 16, 64}) {
            BTree a(m), b(m); b.setNodeSearch(BTree::NodeSearch::Linear);
            for (std::size_t i = 0; i < ks.size(); ++i) {
                if (i % 3 == 2) { a.remove(ks[i - 1]); b.remove(ks[i - 1]); }
                else { a.insert(ks[i]); b.insert(ks[i]); }
            }
            a.resetVisited(); b.resetVisited();
            for (int k = 0; k < 1000000; k += 997) { a.search(k); b.search(k); }
            ok = ok && a.levels() == b.levels() && a.nodesVisited() == b.nodesVisited() && b.validate();
        }
        report("E11", "Che do Linear vs Binary, 100.000 thao tac tron, m = 3, 5, 16, 64",
               "cung cay, cung so nut tham", ok ? "cung cay, cung so nut tham" : "khac", ok);
    }
    {   // E12 m rat lon: toan bo nam trong mot nut
        BTree t(100001); for (int i = 0; i < 100000; ++i) t.insert(i);
        std::ostringstream g; g << "h=" << t.height();
        t.resetVisited(); t.search(77777); g << ", tham " << t.nodesVisited() << " nut";
        report("E12", "m = 100.001, chen 100.000 khoa", "h=0, tham 1 nut", g.str(),
               t.height() == 0 && t.nodesVisited() == 1 && t.validate());
    }
    {   // E13 BST doi chieu std::set (dam bao so sanh Tuan 3 la cong bang)
        std::mt19937 r(9); BST b; std::set<int> ref; bool ok = true;
        for (int i = 0; i < 200000 && ok; ++i) {
            int k = int(r() % 5000);
            ok = (r() % 2) ? b.insert(k) == ref.insert(k).second : b.remove(k) == (ref.erase(k) > 0);
        }
        ok = ok && b.traverse() == std::vector<int>(ref.begin(), ref.end()) && b.size() == long(ref.size());
        report("E13", "BST: 200.000 chen/xoa ngau nhien, doi chieu std::set", "khop", ok ? "khop" : "lech", ok);
    }
    {   // E14 BST suy bien
        BST b; for (int i = 0; i < 20000; ++i) b.insert(i);
        b.resetVisited(); b.search(19999);
        std::ostringstream g; g << "h=" << b.height() << ", tim 19999 tham " << b.nodesVisited() << " nut";
        report("E14", "BST chen 0..19999 tang dan (khong de quy nen khong tran stack)",
               "h=19999, tham 20000 nut", g.str(), b.height() == 19999 && b.nodesVisited() == 20000);
    }

    std::cout << "\nKet qua: " << g_pass << " PASS, " << g_fail << " FAIL\n";
    return g_fail ? 1 : 0;
}
