#!/usr/bin/env sh
# run_all.sh - bien dich va chay lai TOAN BO kiem thu va thuc nghiem (Linux / macOS / Git Bash).
#   sh run_all.sh           chay day du (thuc nghiem khoang 15 phut tren may 1 nhan)
#   sh run_all.sh --quick   chay nhanh (khoang 3-4 phut, n nho hon)
set -e
CXX=${CXX:-g++}
FLAGS="-std=c++17 -Wall -Wextra -Werror -Isrc"
mkdir -p build
echo "== Bien dich"
$CXX $FLAGS -O2      -o build/demo         src/main.cpp            src/btree.cpp
$CXX $FLAGS -O2      -o build/test_btree   tests/test_btree.cpp    src/btree.cpp
$CXX $FLAGS -O2      -o build/stress_btree tests/stress_btree.cpp  src/btree.cpp
$CXX $FLAGS -O2      -o build/edge_cases   tests/edge_cases.cpp    src/btree.cpp src/bst.cpp
$CXX $FLAGS -O2      -o build/trace        tools/trace.cpp         src/btree.cpp
$CXX $FLAGS -O2 -DNDEBUG -o build/bench_btree bench/bench.cpp      src/btree.cpp src/bst.cpp
echo "== Demo (vi du chay tay Tuan 1)";       ./build/demo
echo "== Unit test T1-T16";                   ./build/test_btree | tail -1
echo "== Kiem tra ap luc";                    ./build/stress_btree
echo "== Truong hop bien E1-E14";             ./build/edge_cases | tail -1
echo "== Thuc nghiem -> results/ (--quick: results/quick/)";       ./build/bench_btree all $1 > build/bench.log
if command -v python3 >/dev/null 2>&1; then
  echo "== Ve hinh";                          python3 bench/plot.py $1 || echo "(can pandas + matplotlib de ve hinh)"
fi
echo "== XONG"
