# Báo cáo tổng – B-Tree for Disk-Oriented Search

| Mục | Nội dung | 
| ----- | ----- | 
| Trường / Khoa | HCMC University of Technology and Education / Faculty of Information Technology | 
| Môn học | Cấu trúc dữ liệu và Giải thuật · Đề tài Appendix A · Lesson 8 & 9 | 
| Sinh viên | Đinh Mai Tuyết Nhi – MSSV 25110049 | 
| Giảng viên hướng dẫn | Huỳnh Xuân Phụng | 
| Kho mã nguồn | https://github.com/dinhmaituyetnhi220207-cmyk/dsa_B-TREE | 
| Ngôn ngữ | C++17, không dùng thư viện ngoài | 
| Báo cáo từng tuần | [Tuần 1](../week1/report.md) · [Kế hoạch kiểm thử](../week1/test-plan.md) · [Tuần 2](../week2/report.md) · [Tuần 3](../week3/report.md) | 

## 1. Tóm tắt

Đề tài cài đặt B-Tree bậc $m$ ($m \ge 3$) theo định nghĩa của Knuth, với đủ các thao tác tìm, chèn (tách khi tràn), xóa (mượn / gộp), duyệt theo thứ tự và kiểm tra hợp lệ, cùng bộ đếm số nút được thăm để mô phỏng số lần đọc đĩa. Kết quả chính:

* **Đúng:** 45/45 kiểm tra đơn vị (T1–T16, khớp bản chạy tay Tuần 1), 630.000 thao tác ngẫu nhiên đối chiếu `std::set`, 14/14 trường hợp biên và đối kháng (E1–E14), không lỗi dưới AddressSanitizer / UBSan.

* **Khớp lý thuyết:** 80/80 cấu hình ($n = 10^3 \dots 10^6$, $m = 3 \dots 256$, 4 kiểu dữ liệu, 5 seed) có chiều cao trong $[h_{\min}, h_{\max}]$; số lần tách khớp công thức $1 / (f(m - 1))$ tới 3 chữ số.

* **Đọc đĩa:** với $10^6$ khóa, $m = 256$ chỉ cần 3 lần đọc mỗi lần tìm; $m = 3$ cần 16–19 lần; BST không cân bằng cần 25,5 lần với dữ liệu ngẫu nhiên và $\sim 500.000$ lần với dữ liệu có thứ tự.

* **Chọn** $m$**:** $m$ lớn nhất mà một nút vừa một khối đĩa; với khối 4 KB là $m \approx 341$.

* **Điều không khớp lý thuyết:** thời gian CPU tăng nhanh hơn $\log n$ do cache; giải thích ở Mục 8.

## 2. Bài toán và thiết kế

### 2.1. Bài toán

Dữ liệu quá lớn để nằm trong RAM được lưu trên đĩa theo khối; chi phí một thao tác chủ yếu là **số khối phải đọc**. B-Tree cho mỗi nút chứa nhiều khóa (vừa một khối) để cây thấp và rộng. Yêu cầu: cài đặt B-Tree bậc $m$ lưu khóa nguyên với `search`, `insert`, `remove`, duyệt theo thứ tự, `validate`; đo thời gian, chiều cao và số nút được thăm với các kiểu dữ liệu khác nhau; so sánh với BST; giải thích ảnh hưởng của $m$. Đặc tả đầy đủ đầu vào / đầu ra, phạm vi và tiêu chí hoàn thành: [Tuần 1, Mục 2](../week1/report.md#2-phát-biểu-bài-toán-theo-cách-hiểu-của-nhóm).

### 2.2. Định nghĩa và quy ước cài đặt

B-Tree bậc $m$ (Knuth): mỗi nút tối đa $m$ con và $m - 1$ khóa; nút không phải gốc có ít nhất $\lceil m/2 \rceil - 1$ khóa; gốc không phải lá có ít nhất 2 con; nút có $k$ khóa có đúng $k + 1$ con; khóa trong nút tăng dần và phân tách các cây con; mọi lá cùng độ sâu.

| Quy ước | Lựa chọn | 
| ----- | ----- | 
| Chèn | Luôn chèn vào lá; nút có $m$ **khóa** (tràn) thì tách: giữ $\lfloor m/2 \rfloor$ khóa bên trái, đẩy khóa thứ $\lfloor m/2 \rfloor$ (đếm từ 0) lên cha, phần còn lại ($m - \lfloor m/2 \rfloor - 1$ khóa) sang nút mới. Tách có thể lan tới gốc; gốc bị tách thì cây cao thêm 1. | 
| Xóa | Khóa ở nút trong thì thay bằng **khóa liền trước** rồi xóa ở lá. Nút còn ít hơn $\lceil m/2 \rceil - 1$ khóa thì: mượn anh em trái $\to$ mượn anh em phải $\to$ gộp (ưu tiên gộp với anh em trái). Gốc hết khóa thì cây thấp đi 1. | 
| Khóa trùng | Không chèn, trả về `false`. | 
| Tìm trong nút | Tìm nhị phân (`std::lower_bound`). | 

Không dùng cách “tách sớm trên đường đi xuống” của CLRS: cách đó chỉ đúng khi số khóa tối đa là số lẻ ($m$ chẵn), và với $m$ lẻ sinh ra nút 1 khóa hoặc nút rỗng (lỗi AI phát hiện ở [Tuần 2, Mục 5](../week2/report.md#5-chỗ-ai-làm-sai--thiếu-và-cách-phát-hiện)). Lý do chọn B-Tree thay vì BST, AVL / đỏ-đen, bảng băm, B+ Tree: [Tuần 1, Mục 3.5](../week1/report.md#35-lý-do-chọn-b-tree).

## 3. Cài đặt

| Thành phần | Vị trí | Ghi chú | 
| ----- | ----- | ----- | 
| Cấu trúc `Node`, lớp `BTree` | [src/btree.h](../../src/btree.h) | `Node` gồm `vector<int> keys`, `vector<Node*> children`; là lá khi `children` rỗng | 
| `search` | [btree.cpp#L30](../../src/btree.cpp#L30) | Vòng lặp; tùy chọn trả về đường đi | 
| `insert` $\to$ `insertRec`, `splitNode` | [#L83](../../src/btree.cpp#L83), [#L62](../../src/btree.cpp#L62), [#L46](../../src/btree.cpp#L46) | Đệ quy xuống lá, tách khi quay lên | 
| `remove` $\to$ `removeRec`, `fixChild`, `mergeChildren` | [#L172](../../src/btree.cpp#L172), [#L148](../../src/btree.cpp#L148), [#L117](../../src/btree.cpp#L117), [#L103](../../src/btree.cpp#L103) | Sửa thiếu khóa khi quay lên | 
| `traverse`, `height`, `levels` | [#L194](../../src/btree.cpp#L194), [#L200](../../src/btree.cpp#L200), [#L207](../../src/btree.cpp#L207) | `levels()` in cây theo tầng, dùng cho test và `trace` | 
| `validate` | [#L273](../../src/btree.cpp#L273), [`validateRec` #L246](../../src/btree.cpp#L246) | Kiểm tra đủ 5 tính chất, báo tên tính chất vi phạm | 
| Bộ đếm và thống kê | `nodesVisited`, `splits`, `merges`, `borrows`, [`stats` #L236](../../src/btree.cpp#L236) | Đếm đọc đĩa, ghi đĩa (tách / gộp), bộ nhớ, độ lấp đầy | 
| BST không cân bằng | [src/bst.h](../../src/bst.h), [src/bst.cpp](../../src/bst.cpp) | Viết bằng vòng lặp để không tràn stack khi suy biến | 
| Công cụ in từng bước | [tools/trace.cpp](../../tools/trace.cpp) | `trace 4 +10 +20 +30 +40 -20` in cây sau mỗi thao tác | 

## 4. Phân tích độ phức tạp

Ký hiệu: $n$ số khóa, $m$ bậc, $t = \lceil m/2 \rceil$, $h$ chiều cao (số cạnh từ gốc tới lá). Mô hình chi phí có hai phần: **I/O** (số nút đọc / ghi, mỗi nút một trang) và **CPU** (số phép so sánh và dịch phần tử trong RAM).

### 4.1. Chiều cao

Cây thưa nhất: gốc có $\ge 1$ khóa, tầng $i \ge 1$ có $\ge 2t^{i-1}$ nút, mỗi nút $\ge t - 1$ khóa, nên $n \ge 1 + (t - 1) \cdot 2 \cdot (t^h - 1)/(t - 1) = 2t^h - 1$. Cây đầy nhất: $n \le m^{h+1} - 1$. Vậy:

$$
\lceil \log_m(n + 1) \rceil - 1 \le h \le \lfloor \log_t((n + 1)/2) \rfloor, \quad \text{tức } h = \Theta(\log n / \log m).
$$

### 4.2. Từng thao tác

| Thao tác | Đọc nút (I/O) | Ghi nút (I/O) | CPU | Lập luận | 
| ----- | ----- | ----- | ----- | ----- | 
| `search` | $\le h + 1$ | $0$ | $O(\log m \cdot h) = O(\log n)$ | Mỗi tầng một nút; `lower_bound` tốn $O(\log m)$; $\log m / \log t \le$ hằng số vì $t \ge m/2$ | 
| `insert` | $h + 1$ | $1 + 2 \cdot (\text{số lần tách}) + [1 \text{ nếu tạo gốc mới}] \le 2h + 4$ | $O(m \cdot \log_t n)$ | Chèn vào lá dịch $O(m)$; mỗi tầng tách nhiều nhất một nút, mỗi lần $O(m)$ | 
| `remove` | $\le h + 1$ trên đường đi $+ \le 2$ anh em mỗi tầng | $O(h)$ | $O(m \cdot \log_t n)$ | Tìm khóa liền trước đi tiếp cùng nhánh; mỗi tầng `fixChild` một lần: mượn hoặc gộp $O(m)$ | 
| `traverse`, `validate` | mọi nút | $0$ | $O(n)$ | Mỗi nút thăm đúng một lần, mỗi khóa $O(1)$ lần | 
| Bộ nhớ | – | – | $O(n)$ | Số nút $\le 1 + (n - 1)/(t - 1)$, mỗi nút $O(m)$, $m/(t - 1) \le 4$ với $m \ge 3$ | 

Với $m$ cố định, mọi thao tác trên một khóa đều $O(\log n)$ và không bao giờ suy biến, bất kể thứ tự dữ liệu. Đây là khác biệt cốt lõi so với BST: $O(h)$ với $h$ có thể bằng $n - 1$.

### 4.3. Phân tích khấu hao số lần tách

**Chỉ chèn:** mỗi lần tách tạo đúng một nút mới và không có nút nào bị hủy, nên tổng số lần tách $= (\text{số nút}) - 1 \le (n - 1)/(t - 1)$. Do đó số lần tách **khấu hao trên mỗi lần chèn** $\le 1/(t - 1)$, tức $O(1)$ và nhỏ dần khi $m$ tăng, dù một lần chèn đơn lẻ có thể tách tới $h + 1$ nút. Đo được: $\text{số tách} / \text{số chèn} = 1 / (f \cdot (m - 1))$, ví dụ $0,3692$ với $m = 5, n = 10^6$ ngẫu nhiên.

**Chèn xen kẽ xóa:** chặn khấu hao $O(1)$ **không còn đúng** với quy ước của đề tài. Trường hợp E9 / E10 (Mục 5.3) cho thấy một dãy $\{\text{chèn } x; \text{xóa } x\}$ lặp lại có thể gây $\Theta(h)$ lần tách rồi $\Theta(h)$ lần gộp *ở mỗi thao tác*, vì nút ở đúng ngưỡng bị tách rồi gộp lại liên tục (Mục 8.5).

### 4.4. Đối chiếu với số đo

| Dự đoán | Số đo | Nguồn | 
| ----- | ----- | ----- | 
| $h_{\min} \le h \le h_{\max}$ | 80/80 cấu hình $\times$ 5 seed | [results/height_reads.csv](../../results/height_reads.csv) | 
| Tìm không thấy đọc đúng $h + 1$ nút; tìm thấy $\le h + 1$ | Đúng ở mọi dòng; trường hợp xấu nhất đo được $= h + 1$ | như trên | 
| Chèn có thứ tự cho $h = h_{\max}$ ($f = 50\%$) | $m = 3, 5, 16$ chạm đúng $h_{\max}$; $f = 0,500$ với $m$ lẻ | như trên | 
| Chèn ngẫu nhiên: $f \to \ln 2 \approx 0,69$ (Yao, 1978) | $f = 0,670 \dots 0,722$ | [results/m_sweep.csv](../../results/m_sweep.csv) | 
| BST ngẫu nhiên: tìm thấy thăm $\approx 2H_n - 3$ nút | $21,06$ (dự đoán $21,2$) với $n = 10^5$; $25,54$ ($25,8$) với $n = 10^6$ | [results/btree_vs_bst.csv](../../results/btree_vs_bst.csv) | 
| BST có thứ tự: $h = n - 1$ | $h = 99.999$ với $n = 10^5$ | như trên | 
| Bộ nhớ $O(n)$ với mọi $m$ | Mô hình trang cố định: 13–26 MB cho $10^6$ khóa ở mọi $m$ | [results/m_sweep.csv](../../results/m_sweep.csv) | 
| Thời gian search $O(\log n)$ | **Không khớp**: số nút thăm $\times 2,3$ nhưng thời gian $\times 12$ khi $n \times 1000$ (cache, Mục 8.3) | [results/height_reads.csv](../../results/height_reads.csv) | 

## 5. Kiểm thử

### 5.1. Kiểm thử đơn vị và áp lực

| Bộ | Nội dung | Kết quả | 
| ----- | ----- | ----- | 
| [tests/test_btree.cpp](../../tests/test_btree.cpp) | T0–T16 theo [kế hoạch Tuần 1](../week1/test-plan.md): dựng cây Hình 1; tách gốc; chèn khớp bản chạy tay (T4); khóa trùng; tìm ở gốc / lá / không có; duyệt; xóa ở lá, mượn, xóa ở nút trong, gộp dây chuyền; $1 \dots 1000$ tăng dần; trộn ngẫu nhiên đối chiếu `std::set` | **45/45** | 
| [tests/stress_btree.cpp](../../tests/stress_btree.cpp) | 630.000 thao tác ngẫu nhiên, $m = 3 \dots 9$, `validate()` sau mỗi thao tác, đối chiếu `std::set` | Đạt | 

### 5.2. Kiểm chứng trong thực nghiệm

Mọi cây trong thực nghiệm A (80 cấu hình $\times$ 5 lần) `validate()` đúng sau khi xóa một nửa số khóa; trong thực nghiệm B, `traverse()` của B-Tree và BST khớp `std::set` sau toàn bộ dãy thao tác.

### 5.3. Trường hợp biên và đối kháng ([tests/edge_cases.cpp](../../tests/edge_cases.cpp))

| Mã | Trường hợp | Kết quả | 
| ----- | ----- | ----- | 
| E1 | Cây rỗng: `search`, `remove`, `height` $= -1$, `traverse` rỗng, `validate` | PASS | 
| E2 | Chèn 1 khóa rồi xóa $\to$ cây rỗng lại | PASS | 
| E3 | Chèn cùng một khóa 100.000 lần $\to$ `true` đúng 1 lần | PASS | 
| E4 | `INT_MIN`, `INT_MAX`, $0, -1, 1$ ($m = 3$) | PASS | 
| E5 | $m = 2, 1, 0, -5 \to$ ném `invalid_argument` | PASS | 
| E6 | $m = 3, 4, 6$; 5.000 khóa tăng rồi giảm, `validate` sau mỗi lần chèn | PASS | 
| E7 | Xóa hết 20.000 khóa theo 3 thứ tự rồi chèn lại | PASS | 
| E8 | Xóa khóa không có (dưới min, trên max, ở giữa) $\to$ cây không đổi | PASS | 
| E9 | **Đối kháng:** 100.000 lần $\{\text{chèn } 5; \text{xóa } 5\}$ trên `[1 2 3 4]`, $m = 5 \to 100.000$ tách $+ 100.000$ gộp | PASS | 
| E10 | **Đối kháng:** $m = 3$, 2.047 khóa; khóa 773 gây 6 tách mỗi lần chèn, 6 gộp mỗi lần xóa | PASS | 
| E11 | Tìm tuần tự và nhị phân cho cùng cây, cùng số nút thăm | PASS | 
| E12 | $m = 100.001$: cả cây một nút, $h = 0$ | PASS | 
| E13 | BST đối chiếu `std::set` qua 200.000 thao tác | PASS | 
| E14 | BST 20.000 khóa tăng dần, $h = 19.999$, không tràn stack | PASS | 

## 6. Thực nghiệm

Bộ thực nghiệm [bench/bench.cpp](../../bench/bench.cpp), mô tả đầy đủ ở [Tuần 3, Mục 3](../week3/report.md#3-bộ-thực-nghiệm). Khóa là số chẵn (số lẻ dùng cho tìm không thấy); 4 kiểu dữ liệu: ngẫu nhiên, tăng, giảm, zigzag; số lần đọc là số đếm chính xác, thời gian là trung vị nhiều lần chạy trên máy ảo 1 nhân (dao động $\pm 20\%$).

### 6.1. Chèn $\ge 100.000$ khóa: chiều cao và số lần đọc đĩa (yêu cầu GVHD 1)

$n = 1.000.000$ (bảng đủ cho $n = 10^5$ và các kiểu dữ liệu khác: [Tuần 3, Mục 4](../week3/report.md#4-yêu-cầu-gvhd-1-chèn--100000-khóa--chiều-cao-và-số-lần-đọc-đĩa)):

| $m$ | Kiểu | $h$ | $[h_{\min}, h_{\max}]$ | Số nút | Đọc / tìm thấy | Đọc / tìm không thấy | Đọc / xóa | 
| ----- | ----- | ----- | ----- | ----- | ----- | ----- | ----- | 
| 3 | ngẫu nhiên | 15–16 | \[12, 18\] | 745.853 | 16,25 | 17 | 16,05 | 
| 3 | tăng dần | 18 | \[12, 18\] | 999.993 | 18,01 | 19 | 18,05 | 
| 5 | ngẫu nhiên | 10 | \[8, 11\] | 369.197 | 10,63 | 11 | 11,00 | 
| 5 | tăng dần | 11 | \[8, 11\] | 499.993 | 11,50 | 12 | 12,00 | 
| 16 | ngẫu nhiên | 5 | \[4, 6\] | 97.246 | 5,90 | 6 | 6,00 | 
| 64 | ngẫu nhiên | 3 | \[3, 3\] | 22.950 | 3,98 | 4 | 4,00 | 
| 256 | ngẫu nhiên | 2 | \[2, 2\] | 5.642 | 3,00 | 3 | 3,00 | 

### 6.2. So sánh với BST trên cùng dữ liệu, cùng thao tác (yêu cầu GVHD 2)

| $n$ | Kiểu | Cấu trúc | $h$ | Đọc / tìm thấy | Chèn $n$ khóa (ms) | 
| ----- | ----- | ----- | ----- | ----- | ----- | 
| $10^5$ | tăng dần | B-Tree $m = 5$ | 9 | 9,50 | 10,1 | 
|  |  | B-Tree $m = 64$ | 3 | 3,97 | 5,1 | 
|  |  | BST | **99.999** | **49.442** | **63.065** | 
| $10^6$ | ngẫu nhiên | B-Tree $m = 5$ | 10 | 10,62 | 888 | 
|  |  | B-Tree $m = 64$ | 3 | 3,98 | 307 | 
|  |  | BST | 48 | 25,54 | 910 | 

Dữ liệu có thứ tự làm BST suy biến thành danh sách: chèn chậm hơn B-Tree 4.000–6.000 lần. Dữ liệu ngẫu nhiên: BST đọc nhiều nút gấp 2,4 lần (so với $m = 5$) đến 6,4 lần (so với $m = 64$). Bảng đủ: [Tuần 3, Mục 6](../week3/report.md#6-yêu-cầu-gvhd-2-b-tree-so-với-bst-trên-cùng-dữ-liệu-cùng-thao-tác).

### 6.3. Ảnh hưởng của $m$ lên chiều cao, bộ nhớ và số lần đọc đĩa (yêu cầu GVHD 3)

| $m$ | $h$ | RAM (MB) | Đĩa (MB) | Trang (byte) | Trang đọc / lần tìm | KB đọc / lần tìm | 
| ----- | ----- | ----- | ----- | ----- | ----- | ----- | 
| 3 | 16 | 49,9 | 25,6 | 36 | 16,26 | 0,6 | 
| 16 | 5 | 13,5 | 17,8 | 192 | 5,90 | 1,1 | 
| 64 | 3 | 9,5 | 16,8 | 768 | 3,98 | 3,0 | 
| **341** | 2 | 8,3 | 16,2 | **4.092** | **3,00** | 12,0 | 
| 4096 | 1 | 6,7 | 13,4 | 49.152 | 2,00 | 96,0 | 

* **Chiều cao** giảm theo $\log_m n$: bình phương $m$ thì $h$ giảm khoảng một nửa.

* **Bộ nhớ** là $O(n)$ với mọi $m$. $m$ nhỏ tốn hơn vì tỉ lệ con trỏ trên khóa cao và chi phí cố định mỗi nút; trong RAM, $m = 3$ tốn gấp 6 lần $m = 341$.

* **Đọc đĩa:** số trang đọc giảm theo $m$, nhưng số byte đọc tăng. Đĩa đọc theo khối nên chi phí là số lần truy cập; chọn $m$ lớn nhất để một nút vừa một khối: $12m \le 4096 \Rightarrow m = 341$.

## 7. Thử cải tiến: duyệt tuần tự thay cho tìm nhị phân trong nút

Số nút thăm không đổi (E11), chỉ thời gian CPU thay đổi. Duyệt tuần tự nhanh hơn 5–15% với $16 \le m \le 256$, ngang nhau với $m \le 8$, chậm hơn 1,24 lần ($m = 512$) và 2,01 lần ($m = 1024$). **Loại bỏ**: lợi ích vài chục nanô giây không đáng kể so với một lần đọc đĩa (0,1–10 ms), và vùng $m$ hợp lý cho đĩa ($\approx 341$) nằm gần điểm giao nhau. Số liệu: [Tuần 3, Mục 10](../week3/report.md#10-thử-cải-tiến-duyệt-tuần-tự-thay-cho-tìm-nhị-phân-trong-nút).

## 8. Những điều bất ngờ và giải thích

**8.1. Dữ liệu tăng dần làm cây cao hơn nhưng chèn lại nhanh hơn (tới 6–8 lần khi** $n = 10^6$**).** Ví dụ $n = 10^6, m = 5$: `asc` chèn 135 ns/khóa, `random` 807 ns/khóa, dù cây `asc` cao hơn (11 so với 10). Chênh lệch tăng theo $n$ (1,3–4,5 lần ở $n = 10^3$, 6–8 lần ở $n = 10^6$), đúng như dự đoán nếu nguyên nhân là cache. Lý do: khóa tăng dần luôn đi theo cùng một đường bên phải, nên các nút trên đường đó luôn nằm sẵn trong cache CPU; khóa ngẫu nhiên mỗi lần đi một đường khác, gần như mọi nút đều trượt cache. Nếu là đĩa thật, điều tương tự xảy ra với bộ đệm trang của hệ điều hành. Vì vậy cơ sở dữ liệu thường chèn khóa tăng dần (ví dụ khóa tự tăng).

**8.2.** $m$ **chẵn: dữ liệu tăng và giảm cho cây khác nhau.** Với $m = 16, n = 10^5$: `asc` cho $h = 4, f = 0,533$; `desc` cho $h = 5, f = 0,467$. Với $m$ lẻ hai kiểu cho kết quả giống hệt. Lý do nằm ở quy tắc tách (Mục 2.2): nút tràn có $m = 16$ khóa, giữ $m/2 = 8$ khóa bên trái, đẩy khóa thứ 9 lên, còn 7 khóa bên phải. Khi chèn tăng dần, nút bên trái (8 khóa) bị bỏ lại, nên $f = 8/15 = 0,533$. Khi chèn giảm dần, khóa mới luôn vào bên trái, nên nút bên phải (7 khóa, đúng mức tối thiểu) bị bỏ lại, $f = 7/15 = 0,467$ và cây cao thêm một tầng. Với $m$ lẻ, hai nửa bằng nhau ($(m - 1)/2$ khóa) nên không lệch. Điều này không có trong lý thuyết chung về B-Tree vì phụ thuộc quy ước tách của cài đặt.

**8.3. Thời gian tìm tăng nhanh hơn** $\log n$**.** Số nút thăm tăng 2,3 lần khi $n$ tăng 1000 lần (đúng $\log$), nhưng thời gian tăng 12 lần. Mô hình RAM của phân tích độ phức tạp coi mọi truy cập bộ nhớ có giá như nhau. Thực tế: với $10^3$ khóa, cả cây ($\sim 30$ KB) nằm trong cache L1/L2; với $10^6$ khóa ($\sim 29$ MB) cây lớn hơn cache, nên mỗi nút ở tầng dưới là một lần trượt cache ($\sim 100$ ns). Bộ nhớ chính đóng vai trò “đĩa” đối với cache. Đây chính là lý do B-Tree cũng được dùng làm cấu trúc trong RAM: $m$ **lớn giảm số lần trượt cache** giống như giảm số lần đọc đĩa (Mục 6.3).

**8.4. Trong RAM, BST ngẫu nhiên nhanh ngang (hoặc hơn) B-Tree** $m = 5$ **dù thăm nhiều nút gấp 2,4 lần.** Lý do: một nút BST rất nhỏ và so sánh ở mỗi nút chỉ một phép, trong khi mỗi nút B-Tree $m = 5$ cần `lower_bound` trên một `vector` riêng (thêm một lần nhảy con trỏ tới vùng nhớ của `vector`) và chèn phải dịch phần tử. Lợi thế của B-Tree chỉ hiện ra khi **chi phí mỗi lần thăm nút lớn** (đọc đĩa), hoặc khi $m$ đủ lớn ($m = 64$ tìm nhanh hơn BST 1,8 lần ở $n = 10^5$ và 4,7 lần ở $n = 10^6$). Đây đúng là lý do đề tài gọi là “disk-oriented”.

**8.5. Không có “vùng đệm” giữa tách và gộp.** Trường hợp E10 (Mục 5.3): ở một cây 2.047 khóa với $m = 3$, chèn rồi xóa cùng một khóa lặp lại 10.000 lần gây **6 lần tách cho mỗi lần chèn và 6 lần gộp cho mỗi lần xóa**, thay vì 0 hay 1. E9 cho kết quả tương tự ở cây nhỏ: 100.000 tách và 100.000 gộp. Nguyên nhân: quy ước của chúng em gộp ngay khi nút xuống dưới mức tối thiểu và tách ngay khi tràn, nên một nút ở đúng ngưỡng bị tách rồi gộp lại liên tục. Với dữ liệu ngẫu nhiên điều này hiếm (số gộp/lần xóa trung bình $\sim 0,7$ ở $m = 3$ và $\sim 0,005$ ở $m = 256$), nhưng một dãy thao tác đối kháng có thể làm mỗi thao tác tốn $h$ lần ghi đĩa. Cách khắc phục (chưa cài đặt, để ngoài phạm vi): chỉ gộp khi nút trống hẳn hoặc dưới một ngưỡng thấp hơn (“lazy deletion”), hoặc chia khóa sang anh em trước khi tách ($\text{B}^*$-Tree).

## 9. Hạn chế

1. **Không có đĩa thật.** Cây nằm trong RAM; “đọc đĩa” là số nút được thăm. Mô hình bỏ qua bộ đệm trang (thực tế gốc và tầng 1 thường nằm sẵn trong RAM), nên số đo là cận trên của số lần đọc thật, và không đo được độ trễ thật của HDD / SSD. Các con số mili giây cho đĩa trong báo cáo là ước tính từ giả định.

2. **Bộ đếm khi xóa đếm thiếu.** `nodesVisited()` chỉ đếm các nút trên đường đi xuống; các nút anh em mà `fixChild` đọc để mượn / gộp và đường đi tìm khóa liền trước (đi hai lần trên cùng nhánh) không được đếm. Số “đọc / xóa” ở Mục 6 vì vậy thấp hơn thực tế. Số lần ghi đĩa chỉ được suy ra từ số tách / gộp / mượn, không đếm trực tiếp.

3. **Khóa chỉ là `int`, không có giá trị kèm theo, không trùng khóa.** Một chỉ mục thật lưu cặp (khóa, con trỏ bản ghi) và thường cho phép trùng khóa.

4. **Không có truy vấn khoảng, nạp hàng loạt (bulk loading), hay đa luồng.**

5. **Không có khấu hao** $O(1)$ **khi chèn xen kẽ xóa** (Mục 4.3, 8.5): một dãy đối kháng làm mỗi thao tác tốn $\Theta(h)$ lần tách / gộp.

6. **Nút dùng `std::vector`**, nên bộ nhớ RAM có phần thừa (dung lượng `vector`) và mỗi nút là hai vùng nhớ riêng; đây không phải bố cục một trang liền mạch như trên đĩa. Số bộ nhớ không tính chi phí của bộ cấp phát.

7. **So sánh với BST chỉ dùng BST không cân bằng** như đề yêu cầu; cây cân bằng (`std::set`, đỏ-đen) chỉ được đo thời gian, không đếm được số nút thăm.

8. **Thời gian đo trên máy ảo 1 nhân**, dao động $\pm 20\%$; chỉ nên so sánh thời gian trong cùng một bảng. Số lần đọc thì chính xác và chạy lại cho ra đúng như cũ.

## 10. Ánh xạ yêu cầu $\to$ vị trí trong code và báo cáo

| \# | Yêu cầu | Code | Kiểm chứng | Báo cáo | 
| ----- | ----- | ----- | ----- | ----- | 
| 1 | Tìm kiếm, trả về có/không và đường đi | [`BTree::search` btree.cpp#L30](../../src/btree.cpp#L30) | T6–T8 | Mục 4.2 | 
| 2 | Chèn, giữ đủ tính chất (tách khi tràn) | [`insert` #L83](../../src/btree.cpp#L83), [`insertRec` #L62](../../src/btree.cpp#L62), [`splitNode` #L46](../../src/btree.cpp#L46) | T1–T5, T15, E3, E6 | Mục 2.2, 4.2 | 
| 3 | Xóa, giữ đủ tính chất (mượn / gộp) | [`remove` #L172](../../src/btree.cpp#L172), [`removeRec` #L148](../../src/btree.cpp#L148), [`fixChild` #L117](../../src/btree.cpp#L117), [`mergeChildren` #L103](../../src/btree.cpp#L103) | T10–T14, E7, E8 | Mục 2.2, 4.2 | 
| 4 | Duyệt theo thứ tự | [`traverse` #L194](../../src/btree.cpp#L194) | T9, stress | Mục 4.2 | 
| 5 | Kiểm tra hợp lệ đủ 5 tính chất | [`validate` #L273](../../src/btree.cpp#L273), [`validateRec` #L246](../../src/btree.cpp#L246) | Gọi sau mọi thao tác trong stress, E6 | Mục 2.2 | 
| 6 | Mô phỏng số lần đọc đĩa | `nodesVisited` (btree.cpp#L33, #L63, #L149) | T6–T8, E12 | Mục 4.2, 9 | 
| 7 | Chèn $\ge 100.000$ khóa, báo $h$ và số lần đọc với $\ge 3$ giá trị $m$ | [`expHeight`](../../bench/bench.cpp), [results/height_reads.csv](../../results/height_reads.csv) | 80/80 trong cận | Mục 6.1 | 
| 8 | So sánh BST cùng dữ liệu, cùng thao tác | [src/bst.cpp](../../src/bst.cpp), `expBst` trong [bench.cpp](../../bench/bench.cpp) | E13, E14 | Mục 6.2 | 
| 9 | Ảnh hưởng của $m$ lên $h$, bộ nhớ, đọc đĩa | [`stats` #L236](../../src/btree.cpp#L236), `expMSweep` | – | Mục 6.3 | 
| 10 | Thực nghiệm so với lý thuyết | [bench/plot.py](../../bench/plot.py) | – | Mục 4.4 | 
| 11 | Dữ liệu đa dạng và đối kháng | `makeKeys` (4 kiểu), [tests/edge_cases.cpp](../../tests/edge_cases.cpp) | E1–E14 | Mục 5.3, 6 | 
| 12 | Thử một cải tiến, giữ hoặc loại có số liệu | `setNodeSearch`, [`lowerIndex` #L20](../../src/btree.cpp#L20), `expOpt` | E11 | Mục 7 | 
| 13 | Ví dụ chạy tay và demo | [src/main.cpp](../../src/main.cpp), [tools/trace.cpp](../../tools/trace.cpp) | T4 | [Tuần 1, Mục 4](../week1/report.md#4-ví-dụ-chạy-tay), Mục 12.2 | 
| 14 | Chạy lại được toàn bộ | [run_all.sh](../../run_all.sh), [run_all.bat](../../run_all.bat), [README](../../README.md) | – | Mục 13 | 
| 15 | Nhật ký sử dụng AI | [docs/ai-log.md](../ai-log.md) | – | Mục 11 | 

## 11. Sử dụng AI

| Tuần | Công cụ | Phần AI làm | Lỗi AI đã phát hiện và cách xử lý | 
| ----- | ----- | ----- | ----- | 
| 1 | Gemini 3.6 | Giải thích định nghĩa và ứng dụng B-Tree | Có đoạn chép nguyên văn, sinh viên viết lại theo cách hiểu riêng ([ai-log, Tuần 1](../ai-log.md#tuần-1)) | 
| 2 | Gemini Flash 3.6 | Bản cài đặt B-Tree ban đầu | Tách sớm kiểu CLRS sai với $m$ lẻ (sinh nút 1 khóa ở $m = 5$, phát hiện nhờ T4); `validate` thiếu 3 tính chất; `visited_` không tăng; không biên dịch được; `remove` chưa có ([Tuần 2, Mục 5](../week2/report.md#5-chỗ-ai-làm-sai--thiếu-và-cách-phát-hiện)) | 
| 3 | Gemini 3 Flash | Khung `bench.cpp`, `plot.py`, `bst.cpp`, `edge_cases.cpp` (E1–E14) và gợi ý đối chiếu lý thuyết | 4 lỗi: (1) E10 chèn tăng dần với $m = 3$ cho $f = 0,5$ nên ra 0 lần tách $\to$ sửa thành dựng cây ngẫu nhiên và quét tìm khóa lẻ gây tách nhiều nhất (773); (2) Thực nghiệm A chỉ ghi $h$ của lần chạy cuối $\to$ sửa thành lưu $[h_{\text{obs\_lo}}, h_{\text{obs\_hi}}]$; (3) Thực nghiệm B đo $h$ của BST sau khi xóa $\to$ dời lên ngay sau khi chèn; (4) dùng `std::system("mkdir -p")` bị cảnh báo khi bật `-Werror` $\to$ đổi sang `std::filesystem::create_directories` ([Tuần 3, Mục 11](../week3/report.md#11-chỗ-ai-làm-sai--thiếu-trong-tuần-3-và-cách-phát-hiện)) | 
| 4 | Gemini 3 Flash | Khung báo cáo tổng, `README.md`, script `run_all` và câu hỏi gợi ý ở Mục 12 | Sinh viên tự rà soát số liệu giữa các bảng, tự viết toàn bộ Mục 12 và chạy tay kiểm chứng lại bằng công cụ `trace` | 

## 12. Phần sinh viên tự viết

> Phần này dùng để chứng minh em hiểu đề tài, nên em tự viết và tự chạy tay. Công cụ `trace` chỉ dùng để **kiểm tra lại sau khi** đã chạy tay xong.

### 12.1. Self-Explanation cho từng yêu cầu

Dưới đây là giải thích của em cho từng mục từ 1 đến 15 trong bảng ánh xạ ở Mục 10:

 1. **Tìm kiếm (`BTree::search`):**

    * *Code làm gì:* Bắt đầu từ `root_`, ở mỗi nút tăng biến đếm `visited_++` (và đẩy chỉ số vào vector `path` nếu người gọi truyền vào), sau đó gọi `lowerIndex` để tìm vị trí đầu tiên `idx` mà `keys[idx] >= key`. Nếu `idx < keys.size()` và `keys[idx] == key` thì trả về `true`; nếu nút hiện tại là lá thì dừng và trả về `false`; ngược lại đi xuống con `children[idx]`.

    * *Vì sao đúng:* Do các khóa trong mỗi nút tăng dần và khóa `keys[idx]` là cận trên chặt của cây con `children[idx]` đồng thời là cận dưới chặt của `children[idx + 1]`, nên nếu `key` tồn tại trong cây mà không nằm ở nút hiện tại thì nó bắt buộc phải nằm trong cây con `children[idx]`.

    * *Một chỗ dễ sai:* Kiểm tra `node->keys[idx] == key` trước khi kiểm tra `idx < node->keys.size()`. Khi `key` lớn hơn mọi khóa trong nút, `lowerIndex` trả về `keys.size()`, truy cập `keys[idx]` sẽ đọc tràn bộ nhớ (out-of-bounds).

 2. **Chèn và tách khi tràn (`insert`, `insertRec`, `splitNode`):**

    * *Code làm gì:* Đi đệ quy từ gốc xuống đúng nút lá cần chèn (nếu gặp khóa trùng ở bất kỳ tầng nào thì trả về `false` ngay). Sau khi chèn khóa vào lá theo thứ tự tăng dần, trên đường quay lui đệ quy lên, nếu nút con `children[idx]` bị tràn (có đủ $m$ khóa) thì gọi `splitNode` tách nút đó thành 2 nút: nút trái giữ $\lfloor m/2 \rfloor$ khóa đầu, khóa ở chỉ số $\lfloor m/2 \rfloor$ được đẩy lên chèn vào nút cha, và nút phải mới tạo giữ $m - \lfloor m/2 \rfloor - 1$ khóa cuối cùng (kèm nửa số con tương ứng nếu không phải lá). Nếu `root_` bị tràn sau cùng thì tạo nút gốc mới có 1 khóa và 2 con.

    * *Vì sao đúng:* Với mọi $m \ge 3$, khi nút có đúng $m$ khóa, số khóa bên trái là $\lfloor m/2 \rfloor \ge \lceil m/2 \rceil - 1$ và số khóa bên phải là $m - 1 - \lfloor m/2 \rfloor = \lceil m/2 \rceil - 1$, nên cả hai nút sau khi tách đều thỏa mãn điều kiện số khóa tối thiểu của Knuth. Việc tăng chiều cao chỉ diễn ra khi tách gốc nên mọi lá luôn cùng độ sâu.

    * *Một chỗ dễ sai:* Dùng cách tách sớm (proactive split) của CLRS khi nút có $m - 1$ khóa: với $m$ lẻ (như $m = 5$), $m - 1 = 4$ khóa khi tách đẩy 1 khóa lên cha chỉ còn 3 khóa chia đôi thành 2 và 1, làm nút 1 khóa vi phạm ngưỡng tối thiểu $\lceil 5/2 \rceil - 1 = 2$ khóa.

 3. **Xóa, mượn và gộp (`remove`, `removeRec`, `fixChild`, `mergeChildren`):**

    * *Code làm gì:* Nếu khóa cần xóa nằm ở nút lá thì xóa trực tiếp khỏi vector `keys`. Nếu khóa nằm ở nút trong tại vị trí `idx`, code tìm khóa liền trước (khóa lớn nhất ở nút lá phải cùng của cây con trái `children[idx]`), chép đè lên `keys[idx]`, rồi gọi đệ quy xóa khóa liền trước đó trong `children[idx]`. Khi đệ quy quay lên, nếu nút con `children[idx]` có ít hơn $\lceil m/2 \rceil - 1$ khóa, `fixChild` được gọi theo thứ tự: (1) mượn từ anh em trái `idx - 1` nếu anh em trái dư khóa, (2) mượn từ anh em phải `idx + 1` nếu anh em phải dư khóa, (3) nếu cả hai đều ở mức tối thiểu thì gộp với anh em trái (hoặc phải nếu `idx == 0`) qua `mergeChildren`. Cuối cùng ở hàm ngoài, nếu `root_` hết khóa và không phải lá thì đôn `root_->children[0]` lên làm gốc mới và giải phóng gốc cũ.

    * *Vì sao đúng:* Khóa liền trước luôn nằm ở lá và lớn hơn mọi khóa khác ở cây con trái nhưng nhỏ hơn mọi khóa ở cây con phải, nên thay thế xong vẫn giữ nguyên thứ tự khóa. Khi mượn (xoay khóa qua cha), số khóa của nút thiếu tăng 1 và của anh em giảm 1 nên đều hợp lệ. Khi gộp, hai nút có $(\lceil m/2 \rceil - 2)$ và $(\lceil m/2 \rceil - 1)$ khóa cộng thêm 1 khóa từ cha thành $2\lceil m/2 \rceil - 2 \le m - 1$ khóa, vừa khít trong một nút mà không bao giờ gây tràn.

    * *Một chỗ dễ sai:* Khi mượn hoặc gộp ở nút trong (internal node), chỉ di chuyển khóa mà quên di chuyển con trỏ `children` đi kèm, hoặc sau khi gộp ở gốc làm gốc rỗng (`keys.empty()`) nhưng quên cập nhật `root_ = root_->children[0]` khiến cây có gốc 0 khóa.

 4. **Duyệt theo thứ tự (`traverse`):**

    * *Code làm gì:* Duyệt đệ quy theo thứ tự trung thứ tự tổng quát (in-order): tại một nút có $k$ khóa, lặp $i$ từ $0$ đến $k - 1$, nếu không phải lá thì duyệt `children[i]`, sau đó ghi `keys[i]` vào mảng kết quả; sau khi hết vòng lặp thì duyệt nốt cây con cuối cùng `children[k]`.

    * *Vì sao đúng:* Theo bất biến phân tách khóa của B-Tree, mọi khóa trong `children[i]` đều nhỏ hơn `keys[i]` và mọi khóa trong `children[i + 1]` đều lớn hơn `keys[i]`, nên quy nạp theo chiều cao cho ra dãy tăng dần nghiêm ngặt chứa toàn bộ khóa trong cây.

    * *Một chỗ dễ sai:* Đặt lệnh duyệt `children[k]` bên trong vòng lặp `for (int i = 0; i < k; ++i)` hoặc quên duyệt `children[k]` sau vòng lặp, dẫn đến bỏ sót toàn bộ nhánh ngoài cùng bên phải của mọi nút.

 5. **Kiểm tra hợp lệ đủ 5 tính chất (`validate`, `validateRec`):**

    * *Code làm gì:* Duyệt toàn bộ cây một lần và kiểm tra cả 5 điều kiện của B-Tree bậc $m$: (1) khóa trong mỗi nút tăng dần nghiêm ngặt (`keys[i - 1] < keys[i]`); (2) số khóa của mọi nút khác gốc nằm trong $[\lceil m/2 \rceil - 1, m - 1]$, gốc có từ $1$ đến $m - 1$ khóa; (3) nút không phải lá có $k$ khóa thì có đúng $k + 1$ con khác `nullptr`; (4) mọi nút lá có cùng độ sâu `leafDepth` (lưu độ sâu của lá đầu tiên gặp được rồi so sánh với mọi lá sau); (5) mọi khóa trong nút đều nằm chặt trong khoảng `(low, high)` do cha truyền xuống.

    * *Vì sao đúng:* Năm điều kiện này là điều kiện cần và đủ trong định nghĩa B-Tree của Knuth. Bất kỳ sai sót nào trong tách, mượn, gộp hoặc xoay con trỏ đều làm vi phạm ít nhất một trong 5 tính chất này.

    * *Một chỗ dễ sai:* Dùng kiểu `int` cho hai cận `low` và `high` truyền xuống đệ quy: khi cây chứa khóa biên `INT_MIN` hoặc `INT_MAX` (như ở test E4), ta không có giá trị `int` nào nhỏ hơn `INT_MIN` để làm cận khởi tạo ban đầu. Vì vậy trong code em phải dùng kiểu `long long` (`LLONG_MIN`, `LLONG_MAX`) cho khoảng biên.

 6. **Mô phỏng số lần đọc đĩa (`nodesVisited`):**

    * *Code làm gì:* Mỗi khi bắt đầu xử lý một nút trong `search`, `insertRec` hoặc `removeRec`, biến thành viên `visited_` được tăng lên 1. Hàm `resetCounters()` đặt lại bộ đếm về 0 trước mỗi đợt đo.

    * *Vì sao đúng:* Trong mô hình lưu trữ hướng đĩa, mỗi nút của B-Tree tương ứng với một trang đĩa (disk page). Muốn đọc khóa hay con trỏ trong một nút, hệ thống phải nạp trang đó vào bộ nhớ, nên đếm số nút đi qua phản ánh trực tiếp số trang cần đọc trên đường đi từ gốc xuống lá.

    * *Một chỗ dễ sai:* Khai báo hàm `search(...) const` nhưng quên từ khóa `mutable` cho biến `visited_`, làm trình biên dịch báo lỗi không cho tăng biến đếm trong hàm hằng, hoặc quên tăng `visited_` ở nút cuối cùng khi tìm thấy khóa.

 7. **Thực nghiệm chiều cao và đọc đĩa với** $\ge 100.000$ **khóa (`expHeight`):**

    * *Code làm gì:* Chạy tự động qua các kích thước $n = 10^3, 10^4, 10^5, 10^6$, các bậc $m \in \{3, 5, 16, 64, 256\}$ và 4 thứ tự dữ liệu (`random`, `asc`, `desc`, `zigzag`), mỗi cấu hình lặp 5 seed khác nhau. Đo chiều cao $[h_{\text{obs\_lo}}, h_{\text{obs\_hi}}]$, tính cận lý thuyết $[h_{\min}, h_{\max}]$, đếm số lần đọc trung bình cho chèn, tìm thấy (khóa chẵn), tìm không thấy (khóa lẻ) và xóa $n/2$ khóa, rồi xuất ra `results/height_reads.csv`.

    * *Vì sao đúng:* Việc dùng toàn số chẵn để chèn vào cây đảm bảo 100% các truy vấn số lẻ đều là tìm kiếm không thành công (đi tới tận lá), tách biệt hoàn toàn hai trường hợp tìm thấy và tìm không thấy.

    * *Một chỗ dễ sai:* Lấy trung vị số lần đọc của 5 lần chạy nhưng chỉ in chiều cao $h$ của lần chạy cuối cùng (lỗi AI ở Mục 11 Tuần 3): khi $m = 3$ ngẫu nhiên, $h$ dao động giữa 12 và 13 theo seed, làm dòng CSV có $h = 12$ nhưng số lần đọc khi tìm không thấy lại là $14 \neq 12 + 1$.

 8. **So sánh B-Tree với BST trên cùng dữ liệu, cùng thao tác (`src/bst.cpp`, `expBst`):**

    * *Code làm gì:* Cài đặt cây nhị phân tìm kiếm `BST` không tự cân bằng với cùng giao diện và quy ước như `BTree` (tăng `visited_` mỗi nút đi qua, xóa nút 2 con bằng khóa liền trước), sau đó chạy cả `BTree` ($m = 5, 64$), `BST` và `std::set` trên cùng mảng khóa đầu vào, cùng danh sách khóa tìm kiếm và cùng danh sách khóa xóa.

    * *Vì sao đúng:* Khi cố định cùng một dãy khóa và thao tác, mọi sự khác biệt về chiều cao, số nút thăm và thời gian đều đến từ bản chất cấu trúc dữ liệu (cây tự cân bằng nhiều nhánh so với cây nhị phân không cân bằng), không bị nhiễu bởi dữ liệu ngẫu nhiên khác nhau.

    * *Một chỗ dễ sai:* Viết `BST` bằng đệ quy thông thường: khi chèn $100.000$ khóa tăng dần (`asc`), cây suy biến thành danh sách liên kết có độ sâu $99.999$, gọi đệ quy sẽ làm tràn bộ nhớ ngăn xếp (stack overflow) ngay lập tức. Do đó toàn bộ `BST` phải viết bằng vòng lặp `while` và dùng hàng đợi/ngăn xếp trên Heap.

 9. **Đo ảnh hưởng của** $m$ **lên chiều cao, bộ nhớ và đọc đĩa (`stats`, `expMSweep`):**

    * *Code làm gì:* Hàm `stats()` duyệt qua mọi nút để đếm tổng số nút, số lá, độ lấp đầy $f = \frac{n}{\text{nodes} \cdot (m - 1)}$, bộ nhớ RAM thực tế (`sizeof(Node)` cộng `capacity()` của 2 vector) và bộ nhớ đĩa giả định mỗi trang cố định $4 + 4(m - 1) + 8m = 12m$ byte. Thực nghiệm `expMSweep` quét $m$ từ $3$ đến $4096$ trên $10^6$ khóa ngẫu nhiên.

    * *Vì sao đúng:* Công thức $12m$ byte phản ánh đúng bố cục một nút trên đĩa 64-bit (4 byte lưu số khóa hiện có, $m - 1$ khóa kiểu `int` 4 byte, $m$ địa chỉ trang con 8 byte). Nhân số trang đọc với kích thước trang cho phép quan sát sự đánh đổi giữa số lần truy cập đĩa (giảm theo $m$) và lượng byte phải truyền (tăng theo $m$).

    * *Một chỗ dễ sai:* Tính bộ nhớ RAM bằng `keys.size()` thay vì `keys.capacity()`: vì `std::vector` cấp phát dư dung lượng khi nhân đôi, dùng `size()` sẽ bỏ sót phần bộ nhớ thừa mà `vector` đang giữ trong RAM.

10. **Đối chiếu thực nghiệm với lý thuyết (`bench/plot.py`):**

    * *Code làm gì:* Đọc các file CSV trong `results/`, tính các đường lý thuyết ($h_{\min}, h_{\max}$, độ lấp đầy $\ln 2$, tỉ lệ tách $1/(f(m-1))$, số nút thăm $2H_n - 3$ của BST) và vẽ 5 biểu đồ so sánh trực quan vào `docs/week3/images/`.

    * *Vì sao đúng:* Kết hợp cả kiểm tra tự động bằng số (trong bảng Mục 4.4) lẫn biểu đồ log-scale giúp phát hiện ngay lập tức nếu có bất kỳ điểm dữ liệu nào vượt ra ngoài cận lý thuyết hoặc lệch quy luật tiệm cận.

    * *Một chỗ dễ sai:* Nhầm lẫn giữa chiều cao tính theo số nút và chiều cao tính theo số cạnh: trong đề tài này $h$ là số cạnh từ gốc tới lá (cây 1 nút có $h = 0$), nên số nút đọc khi đi từ gốc tới lá phải là $h + 1$ chứ không phải $h$.

11. **Dữ liệu đa dạng và trường hợp đối kháng (`makeKeys`, `tests/edge_cases.cpp`):**

    * *Code làm gì:* Hàm `makeKeys` sinh 4 kiểu phân bố (`random`, `asc`, `desc`, `zigzag`); file `edge_cases.cpp` kiểm tra 14 trường hợp biên và đối kháng (E1–E14), đặc biệt là E9 và E10 lặp đi lặp lại cặp thao tác $\{\text{chèn } x; \text{xóa } x\}$ ngay tại ngưỡng tách – gộp.

    * *Vì sao đúng:* Dữ liệu ngẫu nhiên chỉ kiểm tra được trường hợp trung bình ($f \approx 69\%$). Các mẫu có thứ tự và đối kháng mới ép được cây rơi vào trạng thái cực단 (độ lấp đầy tối thiểu $50\%$, suy biến ở BST, hoặc tách/gộp dây chuyền từ lá lên gốc ở B-Tree).

    * *Một chỗ dễ sai:* Ở E10, nghĩ rằng chèn tăng dần $2.047$ khóa vào B-Tree $m = 3$ sẽ tạo ra cây đầy 2 khóa mỗi nút (giống cây nhị phân hoàn hảo), nhưng thực tế chèn tăng dần với $m = 3$ làm các nút bị bỏ lại chỉ có 1 khóa, nên chèn thêm không gây ra lần tách nào.

12. **Thử cải tiến tìm kiếm trong nút (`setNodeSearch`, `lowerIndex`, `expOpt`):**

    * *Code làm gì:* Cho phép chuyển đổi hàm `lowerIndex` giữa hai chế độ: `Binary` (`std::lower_bound`, $O(\log m)$) và `Linear` (vòng lặp `while (i < n && keys[i] < key) ++i`, $O(m)$), sau đó đo thời gian tìm kiếm trên $10^6$ khóa với $m = 3 \dots 1024$ và kiểm chứng qua E11 rằng cả hai chế độ tạo ra cùng một cây và cùng số nút thăm.

    * *Vì sao đúng:* Cả hai cách đều trả về chỉ số nhỏ nhất `i` thỏa `keys[i] >= key` (hoặc `keys.size()` nếu không có), nên luồng điều khiển của `search`, `insert`, `remove` hoàn toàn bất biến; sự khác biệt chỉ nằm ở số lệnh CPU và dự đoán nhánh bên trong một nút.

    * *Một chỗ dễ sai:* Kết luận vội vàng dựa trên vài giá trị $m$ nhỏ rằng duyệt tuần tự tốt hơn mà không quét tới $m \ge 512$ và không đặt vào bối cảnh chi phí đọc đĩa (nơi vài chục nanô giây CPU không có ý nghĩa so với hàng mili giây I/O).

13. **Ví dụ chạy tay, demo và công cụ `trace` (`src/main.cpp`, `tools/trace.cpp`):**

    * *Code làm gì:* `main.cpp` chạy lại chính xác dãy thao tác của ví dụ chạy tay ở Tuần 1 và in cấu trúc cây theo từng tầng qua `levels()`. Công cụ `trace.cpp` nhận bậc $m$ và dãy lệnh dạng `+khóa` (chèn) hoặc `-khóa` (xóa) từ dòng lệnh, thực hiện từng bước, in cây và kiểm tra `validate()` sau mỗi thao tác.

    * *Vì sao đúng:* In toàn bộ các nút theo từng tầng (`levels()`) cho phép đối chiếu 1–1 từng khóa và từng nút giữa hình vẽ bằng tay trên giấy với trạng thái thực tế trong bộ nhớ.

    * *Một chỗ dễ sai:* Quy ước khác nhau về chỉ số khóa đẩy lên khi $m$ chẵn giữa lúc chạy tay và lúc viết code (ví dụ chạy tay đẩy khóa thứ $\lfloor (m-1)/2 \rfloor$ nhưng code đẩy khóa thứ $\lfloor m/2 \rfloor$), dẫn đến hai cây đều hợp lệ nhưng hình dạng lệch nhau.

14. **Tự động hóa chạy lại toàn bộ (`run_all.sh`, `run_all.bat`, `README.md`):**

    * *Code làm gì:* Biên dịch toàn bộ mã nguồn với cờ tối ưu `-O2`, chạy lần lượt chương trình demo, 3 bộ kiểm thử (`test_btree`, `stress_btree`, `edge_cases`), chạy bộ thực nghiệm `bench_btree` xuất CSV và gọi `plot.py` vẽ lại toàn bộ hình ảnh. Hỗ trợ cờ `--quick` để chạy kiểm chứng nhanh trong khoảng 4 phút vào thư mục riêng `results/quick/`.

    * *Vì sao đúng:* Đảm bảo tính tái lập (reproducibility): giảng viên hoặc bất kỳ ai clone kho mã nguồn về đều có thể kiểm chứng từ unit test đến biểu đồ chỉ bằng một lệnh duy nhất trên cả Linux/macOS và Windows.

    * *Một chỗ dễ sai:* Để chế độ chạy nhanh `--quick` ghi đè trực tiếp lên các file CSV chính thức trong `results/`, làm mất số liệu chuẩn của $n = 10^6$ khóa.

15. **Nhật ký sử dụng AI (`docs/ai-log.md`):**

    * *Code làm gì:* Ghi lại đầy đủ theo từng tuần các prompt đã dùng, phần nội dung do AI sinh ra, các lỗi sai của AI mà em phát hiện được trong quá trình kiểm thử / đối chiếu lý thuyết, và cách em tự sửa lại.

    * *Vì sao đúng:* Giúp phân định minh bạch phần việc hỗ trợ của công cụ và phần tư duy kiểm chứng, sửa lỗi, phân tích của sinh viên; mỗi lỗi của AI đều gắn với một bài test cụ thể (như T4 ở Tuần 2 hay E10 ở Tuần 3).

    * *Một chỗ dễ sai:* Tin tưởng tuyệt đối vào code hoặc test do AI sinh ra mà không viết kiểm tra bất biến (`validate()`, kiểm tra cận lý thuyết trên CSV) để đối chứng.

### 12.2. Chạy tay lại tách / gộp với $m$ chẵn và trường hợp E9

**Quy ước chạy tay với** $m = 4$ **(khớp Mục 2.2):**

* Mỗi nút có tối đa $m - 1 = 3$ khóa, tối thiểu $\lceil 4/2 \rceil - 1 = 1$ khóa (trừ gốc).

* Nút tràn khi có đủ $m = 4$ **khóa** (đánh chỉ số $0, 1, 2, 3$).

* Khi tách nút 4 khóa $[k_0\ k_1\ k_2\ k_3]$:

  * Nút trái giữ $\lfloor 4/2 \rfloor = 2$ khóa đầu: $[k_0\ k_1]$.

  * Khóa ở chỉ số $\lfloor 4/2 \rfloor = 2$ là $k_2$ (khóa thứ 3) được đẩy lên cha.

  * Nút phải mới giữ $4 - 2 - 1 = 1$ khóa cuối: $[k_3]$.

#### Câu 1: $m = 4$, chèn tăng dần `10, 20, 30, 40, 50, 60, 70` vào cây rỗng

1. **Chèn `10, 20, 30`:** Nút gốc chưa tràn (3 khóa):

   ```
   Tầng 0: [10 20 30]
   
   ```

2. **Chèn `40`:** Gốc tạm thời có 4 khóa `[10 20 30 40]` $\to$ **Tràn (Tách lần 1)**:

   * Nút trái giữ 2 khóa `[10 20]`, khóa chỉ số 2 là `30` đẩy lên tạo gốc mới `[30]`, nút phải giữ 1 khóa `[40]`:

   ```
   Tầng 0:       [30]
                /    \
   Tầng 1: [10 20]  [40]
   
   ```

3. **Chèn `50, 60`:** Đều lớn hơn `30` nên đi xuống lá phải `[40]` $\to$ `[40 50 60]` (3 khóa, chưa tràn):

   ```
   Tầng 0:       [30]
                /    \
   Tầng 1: [10 20]  [40 50 60]
   
   ```

4. **Chèn `70`:** Lá phải tạm thời có 4 khóa `[40 50 60 70]` $\to$ **Tràn (Tách lần 2)**:

   * Nút trái giữ 2 khóa `[40 50]`, khóa chỉ số 2 là `60` đẩy lên gốc `[30 60]`, nút phải mới giữ 1 khóa `[70]`:

   ```
   Tầng 0:          [30 60]
                  /    |    \
   Tầng 1: [10 20]  [40 50]  [70]
   
   ```

* **Nhận xét (So sánh với** $m = 5$**):** Với $m = 4$ (chẵn), mỗi lần tách nút tràn 4 khóa thì **nút trái giữ 2 khóa, nút phải chỉ giữ 1 khóa** (bất đối xứng 2 – 1). Trong khi đó với $m = 5$ (lẻ), nút tràn 5 khóa khi tách sẽ đẩy khóa thứ 3 lên cha, **nút trái giữ 2 khóa và nút phải cũng giữ 2 khóa** (đối xứng 2 – 2).

#### Câu 2: $m = 4$, chèn giảm dần `70, 60, 50, 40, 30, 20, 10` vào cây rỗng

1. **Chèn `70, 60, 50`:** Nút gốc có 3 khóa:

   ```
   Tầng 0: [50 60 70]
   
   ```

2. **Chèn `40`:** Gốc tạm thời có 4 khóa `[40 50 60 70]` $\to$ **Tràn (Tách lần 1)**:

   * Nút trái giữ 2 khóa `[40 50]`, khóa chỉ số 2 là `60` đẩy lên gốc mới `[60]`, nút phải giữ 1 khóa `[70]`:

   ```
   Tầng 0:       [60]
                /    \
   Tầng 1: [40 50]   [70]
   
   ```

3. **Chèn `30`:** Nhỏ hơn `60` nên vào lá trái $\to$ `[30 40 50]` (3 khóa).

4. **Chèn `20`:** Lá trái tạm thời có 4 khóa `[20 30 40 50]` $\to$ **Tràn (Tách lần 2)**:

   * Nút trái giữ 2 khóa `[20 30]`, khóa chỉ số 2 là `40` đẩy lên gốc `[40 60]`, nút phải giữ 1 khóa `[50]`:

   ```
   Tầng 0:          [40 60]
                  /    |    \
   Tầng 1: [20 30]   [50]   [70]
   
   ```

5. **Chèn `10`:** Nhỏ hơn `40` nên vào lá ngoài cùng bên trái $\to$ `[10 20 30]` (3 khóa):

   ```
   Tầng 0:            [40 60]
                    /    |    \
   Tầng 1: [10 20 30]  [50]   [70]
   
   ```

* **So sánh với Câu 1 và liên hệ Mục 8.2:**

  * Ở Câu 1 (chèn tăng dần), khóa mới luôn đi vào nhánh phải cùng, nên các nút bị bỏ lại phía bên trái (`[10 20]`, `[40 50]`) đều giữ **2 khóa** (độ lấp đầy $2/3 \approx 66,7\%$).

  * Ở Câu 2 (chèn giảm dần), khóa mới luôn đi vào nhánh trái cùng, nên các nút bị bỏ lại phía bên phải (`[50]`, `[70]`) chỉ giữ **1 khóa** (độ lấp đầy $1/3 \approx 33,3\%$). Nếu chèn tiếp thêm 2 khóa giảm dần nữa (ví dụ `5, 1`), lá trái `[10 20 30]` sẽ tách đẩy khóa lên làm gốc `[40 60]` tiếp tục tách khi thêm khóa nữa và đẩy chiều cao lên $h = 2$ sớm hơn hẳn so với chèn tăng dần. Đây chính là nguyên nhân ở Mục 8.2 khi $m$ chẵn ($m = 16$), dữ liệu `desc` có độ lấp đầy thấp hơn ($7/15$ so với $8/15$) và làm cây cao hơn dữ liệu `asc`.

#### Câu 3: $m = 4$, từ cây cuối Câu 1 xóa lần lượt `70, 60, 10`

Cây xuất phát từ cuối Câu 1 (ngưỡng tối thiểu mỗi nút khác gốc là $\lceil 4/2 \rceil - 1 = 1$ khóa):

```
Tầng 0:          [30 60]
               /    |    \
Tầng 1: [10 20]  [40 50]  [70]

```

1. **Xóa `70` (`-70`):**

   * `70` nằm ở lá phải cùng `[70]`. Xóa xong lá này còn $0$ khóa ($< 1 \to$ thiếu khóa).

   * `fixChild` kiểm tra anh em liền trái là `[40 50]`: đang có $2 > 1$ khóa $\to$ thực hiện **Mượn anh em trái** (`borrow`).

   * Khóa phân cách `60` ở cha `[30 60]` hạ xuống lá phải; khóa lớn nhất của anh em trái là `50` đẩy lên cha thay vị trí của `60`:

   ```
   Tầng 0:          [30 50]
                  /    |    \
   Tầng 1: [10 20]   [40]   [60]
   
   ```

2. **Xóa `60` (`-60`):**

   * `60` nằm ở lá phải cùng `[60]`. Xóa xong lá này còn $0$ khóa ($< 1 \to$ thiếu khóa).

   * `fixChild` kiểm tra anh em liền trái `[40]`: chỉ có $1$ khóa (đúng bằng mức tối thiểu nên không mượn được), và không có anh em phải $\to$ thực hiện **Gộp với anh em trái** (`merge`).

   * Kéo khóa phân cách `50` từ cha xuống gộp chung với `[40]` và nút rỗng thành `[40 50]`:

   ```
   Tầng 0:       [30]
                /    \
   Tầng 1: [10 20]  [40 50]
   
   ```

3. **Xóa `10` (`-10`):**

   * `10` nằm ở lá trái `[10 20]`. Xóa xong lá trái còn `[20]` ($1$ khóa $\ge 1$, đủ số khóa tối thiểu nên **không cần mượn hay gộp**):

   ```
   Tầng 0:     [30]
              /    \
   Tầng 1: [20]   [40 50]
   
   ```

#### Câu 4: Trường hợp đối kháng E9 ($m = 5$, cây ban đầu `[1 2 3 4]`)

Với $m = 5$: số khóa tối đa ở mỗi nút là $4$, ngưỡng tràn là $5$ khóa, số khóa tối thiểu ở nút khác gốc là $\lceil 5/2 \rceil - 1 = 2$ khóa.

1. **Trạng thái ban đầu (**$S_0$**):**

   ```
   Tầng 0: [1 2 3 4]   (h = 0)
   
   ```

2. **Thực hiện “chèn 5” (`+5`):**

   * Chèn `5` vào gốc $\to$ `[1 2 3 4 5]` (đủ 5 khóa $\to$ **Tràn, tách gốc**).

   * Nút trái giữ $\lfloor 5/2 \rfloor = 2$ khóa `[1 2]`, khóa chỉ số 2 là `3` đẩy lên làm gốc mới `[3]`, nút phải giữ 2 khóa `[4 5]`. Bộ đếm `splits` tăng 1:

   ```
   Tầng 0:      [3]
               /   \
   Tầng 1: [1 2]   [4 5]   (h = 1, trạng thái S_1)
   
   ```

3. **Thực hiện “xóa 5” (`-5`):**

   * Xóa `5` ở lá phải `[4 5]` $\to$ lá phải chỉ còn `[4]` ($1$ khóa $< 2 \to$ thiếu khóa).

   * Kiểm tra anh em trái `[1 2]`: chỉ có đúng $2$ khóa (bằng mức tối thiểu, không mượn được) $\to$ **Gộp với anh em trái**.

   * Kéo khóa `3` từ gốc xuống gộp với `[1 2]` và `[4]` thành `[1 2 3 4]`. Gốc cũ rỗng khóa nên nút gộp `[1 2 3 4]` trở thành gốc mới. Bộ đếm `merges` tăng 1:

   ```
   Tầng 0: [1 2 3 4]   (h = 0, quay về đúng trạng thái S_0)
   
   ```

* **Vì sao lặp 100.000 lần tạo ra đúng 100.000 tách và 100.000 gộp?**
  Vì thao tác `+5` chuyển cây từ $S_0 \to S_1$ kèm đúng 1 lần tách, và thao tác `-5` chuyển cây từ $S_1 \to S_0$ kèm đúng 1 lần gộp. Sau mỗi cặp $\{\text{chèn } 5; \text{xóa } 5\}$, cây quay về trạng thái xuất phát `[1 2 3 4]` mà không có bất kỳ độ trễ (hysteresis) nào giữa ngưỡng tách và ngưỡng gộp.

* **Đề xuất cách sửa và chạy tay lại:**

  * *Cách sửa (thêm độ trễ khi gộp – Underflow Slack):* Chỉ thực hiện gộp khi nút lá tụt xuống còn $\le \lceil m/2 \rceil - 2$ khóa (với $m = 5$ là khi nút còn $\le 1$ khóa thì vẫn giữ nguyên nếu anh em không dư khóa để mượn, chỉ gộp khi nút còn $0$ khóa).

  * *Chạy tay lại với cách sửa:*

    * Ban đầu: `[1 2 3 4]`.

    * Lần 1 `+5`: Tách thành gốc `[3]`, hai lá `[1 2]` và `[4 5]` (**1 lần tách**).

    * Lần 1 `-5`: Lá phải còn `[4]` (1 khóa). Anh em trái `[1 2]` không dư khóa, nhưng theo quy tắc mới chưa gộp ngay nên cây giữ nguyên: gốc `[3]`, lá `[1 2]` và `[4]` (**0 lần gộp**).

    * Lần 2 `+5`: Đi xuống lá phải `[4]` chèn `5` thành `[4 5]` (2 khóa, không tràn $\to$ **0 lần tách**).

    * Lần 2 `-5`: Lá phải về lại `[4]` (**0 lần gộp**).

    * Kết quả sau 100.000 vòng lặp: chỉ tốn **1 lần tách duy nhất ở vòng đầu tiên và 0 lần gộp** (thay vì 100.000 tách và 100.000 gộp).

#### Đối chiếu với công cụ `trace`

Em đã chạy hai lệnh kiểm tra:

* `build/trace 4 +10 +20 +30 +40 +50 +60 +70 -70 -60 -10`

* `build/trace 5 +1 +2 +3 +4 +5 -5 +5 -5`

Kết quả in từng tầng từ `trace` hoàn toàn trùng khớp 100% với bản chạy tay ở cả 4 câu trên (từ vị trí khóa đẩy lên `30`, `60` ở Câu 1, bước mượn `50` khi xóa `70`, bước gộp thành `[40 50]` khi xóa `60` ở Câu 3, cho tới chu trình dao động `[1 2 3 4]` $\leftrightarrow$ `[3] / [1 2] [4 5]` ở Câu 4).

## 13. Chạy lại toàn bộ

Xem [README](../../README.md#chạy-lại-toàn-bộ). Tóm tắt: `sh run_all.sh` (Linux / macOS / Git Bash) hoặc `run_all.bat` (Windows, cần g++ MinGW-w64). Thêm `--quick` để chạy nhanh khoảng 4 phút; kết quả nhanh ghi vào `results/quick/`, không đè lên số liệu chính thức.

## 14. Tài liệu tham khảo

1. T. H. Cormen, C. E. Leiserson, R. L. Rivest, C. Stein, *Introduction to Algorithms*, 3rd ed., MIT Press, 2009, Chapter 18: B-Trees.

2. D. E. Knuth, *The Art of Computer Programming, Vol. 3: Sorting and Searching*, 2nd ed., Addison-Wesley, 1998, §6.2.4.

3. R. Bayer, E. McCreight, “Organization and Maintenance of Large Ordered Indexes,” *Acta Informatica*, vol. 1, no. 3, pp. 173–189, 1972.

4. A. C.-C. Yao, “On Random 2–3 Trees,” *Acta Informatica*, vol. 9, pp. 159–170, 1978.

5. GeeksforGeeks, “Introduction of B-Tree,” https://www.geeksforgeeks.org/dsa/introduction-of-b-tree-2/ (truy cập ngày 5/10/2026).
