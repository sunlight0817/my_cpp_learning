#include <iostream>
#include <stdexcept>

template <typename T>
class DoublyLinkedList {
private:
    // 1. 定义节点结构
    struct Node {
        T data;
        Node* prev;
        Node* next;
        
        // 构造函数
        Node(T val = T()) : data(val), prev(nullptr), next(nullptr) {}
    };

    Node* dummyHead; // 头哨兵
    Node* dummyTail; // 尾哨兵
    size_t size_;    // 节点数量

    // ==========================================
    // 🌟 核心基石操作 (所有增删都基于这两个函数)
    // ==========================================

    // 在指定节点 pos 之前插入新节点 O(1)
    void insertBefore(Node* pos, T val) {
        Node* newNode = new Node(val);
        Node* prevNode = pos->prev;

        // 绑定 newNode 的前后指针
        newNode->prev = prevNode;
        newNode->next = pos;

        // 修改原链表的前后指针
        prevNode->next = newNode;
        pos->prev = newNode;

        size_++;
    }

    // 删除指定节点 pos O(1)
    void eraseNode(Node* pos) {
        if (pos == dummyHead || pos == dummyTail) {
            throw std::invalid_argument("Cannot erase dummy nodes");
        }

        Node* prevNode = pos->prev;
        Node* nextNode = pos->next;

        // 让前后节点互相牵手，架空 pos
        prevNode->next = nextNode;
        nextNode->prev = prevNode;

        delete pos;
        size_--;
    }

public:
    // 2. 构造与析构
    DoublyLinkedList() : size_(0) {
        dummyHead = new Node();
        dummyTail = new Node();
        
        // 初始化时，头尾哨兵互相连接，中间为空
        dummyHead->next = dummyTail;
        dummyTail->prev = dummyHead;
    }

    ~DoublyLinkedList() {
        clear();
        delete dummyHead;
        delete dummyTail;
    }

    // 禁用拷贝以保持简单（可按需实现深拷贝）
    DoublyLinkedList(const DoublyLinkedList&) = delete;
    DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

    // 3. 状态查询
    bool isEmpty() const { return size_ == 0; }
    size_t getSize() const { return size_; }

    // 4. 对外 API：增删操作 (全部复用核心基石函数，极其清爽)
    
    // 头插：插在头哨兵的后面
    void push_front(T val) { insertBefore(dummyHead->next, val); }
    
    // 尾插：插在尾哨兵的前面
    void push_back(T val) { insertBefore(dummyTail, val); }
    
    // 头删
    void pop_front() {
        if (!isEmpty()) eraseNode(dummyHead->next);
    }
    
    // 尾删
    void pop_back() {
        if (!isEmpty()) eraseNode(dummyTail->prev);
    }

    // 清空链表
    void clear() {
        while (!isEmpty()) {
            pop_front();
        }
    }

    // 5. 打印测试
    void printForward() const {
        Node* curr = dummyHead->next;
        std::cout << "正向: Head <-> ";
        while (curr != dummyTail) {
            std::cout << curr->data << " <-> ";
            curr = curr->next;
        }
        std::cout << "Tail\n";
    }

    void printBackward() const {
        Node* curr = dummyTail->prev;
        std::cout << "反向: Tail <-> ";
        while (curr != dummyHead) {
            std::cout << curr->data << " <-> ";
            curr = curr->prev;
        }
        std::cout << "Head\n";
    }
};

// ================= 测试代码 =================
int main() {
    DoublyLinkedList<int> list;

    list.push_back(10);
    list.push_back(20);
    list.push_front(5);
    
    std::cout << "--- 插入后 ---" << std::endl;
    list.printForward();  // 期望: Head <-> 5 <-> 10 <-> 20 <-> Tail
    list.printBackward(); // 期望: Tail <-> 20 <-> 10 <-> 5 <-> Head

    list.pop_back();
    list.pop_front();
    
    std::cout << "\n--- 删除后 ---" << std::endl;
    list.printForward();  // 期望: Head <-> 10 <-> Tail

    return 0;
}