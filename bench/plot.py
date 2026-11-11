# plot.py - ve hinh cho bao cao Tuan 3 tu cac file CSV trong results/.
# Chay (sau ./bench_btree all):  python3 bench/plot.py           -> docs/week3/images/*.png
#      (sau ./bench_btree all --quick): python3 bench/plot.py --quick -> results/quick/images/*.png
import math
import os
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import pandas as pd

QUICK = "--quick" in sys.argv
RES = "results/quick" if QUICK else "results"
OUT = "results/quick/images" if QUICK else "docs/week3/images"
os.makedirs(OUT, exist_ok=True)
plt.rcParams.update({"figure.dpi": 130, "font.size": 9, "axes.grid": True, "grid.alpha": 0.3})

H = pd.read_csv(f"{RES}/height_reads.csv")
B = pd.read_csv(f"{RES}/btree_vs_bst.csv")
O = pd.read_csv(f"{RES}/opt_linear_vs_binary.csv")
S = pd.read_csv(f"{RES}/m_sweep.csv")
ORD = ["random", "asc", "desc", "zigzag"]

# ---- Hinh 1: chieu cao thuc te vs can ly thuyet theo n, cho tung m
fig, axes = plt.subplots(1, 5, figsize=(15, 3.2), sharey=False)
for ax, m in zip(axes, [3, 5, 16, 64, 256]):
    d = H[H.m == m]
    rnd, asc = d[d.order == "random"], d[d.order == "asc"]
    ax.fill_between(rnd.n, rnd.h_min, rnd.h_max, alpha=0.15, label="vùng lý thuyết [h_min, h_max]")
    ax.plot(rnd.n, rnd.h_obs_hi, "o-", label="ngẫu nhiên")
    ax.plot(asc.n, asc.h, "s--", label="tăng dần")
    ax.set_xscale("log"); ax.set_title(f"m = {m}"); ax.set_xlabel("n (số khóa)")
axes[0].set_ylabel("chiều cao h"); axes[0].legend(fontsize=7)
fig.suptitle("Hình 1. Chiều cao đo được nằm trong cận lý thuyết; dữ liệu tăng dần chạm cận trên")
fig.tight_layout(); fig.savefig(f"{OUT}/fig1_height_vs_bounds.png"); plt.close(fig)

# ---- Hinh 2: so lan doc dia moi lan tim (n lon nhat da do), theo m va kieu du lieu
NMAX = int(H.n.max())
d = H[H.n == NMAX]
fig, ax = plt.subplots(figsize=(6.5, 3.4))
w = 0.2
ms = [3, 5, 16, 64, 256]
for i, o in enumerate(ORD):
    vals = [d[(d.m == m) & (d.order == o)].reads_hit.iloc[0] for m in ms]
    ax.bar([j + (i - 1.5) * w for j in range(len(ms))], vals, w, label=o)
ax.plot(range(len(ms)), [math.log(NMAX + 1, m) for m in ms], "k^:", label="log_m(n+1)")
ax.set_xticks(range(len(ms))); ax.set_xticklabels([f"m={m}" for m in ms])
ax.set_ylabel("số nút thăm / lần tìm (= số lần đọc đĩa)")
ax.set_title(f"Hình 2. Số lần đọc đĩa khi tìm, n = {NMAX:,}".replace(",", ".")); ax.legend(fontsize=7)
fig.tight_layout(); fig.savefig(f"{OUT}/fig2_reads_per_search.png"); plt.close(fig)

# ---- Hinh 3: anh huong cua m (quet m, n = 10^6 ngau nhien)
fig, axes = plt.subplots(1, 3, figsize=(13, 3.4))
axes[0].plot(S.m, S.h, "o-", label="h đo được")
axes[0].plot(S.m, S.h_min, ":", label="h_min"); axes[0].plot(S.m, S.h_max, ":", label="h_max")
axes[0].set_title("Chiều cao"); axes[0].legend(fontsize=7)
axes[1].plot(S.m, S.heap_MB, "o-", label="RAM thực (vector + overhead)")
axes[1].plot(S.m, S.disk_MB, "s-", label="đĩa (trang cố định 12m byte)")
axes[1].set_title("Bộ nhớ (MB)"); axes[1].legend(fontsize=7)
ax2 = axes[2]
ax2.plot(S.m, S.reads_hit, "o-", color="C0", label="số trang đọc / lần tìm")
ax2.set_ylabel("số trang", color="C0")
ax3 = ax2.twinx(); ax3.grid(False)
ax3.plot(S.m, S.bytes_per_search / 1024, "s--", color="C3", label="KB đọc / lần tìm")
ax3.set_ylabel("KB", color="C3")
ax2.axvline(341, color="gray", ls=":"); ax2.text(341, ax2.get_ylim()[1] * 0.9, " trang 4 KB\n m = 341", fontsize=7)
ax2.set_title("Đọc đĩa")
for a in axes: a.set_xscale("log", base=2); a.set_xlabel("m")
fig.suptitle("Hình 3. Ảnh hưởng của m (n = 1.000.000 khóa ngẫu nhiên)")
fig.tight_layout(); fig.savefig(f"{OUT}/fig3_effect_of_m.png"); plt.close(fig)

# ---- Hinh 4: B-Tree vs BST
fig, axes = plt.subplots(1, 2, figsize=(11, 3.4))
d = B[(B.n == 100000) & (B.structure != "std_set")]
structs = ["btree_m5", "btree_m64", "bst"]
for i, s in enumerate(structs):
    vals = [d[(d.structure == s) & (d.order == o)].reads_hit.iloc[0] for o in ORD]
    axes[0].bar([j + (i - 1) * 0.27 for j in range(4)], vals, 0.27, label=s)
axes[0].set_yscale("log"); axes[0].set_xticks(range(4)); axes[0].set_xticklabels(ORD)
axes[0].set_ylabel("nút thăm / lần tìm (log)"); axes[0].set_title("Số lần đọc đĩa khi tìm, n = 100.000")
axes[0].legend(fontsize=7)
d = B[B.n == 100000]
for i, s in enumerate(structs + ["std_set"]):
    vals = [d[(d.structure == s) & (d.order == o)].ms_insert.iloc[0] for o in ORD]
    axes[1].bar([j + (i - 1.5) * 0.2 for j in range(4)], vals, 0.2, label=s)
axes[1].set_yscale("log"); axes[1].set_xticks(range(4)); axes[1].set_xticklabels(ORD)
axes[1].set_ylabel("ms để chèn 100.000 khóa (log)"); axes[1].set_title("Thời gian chèn (trong RAM)")
axes[1].legend(fontsize=7)
fig.suptitle("Hình 4. B-Tree và BST không cân bằng trên cùng dữ liệu, cùng thao tác")
fig.tight_layout(); fig.savefig(f"{OUT}/fig4_btree_vs_bst.png"); plt.close(fig)

# ---- Hinh 5: cai tien - duyet tuan tu vs tim nhi phan trong nut
b, l = O[O["mode"] == "binary"].set_index("m"), O[O["mode"] == "linear"].set_index("m")
fig, ax = plt.subplots(figsize=(6.5, 3.4))
ax.plot(b.index, b.ns_search, "o-", label="tìm nhị phân (lower_bound)")
ax.plot(l.index, l.ns_search, "s-", label="duyệt tuần tự")
ax.set_xscale("log", base=2); ax.set_yscale("log"); ax.set_xlabel("m"); ax.set_ylabel("ns / lần tìm")
for m in b.index:
    r = l.ns_search[m] / b.ns_search[m]
    ax.annotate(f"{r:.2f}", (m, l.ns_search[m]), textcoords="offset points", xytext=(0, 6), fontsize=6, ha="center")
ax.set_title("Hình 5. Thử cải tiến: thời gian tìm (n = 10⁶), số trên điểm = tuần tự / nhị phân")
ax.legend(fontsize=7)
fig.tight_layout(); fig.savefig(f"{OUT}/fig5_opt_linear_vs_binary.png"); plt.close(fig)

print("Da ve 5 hinh vao", OUT)
