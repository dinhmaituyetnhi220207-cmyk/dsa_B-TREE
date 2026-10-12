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
src/            mã nguồn C++ (btree.h, btree.cpp, main.cpp)
docs/week1/     báo cáo Tuần 1, kế hoạch kiểm thử, hình minh họa
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
| 2 | Cài đặt các thao tác, unit test, chứng minh độ phức tạp | – |
| 3 | Thực nghiệm hiệu năng, so sánh lý thuyết, thử cải tiến | – |
| 4 | Hoàn thiện, chuẩn bị vấn đáp | – |

## Sử dụng AI

Dự án có sử dụng công cụ AI; toàn bộ được khai báo trong [docs/ai-log.md](docs/ai-log.md).
