# Nhật ký sử dụng AI

Khai báo theo quy định môn học. Mỗi tuần một mục; phần chỉnh sửa của sinh viên được minh chứng bằng link file và commit.

## Tuần 1

| # | Công cụ / model | Prompt (tóm tắt) | AI tạo ra | Sinh viên đã sửa / hoàn thiện | Minh chứng |
|:---:|---|---|---|---|---|
| 1 | Gemini 3.6 | “B tree là gì, cấu trúc và hoạt động của nó như thế nào” | Giải thích định nghĩa, tính chất | Đối chiếu CLRS và Knuth; tự viết lại Mục 3.1 | [report.md – Mục 3.1](week1/report.md#31-định-nghĩa-b-tree-bậc-m) |
| 2 | Gemini 3.6 | “What is application of B-tree” | Danh sách ứng dụng | Bỏ phần chép nguyên văn; viết lại ngắn ở Mục 2.1 | [report.md – Mục 2.1](week1/report.md#21-bối-cảnh) |




## Tuần 2

| # | Công cụ / model | Prompt (tóm tắt) | AI tạo ra | Sinh viên đã sửa / hoàn thiện | Minh chứng |


**AI làm sai / thiếu đã phát hiện trong Tuần 2:** bản code ban đầu tách nút sớm (cách của CLRS) nên với m lẻ tạo ra nút thiếu khóa; phát hiện bằng cách chạy test T4 và so với bản chạy tay Tuần 1, rồi kiểm tra đủ 5 tính chất thì thấy nút chỉ có 1 khóa.


## Tuần 3

| # | Công cụ / model | Prompt (tóm tắt) | AI tạo ra | Sinh viên đã sửa / hoàn thiện | Minh chứng |
|:---:|---|---|---|---|---|
| 1 | Claude Opus 5.5 | “làm tiếp tuần 3” kèm yêu cầu Tuần 3 của đề và nhận xét GVHD (chèn ≥ 100.000 khóa với ≥ 3 giá trị m; so sánh BST; ảnh hưởng của m) | Bộ đếm tách/gộp/mượn và `stats()` trong B-Tree; lớp BST; `bench/bench.cpp`, `bench/plot.py`, `tests/edge_cases.cpp`; số liệu `results/`; bản nháp báo cáo Tuần 3 | *(sinh viên điền: phần đã tự kiểm tra, chạy lại, sửa)* | *(link commit)* |

**AI làm sai / thiếu đã phát hiện trong Tuần 3:** xem [báo cáo Tuần 3, Mục 11](week3/report.md#11-chỗ-ai-làm-sai--thiếu-trong-tuần-3-và-cách-phát-hiện) (trường hợp đối kháng E10 bản đầu không gây tách nút; cột chiều cao không khớp số lần đọc giữa các lần chạy; đo chiều cao BST sau khi xóa thay vì sau khi chèn).
