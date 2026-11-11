// bench.cpp - bo thuc nghiem Tuan 3 (benchmark harness).
// Bien dich: g++ -std=c++17 -O2 -DNDEBUG -Isrc -o bench_btree bench/bench.cpp src/btree.cpp src/bst.cpp
// Chay:      ./bench_btree [all|height|bst|opt|msweep] [--quick]     -> ghi CSV vao results/ (--quick: results/quick/)
//
// Mo hinh "doc dia": moi nut duoc tham = 1 lan doc mot trang dia (nodesVisited()).
// Khoa duoc dung: so chan 0, 2, 4, ..., 2(n-1)  -> tim khoa le de do tim KHONG thanh cong.
#include "btree.h"
#include "bst.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <vector>

using Clock = std::chrono::steady_clock;
static double msSince(Clock::time_point t0) {
    return std::chrono::duration<double, std::milli>(Clock::now() - t0).count();
}
static volatile long g_sink = 0;
static std::string g_out = "results";   // --quick ghi vao results/quick de khong de len so lieu chinh thuc   // chong trinh bien dich bo vong lap do

// ------------------------------------------------------------------ du lieu dau vao
enum class Order { Random, Asc, Desc, ZigZag };
static const char* name(Order o) {
    switch (o) { case Order::Random: return "random"; case Order::Asc: return "asc";
                 case Order::Desc: return "desc"; default: return "zigzag"; }
}
static std::vector<int> makeKeys(long n, Order o, unsigned seed) {
    std::vector<int> v(static_cast<std::size_t>(n));
    for (long i = 0; i < n; ++i) v[static_cast<std::size_t>(i)] = static_cast<int>(2 * i);
    if (o == Order::Random) { std::mt19937 rng(seed); std::shuffle(v.begin(), v.end(), rng); }
    else if (o == Order::Desc) std::reverse(v.begin(), v.end());
    else if (o == Order::ZigZag) {                      // nho, lon, nho, lon ... (0, max, 2, max-2, ...)
        std::vector<int> z; z.reserve(v.size());
        std::size_t lo = 0, hi = v.size();
        while (lo < hi) { z.push_back(v[lo++]); if (lo < hi) z.push_back(v[--hi]); }
        v.swap(z);
    }
    return v;
}
static std::vector<int> sample(const std::vector<int>& keys, long q, unsigned seed, bool miss) {
    std::mt19937 rng(seed);
    std::vector<int> out(static_cast<std::size_t>(q));
    for (auto& x : out) x = keys[rng() % keys.size()] + (miss ? 1 : 0);   // +1 -> so le -> khong co
    return out;
}

// ------------------------------------------------------------------ can ly thuyet chieu cao
// h_min = ceil(log_m(n+1)) - 1 : nho nhat h sao cho m^(h+1) - 1 >= n
// h_max = floor(log_t((n+1)/2)): lon nhat h sao cho 2 t^h - 1 <= n, t = ceil(m/2)
static int hMin(long n, int m) { int h = 0; long double cap = m; while (cap - 1 < n) { cap *= m; ++h; } return h; }
static int hMax(long n, int m) { int t = (m + 1) / 2, h = 0; long double p = t; while (2 * p - 1 <= n) { p *= t; ++h; } return h; }

static double median(std::vector<double> v) { std::sort(v.begin(), v.end()); return v[v.size() / 2]; }

// ================================================================== THUC NGHIEM A
// (h, nodes, fill, bo nho: cua lan chay cuoi; h_obs_lo/hi: nho nhat/lon nhat qua cac lan chay;
//  so lan doc va thoi gian: trung vi qua cac lan chay)
// Chen n khoa (n >= 100.000 theo yeu cau), do chieu cao, so lan doc dia, bo nho, thoi gian
// voi m = 3, 5, 16, 64, 256 va 4 kieu du lieu. Moi cau hinh lap RUNS lan (seed khac nhau).
static void expHeight(bool quick) {
    std::ofstream f(g_out + "/height_reads.csv");
    f << "n,m,order,h,h_obs_lo,h_obs_hi,h_min,h_max,nodes,fill,heap_MB,disk_MB,splits_per_insert,"
         "reads_per_insert,reads_hit,reads_hit_max,reads_miss,reads_per_remove,merges,borrows,"
         "ns_insert,ns_search,ns_remove,valid\n";
    const std::vector<long> ns = quick ? std::vector<long>{1000, 100000} : std::vector<long>{1000, 10000, 100000, 1000000};
    const int RUNS = quick ? 2 : 5;
    for (long n : ns)
    for (int m : {3, 5, 16, 64, 256})
    for (Order o : {Order::Random, Order::Asc, Order::Desc, Order::ZigZag}) {
        const long Q = std::min(n, 100000L);
        std::vector<double> ti, ts, tr, rIns, rHit, rMiss, rRem;
        long hitMax = 0;
        BTree::Stats st; int h = 0, hLo = 1 << 30, hHi = -1; long splits = 0, merges = 0, borrows = 0; bool valid = true;
        for (int run = 0; run < RUNS; ++run) {
            auto keys = makeKeys(n, o, 1000 + run);
            BTree t(m);
            t.resetCounters();
            auto t0 = Clock::now();
            for (int k : keys) t.insert(k);
            ti.push_back(msSince(t0) * 1e6 / n);
            rIns.push_back(double(t.nodesVisited()) / n);
            splits = t.splits(); h = t.height(); st = t.stats();
            hLo = std::min(hLo, h); hHi = std::max(hHi, h);

            auto hits = sample(keys, Q, 7 + run, false), miss = sample(keys, Q, 9 + run, true);
            long found = 0, worst = 0;
            t.resetVisited();
            t0 = Clock::now();
            for (int k : hits) found += t.search(k);
            ts.push_back(msSince(t0) * 1e6 / Q);
            rHit.push_back(double(t.nodesVisited()) / Q);
            for (std::size_t i = 0; i < 2000 && i < hits.size(); ++i) {   // so nut tham lon nhat
                t.resetVisited(); t.search(hits[i]); worst = std::max(worst, t.nodesVisited());
            }
            hitMax = std::max(hitMax, worst);
            t.resetVisited();
            for (int k : miss) found += t.search(k);
            rMiss.push_back(double(t.nodesVisited()) / Q);
            g_sink += found;

            // xoa mot nua so khoa (thu tu ngau nhien)
            std::vector<int> del(keys.begin(), keys.begin() + n / 2);
            std::mt19937 rng(55 + run); std::shuffle(del.begin(), del.end(), rng);
            t.resetCounters();
            t0 = Clock::now();
            for (int k : del) t.remove(k);
            tr.push_back(msSince(t0) * 1e6 / del.size());
            rRem.push_back(double(t.nodesVisited()) / del.size());
            merges = t.merges(); borrows = t.borrows();
            valid = valid && t.validate();
        }
        char buf[512];
        std::snprintf(buf, sizeof buf,
            "%ld,%d,%s,%d,%d,%d,%d,%d,%ld,%.3f,%.2f,%.2f,%.4f,%.3f,%.3f,%ld,%.3f,%.3f,%ld,%ld,%.1f,%.1f,%.1f,%d\n",
            n, m, name(o), h, hLo, hHi, hMin(n, m), hMax(n, m), st.nodes, st.fill, st.heapBytes / 1048576.0,
            st.diskBytes / 1048576.0, double(splits) / n, median(rIns), median(rHit), hitMax, median(rMiss),
            median(rRem), merges, borrows, median(ti), median(ts), median(tr), valid ? 1 : 0);
        f << buf; std::cout << "[height] " << buf << std::flush;
    }
}

// ================================================================== THUC NGHIEM B
// So sanh B-Tree voi BST khong can bang tren CUNG du lieu, CUNG thao tac:
// chen n khoa -> tim Q khoa co -> tim Q khoa khong co -> xoa Q khoa. std::set (cay do-den) lam moc.
static double heapMB(const BTree& t) { return t.stats().heapBytes / 1048576.0; }
static double heapMB(const BST& t) { return t.heapBytes() / 1048576.0; }
template <typename Tree>
static void runSame(Tree& t, const std::vector<int>& keys, const std::vector<int>& hits,
                    const std::vector<int>& miss, const std::vector<int>& del, double out[7], int& h, double& mb) {
    t.resetVisited();
    auto t0 = Clock::now(); for (int k : keys) t.insert(k);
    out[0] = msSince(t0); out[1] = double(t.nodesVisited()) / keys.size();
    h = t.height(); mb = heapMB(t);                  // do ngay sau khi chen xong n khoa
    t.resetVisited(); long found = 0;
    t0 = Clock::now(); for (int k : hits) found += t.search(k);
    out[2] = msSince(t0); out[3] = double(t.nodesVisited()) / hits.size();
    t.resetVisited(); for (int k : miss) found += t.search(k);
    out[4] = double(t.nodesVisited()) / miss.size();
    t.resetVisited();
    t0 = Clock::now(); for (int k : del) found += t.remove(k);
    out[5] = msSince(t0); out[6] = double(t.nodesVisited()) / del.size();
    g_sink += found;
}
static void expBst(bool quick) {
    std::ofstream f(g_out + "/btree_vs_bst.csv");
    f << "n,order,structure,height_after_insert,heap_MB_after_insert,ms_insert,reads_per_insert,ms_search,reads_hit,reads_miss,ms_remove,reads_per_remove,valid\n";
    const long Q = 10000;
    auto row = [&](long n, Order o, const std::string& s, int h, double mb, const double r[7], bool ok) {
        char buf[300];
        std::snprintf(buf, sizeof buf, "%ld,%s,%s,%d,%.2f,%.1f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%d\n",
                      n, name(o), s.c_str(), h, mb, r[0], r[1], r[2], r[3], r[4], r[5], r[6], ok ? 1 : 0);
        f << buf; std::cout << "[bst] " << buf << std::flush;
    };
    for (long n : quick ? std::vector<long>{10000, 100000} : std::vector<long>{100000, 1000000})
    for (Order o : {Order::Random, Order::Asc, Order::Desc, Order::ZigZag}) {
        if (n > 100000 && o != Order::Random) continue;   // BST suy bien: ~n^2/2 = 5*10^11 buoc, khong chay noi
        auto keys = makeKeys(n, o, 2026);
        auto hits = sample(keys, Q, 11, false), miss = sample(keys, Q, 13, true);
        std::vector<int> del(hits.begin(), hits.end());
        std::sort(del.begin(), del.end()); del.erase(std::unique(del.begin(), del.end()), del.end());
        std::mt19937 rng(3); std::shuffle(del.begin(), del.end(), rng);
        std::set<int> ref(keys.begin(), keys.end());
        for (int k : del) ref.erase(k);
        std::vector<int> want(ref.begin(), ref.end());
        double r[7], mb = 0; int h = 0;
        for (int m : {5, 64}) {
            BTree t(m); runSame(t, keys, hits, miss, del, r, h, mb);
            row(n, o, "btree_m" + std::to_string(m), h, mb, r,
                t.validate() && t.traverse() == want);
        }
        BST b; runSame(b, keys, hits, miss, del, r, h, mb);
        row(n, o, "bst", h, mb, r, b.traverse() == want);
        // std::set: chi do thoi gian (khong dem duoc so nut tham)
        std::set<int> s; long found = 0;
        auto t0 = Clock::now(); for (int k : keys) s.insert(k); r[0] = msSince(t0);
        t0 = Clock::now(); for (int k : hits) found += long(s.count(k)); r[2] = msSince(t0);
        t0 = Clock::now(); for (int k : del) found += long(s.erase(k)); r[5] = msSince(t0);
        g_sink += found; r[1] = r[3] = r[4] = r[6] = -1;
        row(n, o, "std_set", -1, double(keys.size() * 40) / 1048576.0, r, true);
    }
}

// ================================================================== THUC NGHIEM C (cai tien)
// Tim nhi phan (lower_bound) vs duyet tuan tu trong moi nut. So nut tham PHAI giong nhau;
// chi thoi gian CPU khac. n = 10^6 khoa ngau nhien, 10^6 lan tim.
static void expOpt(bool quick) {
    std::ofstream f(g_out + "/opt_linear_vs_binary.csv");
    f << "m,mode,ns_search,ns_insert,reads_hit,same_tree\n";
    const long n = quick ? 200000 : 1000000;
    const int RUNS = quick ? 3 : 7;
    auto keys = makeKeys(n, Order::Random, 42);
    auto hits = sample(keys, n, 43, false);
    for (int m : {3, 4, 5, 8, 16, 32, 64, 128, 256, 512, 1024}) {
        std::string shape[2];
        for (int mode = 0; mode < 2; ++mode) {
            std::vector<double> tsr, tin; double reads = 0;
            for (int run = 0; run < RUNS; ++run) {
                BTree t(m);
                t.setNodeSearch(mode ? BTree::NodeSearch::Linear : BTree::NodeSearch::Binary);
                auto t0 = Clock::now(); for (int k : keys) t.insert(k);
                tin.push_back(msSince(t0) * 1e6 / n);
                t.resetVisited(); long found = 0;
                t0 = Clock::now(); for (int k : hits) found += t.search(k);
                tsr.push_back(msSince(t0) * 1e6 / n);
                reads = double(t.nodesVisited()) / n; g_sink += found;
                if (run == 0 && m <= 16) shape[mode] = t.levels();
            }
            bool same = mode == 0 || m > 16 || shape[0] == shape[1];
            char buf[200];
            std::snprintf(buf, sizeof buf, "%d,%s,%.1f,%.1f,%.3f,%d\n", m, mode ? "linear" : "binary",
                          median(tsr), median(tin), reads, same ? 1 : 0);
            f << buf; std::cout << "[opt] " << buf << std::flush;
        }
    }
}

// ================================================================== THUC NGHIEM D
// Anh huong cua m (quet day): n = 10^6 ngau nhien. Bo nho RAM, bo nho neu luu dia,
// so trang doc moi lan tim, so BYTE doc moi lan tim (= so trang * kich thuoc trang).
static void expMSweep(bool quick) {
    std::ofstream f(g_out + "/m_sweep.csv");
    f << "m,h,h_min,h_max,nodes,fill,heap_MB,page_bytes,disk_MB,reads_hit,bytes_per_search,ns_search\n";
    const long n = quick ? 200000 : 1000000;
    auto keys = makeKeys(n, Order::Random, 77);
    auto hits = sample(keys, 200000, 78, false);
    for (int m : {3, 4, 5, 8, 16, 32, 64, 128, 256, 341, 512, 1024, 4096}) {
        BTree t(m);
        for (int k : keys) t.insert(k);
        t.resetVisited(); long found = 0;
        auto t0 = Clock::now(); for (int k : hits) found += t.search(k);
        double ns = msSince(t0) * 1e6 / hits.size(); g_sink += found;
        double reads = double(t.nodesVisited()) / hits.size();
        auto st = t.stats();
        long page = 4 + 4L * (m - 1) + 8L * m;
        char buf[300];
        std::snprintf(buf, sizeof buf, "%d,%d,%d,%d,%ld,%.3f,%.2f,%ld,%.2f,%.3f,%.0f,%.1f\n", m, t.height(),
                      hMin(n, m), hMax(n, m), st.nodes, st.fill, st.heapBytes / 1048576.0, page,
                      st.diskBytes / 1048576.0, reads, reads * page, ns);
        f << buf; std::cout << "[msweep] " << buf << std::flush;
    }
}

int main(int argc, char** argv) {
    std::string what = argc > 1 ? argv[1] : "all";
    bool quick = argc > 2 && std::string(argv[2]) == "--quick";
    if (quick) g_out = "results/quick";
    std::filesystem::create_directories(g_out);
    if (what == "all" || what == "height") expHeight(quick);
    if (what == "all" || what == "bst")    expBst(quick);
    if (what == "all" || what == "opt")    expOpt(quick);
    if (what == "all" || what == "msweep") expMSweep(quick);
    return g_sink == 42 ? 1 : 0;
}
