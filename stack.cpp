#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>
#include <string>
using namespace std;
template <typename T>
class astack {
private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    unique_ptr<T[]> data_;

    void reallocate(size_t new_capacity) {
        auto new_data = make_unique<T[]>(new_capacity);
        for (size_t i = 0; i < size_; ++i) {
            new_data[i] = std::move(data_[i]);
        }

        data_ = std::move(new_data);
        capacity_ = new_capacity;
    }

public:
//拷贝赋值，拷贝构造，移动构造，移动赋值，析构
    astack() = default;
    ~astack() = default;
    astack(const astack&) = delete;
    astack& operator=(const astack&) = delete;
    astack(astack&&) noexcept = default;
    astack& operator=(astack&&) noexcept = default;

    void push(T val) {
        if (size_ == capacity_) {
            reallocate(capacity_ == 0 ? 1 : capacity_ * 2);
        }
        data_[size_++] = std::move(val);
    }

    void pop() {
        if(empty()) {
            throw out_of_range("Stack underflow: Cannot pop from an empty stack");
        }
        size_--;
    }

    T& top() {
        if(empty()) {
            throw out_of_range("Stack is empty");
        }
        return data_[size_ - 1];
    }

    const T& top() const {
        if(empty()) {
            throw out_of_range("Stack is empty");
        }
        return data_[size_ - 1];
    }

    bool empty() const {
        return size_ == 0;
    }
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }

};
int main() {
    astack<string> stack;

    cout << "--- 测试动态扩容与压栈 ---" << std::endl;
    stack.push("C++");
    stack.push("Modern");
    stack.push("Awesome");
    
    cout << "当前栈顶: " << stack.top() << std::endl; // 输出: Awesome
    cout << "当前大小: " << stack.size() << ", 当前容量: " << stack.capacity() << std::endl;

    cout << "\n--- 测试出栈 ---" << std::endl;
    stack.pop(); // Awesome 被弹出
    cout << "弹出一个元素后，新的栈顶: " << stack.top() << std::endl; // 输出: Modern

    cout << "\n--- 测试清空栈 ---" << std::endl;
    while (!stack.empty()) {
        cout << "正在弹出: " << stack.top() << std::endl;
        stack.pop();
    }

    // 测试异常安全
    try {
        stack.pop();
    } catch (const std::exception& e) {
        cout << "\n成功捕获异常: " << e.what() << std::endl;
    }

    return 0;
}