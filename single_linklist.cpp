#include <iostream>
#include <memory>
#include <utility>

template <typename T>
class List {
private:
    struct Node {
        T val; // data 改为更简短的 val
        std::unique_ptr<Node> next;
        
        Node(T v) : val(std::move(v)), next(nullptr) {}
    };

    std::unique_ptr<Node> head;
    size_t size = 0; // 直接在这里初始化，省去构造函数里的赋值

public:
    // 1. 构造与析构
    List() = default; // 极其清爽的默认构造
    ~List() { clear(); }

    // 2. 移动语义 (直接用 default 搞定)
    List(List&&) noexcept = default;
    List& operator=(List&&) noexcept = default;

    // 3. 深拷贝构造
    List(const List& other) {
        if (!other.head) return;
        
        head = std::make_unique<Node>(other.head->val);
        size++;
        // tail: 新链表的尾巴，负责往后接新节点
        // src:  源链表 (source) 的节点，负责被读取
        Node* tail = head.get();          
        Node* src = other.head->next.get(); 

        while (src) {
            tail->next = std::make_unique<Node>(src->val);
            tail = tail->next.get();
            src = src->next.get();
            size++;
        }
    }

    // 4. 拷贝赋值
    List& operator=(const List& other) {
        if (this != &other) {
            List clone(other); // temp 改叫 clone，意图更明显
            head = std::move(clone.head);
            size = clone.size;
        }
        return *this;
    }

    // --- 极简迭代器 ---
    class Iterator {
    private:
        Node* node; // current 改为 node
    public:
        explicit Iterator(Node* p) : node(p) {}
        T& operator*() const { return node->val; }
        T* operator->() const { return &(node->val); }

        Iterator& operator++() {
            if (node) node = node->next.get();
            return *this;
        }
        
        bool operator!=(const Iterator& other) const { 
            return node != other.node; 
        }
    };

    Iterator begin() { return Iterator(head.get()); }
    Iterator end() { return Iterator(nullptr); }

    // --- 核心操作 ---
    void push_front(T v) {
        auto node = std::make_unique<Node>(std::move(v)); // newNode 改为 node
        node->next = std::move(head);
        head = std::move(node);
        size++;
    }

    void clear() {
        while (head) {
            head = std::move(head->next);
        }
        size = 0;
    }
    
    void print() const {
        Node* curr = head.get(); // current 改为通用的简写 curr
        while (curr) {
            std::cout << curr->val << " -> ";
            curr = curr->next.get();
        }
        std::cout << "null\n"; // nullptr 打印时简写为 null
    }
};

// ================= 测试代码 =================
int main() {
    List<int> listA; // ModernLinkedList 简化为 List
    listA.push_front(3);
    listA.push_front(2);
    listA.push_front(1);
    
    std::cout << "listA: ";
    listA.print(); 

    // 测试拷贝
    List<int> listB = listA; 
    std::cout << "listB: ";
    listB.print(); 

    // 测试范围 for 循环
    std::cout << "迭代打印: ";
    for (int val : listB) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    return 0;
}