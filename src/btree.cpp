#include "btree.h"
#include <stdexcept>
#include <algorithm>

BTree::BTree(int order) : m_(order) {
    if (order < 3) throw std::invalid_argument("B-Tree order must be >= 3");
    root_ = nullptr;
}

BTree::~BTree() { 
    destroy(root_); 
}

void BTree::destroy(Node* node) {
    if (!node) return;
    for (Node* child : node->children) {
        destroy(child);
    }
    delete node;
}

// 1. HAM TIM KIEM (SEARCH)
bool BTree::search(int key, std::vector<std::vector<int>>* path) const {
    Node* current = root_;
    while (current != nullptr) {
        // Ghi lai cac khoa cua nut hien tai vao path (mo phong Disk Read / Trace)
        if (path) {
            path->push_back(current->keys);
        }

        // Tim vi tri khoa dau tien lớn hon hoac bang key
        auto it = std::lower_bound(current->keys.begin(), current->keys.end(), key);
        int idx = std::distance(current->keys.begin(), it);

        // Neu tim thay key trong nut hien tai
        if (idx < current->keys.size() && current->keys[idx] == key) {
            return true;
        }

        // Neu la nut la thi khong con duong de di
        if (current->isLeaf()) {
            break;
        }

        // Chuyen xuong nut con tuong ung
        current = current->children[idx];
    }
    return false;
}

// Hàm hỗ trợ tách nút con khi tràn (Node Splitting)
void BTree::splitChild(Node* parent, int i, Node* fullChild) {
    // Tạo nút mới lưu m-1/2 khóa của fullChild
    Node* newNode = new Node(fullChild->isLeaf());
    int medianIdx = (m_ - 1) / 2;

    // Chuyển nửa sau khóa của fullChild sang newNode
    newNode->keys.assign(fullChild->keys.begin() + medianIdx + 1, fullChild->keys.end());

    // Nếu không phải lá, chuyển nửa sau nút con sang newNode
    if (!fullChild->isLeaf()) {
        newNode->children.assign(fullChild->children.begin() + medianIdx + 1, fullChild->children.end());
        fullChild->children.erase(fullChild->children.begin() + medianIdx + 1, fullChild->children.end());
    }

    int medianKey = fullChild->keys[medianIdx];
    fullChild->keys.erase(fullChild->keys.begin() + medianIdx, fullChild->keys.end());

    // Chèn con mới vào parent
    parent->children.insert(parent->children.begin() + i + 1, newNode);
    // Đẩy khóa giữa lên parent
    parent->keys.insert(parent->keys.begin() + i, medianKey);
}

// Hàm hỗ trợ chèn vào nút chưa đầy (Insert Non-Full)
void BTree::insertNonFull(Node* node, int key) {
    int i = node->keys.size() - 1;

    if (node->isLeaf()) {
        // Tìm vị trí thích hợp và chèn key vào lá
        node->keys.push_back(0);
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
    } else {
        // Tìm nút con để xuống
        while (i >= 0 && node->keys[i] > key) {
            i--;
        }
        i++;

        // Nếu nút con bị tràn (chứa m-1 khóa), tách nút trước khi xuống
        if (node->children[i]->keys.size() == m_ - 1) {
            splitChild(node, i, node->children[i]);
            if (node->keys[i] < key) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

// 2. HAM CHEN (INSERT)
bool BTree::insert(int key) {
    // Nếu key đã tồn tại trong cây thì không chèn lại
    if (search(key)) return false;

    if (root_ == nullptr) {
        root_ = new Node(true);
        root_->keys.push_back(key);
        return true;
    }

    // Nếu gốc bị tràn, tạo gốc mới và tách
    if (root_->keys.size() == m_ - 1) {
        Node* newRoot = new Node(false);
        newRoot->children.push_back(root_);
        splitChild(newRoot, 0, root_);

        int i = 0;
        if (newRoot->keys[0] < key) {
            i++;
        }
        insertNonFull(newRoot->children[i], key);
        root_ = newRoot;
    } else {
        insertNonFull(root_, key);
    }
    return true;
}

// 3. HAM XOA (REMOVE - OPTIONAL/DUMMY FOR WEEK 2)
bool BTree::remove(int key) {
    // Xóa khóa trên B-Tree (Optional bonus).
    // Nếu chưa cài đặt chi tiết có thể để trả về false nếu không tìm thấy key
    if (!search(key)) return false;
    
    // TODO: Triển khai Merge/Redistribution nếu làm phần thưởng điểm cộng (Bonus)
    return false; 
}

// Hàm hỗ trợ duyệt In-order
void BTree::inOrderHelper(Node* node, std::vector<int>& result) const {
    if (!node) return;
    for (size_t i = 0; i < node->keys.size(); ++i) {
        if (!node->isLeaf()) {
            inOrderHelper(node->children[i], result);
        }
        result.push_back(node->keys[i]);
    }
    if (!node->isLeaf()) {
        inOrderHelper(node->children.back(), result);
    }
}

// 4. HAM DUYET CAY IN-ORDER (TRAVERSE)
std::vector<int> BTree::traverse() const {
    std::vector<int> result;
    inOrderHelper(root_, result);
    return result;
}

// 5. HAM KIEM TRA TINH CHAT CAY (VALIDATE)
bool BTree::validate(std::string* error) const {
    if (!root_) return true; // Cây rỗng coi như hợp lệ

    // 1. Kiểm tra số lượng khóa của gốc
    if (root_->keys.size() >= static_cast<size_t>(m_)) {
        if (error) *error = "Root has too many keys";
        return false;
    }

    // 2. Kiểm tra tính thứ tự tăng dần của các khóa thu được qua duyệt In-order
    std::vector<int> keys = traverse();
    for (size_t i = 1; i < keys.size(); ++i) {
        if (keys[i] <= keys[i - 1]) {
            if (error) *error = "Keys are not strictly sorted or contain duplicates";
            return false;
        }
    }
    return true;
}

// 6. HAM TINH CHIEU CAO CAY (HEIGHT)
int BTree::height() const {
    if (!root_) return -1;
    int h = 0;
    for (const Node* n = root_; !n->isLeaf(); n = n->children[0]) {
        ++h;
    }
    return h;
}