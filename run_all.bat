@echo off
rem run_all.bat - bien dich va chay lai TOAN BO kiem thu va thuc nghiem tren Windows.
rem Can g++ (MinGW-w64 / MSYS2) trong PATH. Them --quick de chay nhanh: run_all.bat --quick
setlocal
set FLAGS=-std=c++17 -Wall -Wextra -Werror -Isrc -O2
if not exist build mkdir build
echo == Bien dich
g++ %FLAGS% -o build\demo.exe         src\main.cpp           src\btree.cpp                || goto :err
g++ %FLAGS% -o build\test_btree.exe   tests\test_btree.cpp   src\btree.cpp                || goto :err
g++ %FLAGS% -o build\stress_btree.exe tests\stress_btree.cpp src\btree.cpp                || goto :err
g++ %FLAGS% -o build\edge_cases.exe   tests\edge_cases.cpp   src\btree.cpp src\bst.cpp    || goto :err
g++ %FLAGS% -o build\trace.exe        tools\trace.cpp        src\btree.cpp                || goto :err
g++ %FLAGS% -DNDEBUG -o build\bench_btree.exe bench\bench.cpp src\btree.cpp src\bst.cpp   || goto :err
echo == Demo (vi du chay tay Tuan 1)
build\demo.exe
echo == Unit test T1-T16
build\test_btree.exe
echo == Kiem tra ap luc
build\stress_btree.exe
echo == Truong hop bien E1-E14
build\edge_cases.exe
echo == Thuc nghiem -^> results\ (--quick: results\quick\)
build\bench_btree.exe all %1 > build\bench.log || goto :err
where python >nul 2>nul && (python bench\plot.py %1 || echo Can pandas + matplotlib de ve hinh)
echo == XONG
pause
exit /b 0
:err
echo LOI - xem thong bao phia tren.
pause
exit /b 1
