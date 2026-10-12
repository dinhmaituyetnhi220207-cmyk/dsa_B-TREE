# Kế hoạch kiểm thử – Tuần 1

B-Tree bậc 5 (tối đa 4 khóa mỗi nút). Kết quả mong đợi được **tính tay trước khi viết code**, theo quy ước ở Mục 3.6 của [báo cáo](report.md). “Hình 1” là cây trong [images/fig1_btree_order5.png](images/fig1_btree_order5.png).

## 1. Test case chức năng

| Mã | Mục đích | Đầu vào | Kết quả mong đợi | Trạng thái |
|:---:|---|---|---|:---:|
| T1 | Cây rỗng | `search(5)`; `traverse()` | false; danh sách rỗng | Chưa chạy |
| T2 | Chèn chưa đầy | Chèn 10, 20, 30, 40 | Một nút [10, 20, 30, 40]; h = 0 | Chưa chạy |
| T3 | Tách gốc | T2, chèn thêm 50 | Gốc [30]; lá [10, 20], [40, 50]; h = 1 | Chưa chạy |
| T4 | Khớp chạy tay | Chèn 20, 40, 10, 30, 15, 35, 45, 50 | Gốc [20, 40]; lá [10, 15], [30, 35], [45, 50] | Chưa chạy |
| T5 | Khóa trùng | T4, chèn thêm 35 | Trả về false; cây không đổi | Chưa chạy |
| T6 | Tìm ở gốc | `search(100)` trên Hình 1 | true; thăm 1 nút | Chưa chạy |
| T7 | Tìm ở lá | `search(120)` trên Hình 1 | true; đường đi [100] → [130, 180] → [110, 120] | Chưa chạy |
| T8 | Khóa không tồn tại | `search(125)`, `search(5)`, `search(300)` trên Hình 1 | false; mỗi lần thăm 3 nút | Chưa chạy |
| T9 | Duyệt theo thứ tự | `traverse()` trên Hình 1 | 19 khóa tăng dần: 10, 20, 35, 40, …, 260 | Chưa chạy |
| T10 | Xóa ở lá còn dư | `remove(260)` trên Hình 1 | Lá cuối thành [190, 240]; phần còn lại không đổi | Chưa chạy |
| T11 | Xóa phải mượn khóa | `remove(40)` trên Hình 1 | [40, 50] còn [50], mượn qua cha từ [70, 80, 90]: lá [50, 65], cha [35, 70], anh em [80, 90] | Chưa chạy |
| T12 | Xóa ở nút trong | `remove(100)` trên Hình 1 | Thay bằng khóa liền trước 90: gốc [90]; lá [70, 80] | Chưa chạy |
| T13 | Gộp dây chuyền, cây thấp đi | `remove(110)` trên Hình 1 | Gộp hai lần, h giảm từ 2 xuống 1: gốc [35, 65, 100, 180]; lá [10, 20], [40, 50], [70, 80, 90], [120, 130, 140, 160], [190, 240, 260] | Chưa chạy |
| T14 | Xóa khóa không có | `remove(125)` trên Hình 1 | false; cây không đổi | Chưa chạy |
| T15 | Dữ liệu tăng dần | Chèn 1, 2, …, 1000 | `validate()` luôn đúng; 4 ≤ h ≤ 5 | Chưa chạy |
| T16 | Ngẫu nhiên, đối chiếu | Chèn 300 khóa ngẫu nhiên, xóa 200 khóa | `traverse()` bằng danh sách sắp xếp của các khóa còn lại; `validate()` luôn đúng | Chưa chạy |

## 2. Hàm `validate()`

Sau mỗi thao tác chèn hoặc xóa, `validate()` kiểm tra:

1. Khóa trong mỗi nút tăng dần.
2. Mỗi nút có tối đa m − 1 khóa; mỗi nút không phải gốc có ít nhất ⌈m/2⌉ − 1 khóa.
3. Mỗi nút trong có k khóa thì có đúng k + 1 con.
4. Mọi lá nằm cùng một độ sâu.
5. Mọi khóa trong cây con thứ i nằm đúng khoảng giữa hai khóa phân cách của nút cha.

## 3. Thực nghiệm hiệu năng (Tuần 3)

| Yếu tố | Kế hoạch |
|---|---|
| Kích thước n | 10<sup>3</sup>, 10<sup>4</sup>, 10<sup>5</sup>, 10<sup>6</sup> khóa |
| Kiểu dữ liệu | Ngẫu nhiên; tăng dần; giảm dần. Dữ liệu tăng dần dự kiến là trường hợp xấu về chiều cao: các lần tách luôn để lại nút bên trái ở mức tối thiểu |
| Bậc m | 3, 5, 16, 64, 256 |
| Đại lượng đo | Thời gian chèn, tìm, xóa; chiều cao h; số lần tách và gộp nút; số nút được thăm mỗi lần tìm |
| So sánh lý thuyết | Số nút thăm khi tìm = h + 1; ⌈log<sub>m</sub>(n + 1)⌉ − 1 ≤ h ≤ log<sub>t</sub>((n + 1)/2) |
| Cách đo | Mỗi cấu hình chạy 5 lần với seed cố định, lấy trung bình |
| Cải tiến thử nghiệm | Tìm nhị phân thay cho duyệt tuần tự trong mỗi nút; giữ lại nếu nhanh hơn rõ rệt với m lớn |
