# Báo cáo Tuần 2 – Cài đặt cốt lõi

**Đề tài:** B-Tree for Disk-Oriented Search · **Sinh viên:** Đinh Mai Tuyết Nhi – 25110049 · **Ngôn ngữ:** C++17

## 1. Tóm tắt

| Việc | Kết quả | File |
|---|---|---|
| Cài đặt search, insert, remove, traverse, validate, đếm số nút thăm | Xong, biên dịch sạch với `-Wall -Wextra -Werror` | [src/btree.h](../../src/btree.h), [src/btree.cpp](../../src/btree.cpp) |
| Unit test T1–T16 (kế hoạch Tuần 1) | **45/45 kiểm tra đạt** | [tests/test_btree.cpp](../../tests/test_btree.cpp) |
| Kiểm tra áp lực, đối chiếu `std::set` | 630.000 thao tác ngẫu nhiên, m = 3…9, không lỗi | [tests/stress_btree.cpp](../../tests/stress_btree.cpp) |
| Demo chạy lại ví dụ chạy tay Tuần 1 | Khớp từng bước | [src/main.cpp](../../src/main.cpp) |
| Chứng minh độ phức tạp | Mục 4 | – |
| Chỗ AI làm sai / thiếu | Mục 5 | – |

## 2. Cài đặt

Quy ước giữ đúng Mục 3.6 báo cáo Tuần 1 (định nghĩa bậc m của Knuth).

| Hàm | Ý tưởng |
|---|---|
| [`search`](../../src/btree.cpp#L24) | Mỗi nút: tìm nhị phân vị trí i đầu tiên có `keys[i] >= key` (`lower_bound`); bằng thì trả true, là lá thì trả false, ngược lại xuống `children[i]`. Mỗi nút đi qua tăng `visited_` (mô phỏng một lần đọc đĩa). |
| [`insertRec`](../../src/btree.cpp#L55), [`splitNode`](../../src/btree.cpp#L40), [`insert`](../../src/btree.cpp#L76) | Đi xuống tới lá và chèn. Khi quay lên, nút nào có **m khóa** (tràn) thì tách: giữ m/2 khóa đầu, đẩy khóa thứ m/2 lên cha, phần còn lại sang nút mới. Gốc bị tách thì tạo gốc mới, cây cao thêm 1. Khóa trùng được phát hiện ngay trên đường đi xuống. |
| [`removeRec`](../../src/btree.cpp#L138), [`fixChild`](../../src/btree.cpp#L109), [`mergeChildren`](../../src/btree.cpp#L96), [`remove`](../../src/btree.cpp#L162) | Khóa ở nút trong thì thay bằng khóa liền trước rồi xóa khóa đó ở lá. Khi quay lên, con nào còn ít hơn ⌈m/2⌉ − 1 khóa thì: mượn anh em trái → mượn anh em phải → gộp. Gốc hết khóa thì cây thấp đi 1. |
| [`validate`](../../src/btree.cpp#L244) | Kiểm tra đủ 5 tính chất của Tuần 1 và báo tên tính chất bị vi phạm. |

## 3. Kiểm thử

```bash
g++ -std=c++17 -Wall -Wextra -Isrc -o test_btree tests/test_btree.cpp src/btree.cpp && ./test_btree
g++ -std=c++17 -O2 -Isrc -o stress_btree tests/stress_btree.cpp src/btree.cpp && ./stress_btree
```

| Test | Nội dung | Kết quả |
|:---:|---|:---:|
| T0 | Dựng đúng cây Hình 1 bằng 19 lần `insert` | PASS |
| T1–T3 | Cây rỗng; chèn chưa đầy; tách gốc | PASS |
| **T4** | **Chèn 20, 40, 10, 30, 15, 35, 45, 50 → gốc [20 40]; lá [10 15] [30 35] [45 50], khớp bản chạy tay Tuần 1** | **PASS** |
| T5 | Chèn khóa trùng trả về false, cây không đổi | PASS |
| T6–T8 | Tìm ở gốc (thăm 1 nút), ở lá (đường đi đúng), khóa không tồn tại (thăm 3 nút) | PASS |
| T9 | Duyệt theo thứ tự ra 19 khóa tăng dần | PASS |
| T10–T14 | Xóa ở lá; mượn khóa; xóa ở nút trong; gộp dây chuyền làm cây thấp đi; xóa khóa không có | PASS |
| T15 | Chèn 1…1000 tăng dần: validate luôn đúng, h = 5 | PASS |
| T16 | 300 chèn + 200 xóa ngẫu nhiên, đối chiếu `std::set` | PASS |

Ngoài ra, chạy toàn bộ test với AddressSanitizer không phát hiện lỗi bộ nhớ.

## 4. Chứng minh độ phức tạp

Ký hiệu: n là số khóa, m là bậc, t = ⌈m/2⌉, h là chiều cao (số cạnh từ gốc tới lá).

**Bổ đề (chiều cao, chứng minh ở Mục 3.3 báo cáo Tuần 1):** ⌈log<sub>m</sub>(n + 1)⌉ − 1 ≤ h ≤ log<sub>t</sub>((n + 1)/2). Vậy h = O(log<sub>t</sub> n).

**Search.** Mỗi vòng lặp thăm đúng một nút rồi xuống một tầng, nên thăm nhiều nhất h + 1 nút: **O(log<sub>t</sub> n) lần đọc đĩa**. Trong mỗi nút, `lower_bound` tốn O(log m). Tổng thời gian CPU là O(log m · log<sub>t</sub> n) = O(log m · log n / log t). Vì t ≥ m/2 nên log t ≥ log m − 1, tỉ số log m / log t bị chặn bởi hằng số; do đó **search là O(log n)**, không phụ thuộc m.

**Insert.** Đi xuống thăm h + 1 nút (O(log m) mỗi nút); chèn vào lá dịch tối đa m phần tử, O(m). Khi quay lên, mỗi tầng tách nhiều nhất một nút, mỗi lần tách và chèn khóa giữa vào cha tốn O(m); có tối đa h + 1 lần tách. Tổng: **O(h) nút được thăm, thời gian O(m · log<sub>t</sub> n)**; với m cố định là O(log n).

**Remove.** Đi xuống tìm khóa thăm tối đa h + 1 nút. Nếu khóa ở nút trong, việc tìm khóa liền trước đi tiếp xuống cùng một nhánh, vẫn trong h + 1 nút. Khi quay lên, mỗi tầng gọi `fixChild` một lần: mượn khóa dịch O(m) phần tử, gộp chép O(m) phần tử. Tổng: **O(m · log<sub>t</sub> n)**.

**Traverse và validate.** Mỗi nút được thăm đúng một lần và mỗi khóa được xử lý O(1) lần: **O(n)**.

**Bộ nhớ.** Mỗi nút không phải gốc có ít nhất t − 1 khóa, nên số nút ≤ 1 + (n − 1)/(t − 1). Mỗi nút giữ O(m) phần tử, nên tổng bộ nhớ O(n · m / t) = **O(n)** (vì m/t ≤ 2).

**Đối chiếu với số liệu chạy thật:**
- T6–T8: số nút thăm bằng độ sâu của nút chứa khóa cộng 1 (1 nút khi khóa ở gốc, 3 nút khi phải xuống tới lá, h = 2).
- T15: chèn 1…1000 tăng dần cho h = 5, **đúng bằng cận trên** ⌊log<sub>3</sub>(500,5)⌋ = 5. Dữ liệu tăng dần làm mỗi lần tách để lại nút bên trái ở mức tối thiểu, nên đây là trường hợp xấu nhất về chiều cao. Tuần 3 sẽ đo chi tiết.

## 5. Chỗ AI làm sai / thiếu và cách phát hiện

Bản cài đặt ban đầu (Gemini Flash 3.6) nằm ở commit [`0aca89b`](https://github.com/dinhmaituyetnhi220207-cmyk/dsa_B-TREE/commit/0aca89b). Bản sửa nằm ở commit [`c4b7711`](https://github.com/dinhmaituyetnhi220207-cmyk/dsa_B-TREE/commit/c4b7711992c1): trang diff của commit này cho thấy `splitChild` / `insertNonFull` (tách nút sớm, `new Node(true)`) bị thay bằng `insertRec` / `splitNode` (tách khi tràn), cùng `removeRec`, `fixChild`, `validateRec` mới.

| # | Lỗi | Cách phát hiện | Cách sửa |
|:---:|---|---|---|
| 1 | **Chèn theo kiểu CLRS: tách nút sớm** khi nút có m − 1 khóa, trái với quy ước Tuần 1. Với m lẻ, nút đầy có số khóa chẵn nên tách lệch: m = 5 sinh nút chỉ 1 khóa (chèn 10, 20, 30, 40 rồi 5 → lá [40]); m = 3 sinh **nút rỗng** (ví dụ trong `main.cpp` cũ). | Chạy **T4** và so với bản chạy tay Tuần 1: code cũ cho gốc [30]; lá [10 15 20] [35 40 45 50], lệch hẳn. Viết bộ kiểm tra đủ 5 tính chất thì thấy nút 1 khóa và nút 0 khóa. | Chèn vào lá rồi **tách khi tràn** (m khóa), lấy khóa thứ m/2 làm khóa giữa. |
| 2 | **`validate()` thiếu 3 tính chất** (số khóa tối thiểu, số con = số khóa + 1, lá cùng tầng), nên vẫn trả `true` với hai cây hỏng ở trên. | Gọi `validate()` cũ trên cây có nút rỗng: kết quả `true`. | Kiểm tra đủ 5 tính chất, báo tên tính chất bị vi phạm. |
| 3 | `visited_` không bao giờ được tăng, nên `nodesVisited()` luôn bằng 0. | Gọi `search(35)` rồi đọc `nodesVisited()`: ra 0, mong đợi 2. | Tăng `visited_` ở mỗi nút đi qua; thêm `resetVisited()`. |
| 4 | Gọi `new Node(true)` trong khi `Node` không có constructor nhận `bool`: **không biên dịch được**. | Biên dịch với `g++ -Wall -Wextra`. | Tính lá suy ra từ `children.empty()`, dùng `new Node()`. |
| 5 | `remove()` chưa cài đặt (luôn trả false), trái với kế hoạch Tuần 2. | T10–T14 không thể đạt. | Cài đặt mượn trái/phải và gộp nút. |

**Bài học:** tách nút sớm của CLRS chỉ đúng khi số khóa tối đa là số lẻ (2t − 1, tức m chẵn). Phải đối chiếu code AI với quy ước đã chọn và với bản chạy tay; unit test T4 bắt được lỗi này ngay.



## 6. Minh chứng commit

| Commit | Nội dung |
|---|---|
| [`0aca89b`](https://github.com/dinhmaituyetnhi220207-cmyk/dsa_B-TREE/commit/0aca89b) | Bản cài đặt ban đầu (tách nút sớm kiểu CLRS, chưa biên dịch được) |
| [`c4b7711`](https://github.com/dinhmaituyetnhi220207-cmyk/dsa_B-TREE/commit/c4b7711992c1) | Sửa lỗi: viết lại B-Tree theo quy ước Tuần 1 (tách khi tràn, remove mượn/gộp, validate đủ 5 tính chất, đếm số nút thăm) |
| [`1a09562`](https://github.com/dinhmaituyetnhi220207-cmyk/dsa_B-TREE/commit/1a095629de7b) | Unit test T1–T16, 45 kiểm tra |
| [`c343392`](https://github.com/dinhmaituyetnhi220207-cmyk/dsa_B-TREE/commit/c34339278a56) | `main.cpp` chạy lại ví dụ chạy tay Tuần 1 |
| [`f2ea544`](https://github.com/dinhmaituyetnhi220207-cmyk/dsa_B-TREE/commit/f2ea544b2462) | README – cách chạy test, tiến độ Tuần 2 |
| [`1bb1faf`](https://github.com/dinhmaituyetnhi220207-cmyk/dsa_B-TREE/commit/1bb1faf43e3a) | Nhật ký AI Tuần 2 |

Kiểm tra áp lực (`tests/stress_btree.cpp`) và báo cáo này được thêm ở các commit nộp bổ sung; link trực tiếp của hai commit đó có trong nội dung nộp trên hệ thống.

## 7. Kế hoạch Tuần 3

Đo thời gian và số nút thăm với n = 10<sup>3</sup>…10<sup>6</sup>, dữ liệu ngẫu nhiên / tăng dần / giảm dần, m = 3…256; so với cận lý thuyết ở Mục 4; thử cải tiến tìm nhị phân so với duyệt tuần tự trong nút.
