# B-Tree for Disk-Oriented Search

Bài tập lớn môn **Cấu trúc dữ liệu và Giải thuật** – HCMC University of Technology and Education.

- Sinh viên: Đinh Mai Tuyết Nhi – MSSV 25110049
- Giảng viên hướng dẫn: Huỳnh Xuân Phụng
- Đề tài (Appendix A): B-Tree for Disk-Oriented Search – Lesson 8 & 9
- Ngôn ngữ: C++17, không dùng thư viện ngoài (vẽ hình dùng Python + pandas + matplotlib)

**Báo cáo tổng: [docs/final/report.md](docs/final/report.md)**

## Kết quả chính

| | |
|---|---|
| Đúng | 45/45 unit test, 630.000 thao tác ngẫu nhiên đối chiếu `std::set`, 14/14 trường hợp biên, sạch AddressSanitizer / UBSan |
| Chiều cao | 80/80 cấu hình nằm trong [⌈log<sub>m</sub>(n+1)⌉ − 1, ⌊log<sub>⌈m/2⌉</sub>((n+1)/2)⌋] |
| Đọc đĩa với 10⁶ khóa | m = 256: 3 lần / tìm · m = 5: 11 · BST ngẫu nhiên: 25,5 · BST dữ liệu có thứ tự: ~500.000 |
| Chọn m | Một nút vừa một khối đĩa: m ≈ 341 với khối 4 KB |

## Chạy lại toàn bộ

Một lệnh biên dịch tất cả, chạy demo, unit test, kiểm tra áp lực, trường hợp biên, thực nghiệm và vẽ hình:

| Hệ điều hành | Lệnh | Cần |
|---|---|---|
| Linux / macOS / Git Bash | `sh run_all.sh` | g++ ≥ 9 |
| Windows | bấm đúp `run_all.bat` hoặc `run_all.bat` trong cmd | g++ MinGW-w64 (MSYS2) trong PATH |

- Bản đầy đủ mất khoảng 15 phút trên máy 1 nhân và ghi đè `results/*.csv`, `docs/week3/images/*.png`.
- Thêm `--quick` (`sh run_all.sh --quick`) để chạy nhanh khoảng 4 phút với n nhỏ hơn; kết quả ghi vào `results/quick/` và **không** đè lên số liệu chính thức.
- Thời gian (ns, ms) phụ thuộc máy; số nút thăm, chiều cao, số tách / gộp thì giống hệt khi chạy lại.

Chạy riêng từng phần (sau khi đã chạy `run_all` một lần, các chương trình nằm trong `build/`):

| Chương trình | Nội dung |
|---|---|
| `build/demo` | Chạy lại ví dụ chạy tay Tuần 1 (m = 5, chèn 20 40 10 30 15 35 45 50) |
| `build/test_btree` | Unit test T0–T16 |
| `build/stress_btree` | 630.000 thao tác ngẫu nhiên, m = 3…9 |
| `build/edge_cases` | Trường hợp biên và đối kháng E1–E14, in bảng Markdown |
| `build/bench_btree [all\|height\|bst\|opt\|msweep] [--quick]` | Thực nghiệm, ghi CSV |
| `build/trace <m> +k -k ?k ...` | In cây sau từng thao tác. Ví dụ: `build/trace 5 +1 +2 +3 +4 +5 -5` (trường hợp E9) |

## Cấu trúc thư mục

```
src/            btree.h, btree.cpp (B-Tree); bst.h, bst.cpp (BST để so sánh); main.cpp (demo)
tests/          test_btree.cpp (T0–T16), stress_btree.cpp, edge_cases.cpp (E1–E14)
bench/          bench.cpp (bộ thực nghiệm), plot.py (vẽ hình)
tools/          trace.cpp (in cây từng bước, để kiểm tra bản chạy tay)
results/        số liệu thực nghiệm chính thức (CSV)
docs/final/     báo cáo tổng
docs/week1..3/  báo cáo từng tuần, kế hoạch kiểm thử, hình
docs/ai-log.md  nhật ký sử dụng AI
run_all.sh, run_all.bat   chạy lại toàn bộ
```

## Tiến độ

| Tuần | Nội dung | Tài liệu |
|:---:|---|---|
| 1 | Hiểu đề, chọn cấu trúc dữ liệu, chạy tay, kế hoạch kiểm thử, khung mã | [Báo cáo](docs/week1/report.md) · [Kế hoạch kiểm thử](docs/week1/test-plan.md) |
| 2 | Cài đặt các thao tác, unit test, chứng minh độ phức tạp | [Báo cáo](docs/week2/report.md) |
| 3 | Thực nghiệm, so sánh lý thuyết và BST, ảnh hưởng của m, thử cải tiến, trường hợp biên | [Báo cáo](docs/week3/report.md) |
| 4 | Báo cáo tổng, phân tích độ phức tạp cuối, hạn chế, ánh xạ yêu cầu → code, chuẩn bị vấn đáp | [Báo cáo tổng](docs/final/report.md) |

## Sử dụng AI

Dự án có sử dụng công cụ AI; toàn bộ được khai báo trong [docs/ai-log.md](docs/ai-log.md) và mục 11 của báo cáo tổng.
