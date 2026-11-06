# B-Tree for Disk-Oriented Search

Bài tập lớn môn **Cấu trúc dữ liệu và Giải thuật** – HCMC University of Technology and Education.

- Sinh viên: Đinh Mai Tuyết Nhi – MSSV 25110049
- Giảng viên hướng dẫn: Huỳnh Xuân Phụng
- Đề tài (Appendix A): B-Tree for Disk-Oriented Search – Lesson 8 & 9
- Ngôn ngữ: C++17

## Mục tiêu

Cài đặt B-Tree bậc m (mặc định m = 5) với các thao tác tìm kiếm, chèn, xóa, duyệt theo thứ tự và kiểm tra tính hợp lệ; đếm số nút được thăm để mô phỏng số lần đọc đĩa; đo hiệu năng và so sánh với lý thuyết.

## Cấu trúc thư mục

```
src/            mã nguồn C++ (btree.h, btree.cpp, main.cpp; bst.h, bst.cpp để so sánh)
tests/          unit test T1–T16, kiểm tra áp lực, trường hợp biên E1–E14
bench/          bộ thực nghiệm Tuần 3 (bench.cpp) và script vẽ hình (plot.py)
results/        kết quả thực nghiệm dạng CSV
docs/week1/     báo cáo Tuần 1, kế hoạch kiểm thử, hình minh họa
docs/week2/     báo cáo Tuần 2
docs/week3/     báo cáo Tuần 3 và hình kết quả
docs/ai-log.md  nhật ký sử dụng AI (cộng dồn các tuần)
```

## Biên dịch và chạy

```bash
g++ -std=c++17 -Wall -Wextra -o btree src/btree.cpp src/main.cpp
./btree
```

## Tiến độ

| Tuần | Nội dung | Tài liệu |
|:---:|---|---|
| 1 | Hiểu đề, chọn cấu trúc dữ liệu, chạy tay, kế hoạch kiểm thử, khung mã C++ | [Báo cáo](docs/week1/report.md) · [Kế hoạch kiểm thử](docs/week1/test-plan.md) |
| 2 | Cài đặt các thao tác, unit test, chứng minh độ phức tạp | [Báo cáo](docs/week2/report.md) |
| 3 | Thực nghiệm hiệu năng, so sánh lý thuyết và BST, ảnh hưởng của m, thử cải tiến, trường hợp biên | [Báo cáo](docs/week3/report.md) |
| 4 | Hoàn thiện, chuẩn bị vấn đáp | – |

## Sử dụng AI

Dự án có sử dụng công cụ AI; toàn bộ được khai báo trong [docs/ai-log.md](docs/ai-log.md).

## Tuần 2 – Cài đặt cốt lõi

- Mã nguồn: [src/btree.h](src/btree.h), [src/btree.cpp](src/btree.cpp) – search, insert (tách khi tràn), remove (mượn/gộp), traverse, validate (đủ 5 tính chất), đếm số nút được thăm.
- Unit test T1–T16 theo [kế hoạch Tuần 1](docs/week1/test-plan.md): [tests/test_btree.cpp](tests/test_btree.cpp) – 45/45 kiểm tra đạt.
- Demo chạy lại ví dụ chạy tay Tuần 1: [src/main.cpp](src/main.cpp).

```bash
g++ -std=c++17 -Wall -Wextra -Isrc -o test_btree tests/test_btree.cpp src/btree.cpp
./test_btree
g++ -std=c++17 -Wall -Wextra -o btree src/main.cpp src/btree.cpp
./btree
```
- Báo cáo Tuần 2 (chứng minh độ phức tạp, chỗ AI làm sai, minh chứng commit): [docs/week2/report.md](docs/week2/report.md)
- Kiểm tra áp lực 630.000 thao tác, m = 3…9: [tests/stress_btree.cpp](tests/stress_btree.cpp)

## Tuần 3 – Thực nghiệm và phân tích

- Báo cáo: [docs/week3/report.md](docs/week3/report.md) – chiều cao và số lần đọc đĩa với n = 10⁵, 10⁶ và m = 3, 5, 16, 64, 256; so sánh BST; ảnh hưởng của m; thử cải tiến (bị loại, có số liệu); trường hợp biên E1–E14.
- Bộ thực nghiệm: [bench/bench.cpp](bench/bench.cpp), vẽ hình: [bench/plot.py](bench/plot.py), số liệu: [results/](results).

```bash
g++ -std=c++17 -O2 -DNDEBUG -Isrc -o bench_btree bench/bench.cpp src/btree.cpp src/bst.cpp
./bench_btree all            # khoảng 15 phút trên máy 1 nhân; ./bench_btree all --quick để chạy nhanh
python3 bench/plot.py        # cần pandas, matplotlib
g++ -std=c++17 -O2 -Wall -Wextra -Isrc -o edge_cases tests/edge_cases.cpp src/btree.cpp src/bst.cpp
./edge_cases
```
