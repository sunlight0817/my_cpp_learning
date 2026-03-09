#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
using namespace std;
template <typename T>
class aqueue {
private:
    unique_ptr<T[]> data_;
    size_t size_ = 0;
    size_t head_ = 0;
    size_t tail_ = 0;
    size_t capacity_ = 0;
    void reallocate(size_t new_capacity) {
        auto new_data = make_unique<T[]>(new_capacity);

        for (size_t i = 0; i < size_; ++i) {
            size_t old_index = (head_ + i) % capacity_;
            new_data[i] = std::move(data_[old_index]); 
        }
        data_ = std::move(new_data);
        head_ = 0;
        tail_ = size_;
        capacity_ = new_capacity;
    }

public:
    aqueue() = default;
    ~aqueue() = default;
    aqueue(const aqueue&) = delete;
    aqueue& operator=(const aqueue&) = delete;
    aqueue(aqueue&&) noexcept = default;
    aqueue& operator=(aqueue&&) noexcept = default;
    
    bool empty(){ return size_ == 0; }
    size_t size(){ return size_; }

    void push(T val) {
        if (size_ == capacity_) {
            reallocate(capacity_ == 0 ? 4 : capacity_ * 2);
        }
        data_[tail_] = std::move(val);
        tail_ = (tail_ + 1) % capacity_;
        ++size_;
    }

    void pop() {
        if(empty()){
            throw out_of_range("the queue is empty");
        }
        // 如果 T 是 shared_ptr 或大对象，光把 head 往前推是不够的。
        // 原来的位置虽然“逻辑上”出队了，但物理上还残留着对象，导致内存无法及时释放。
        // 所以我们用一个空的默认对象覆盖它，强行释放底层资源
        data_[head_] = T{};
        head_ = (head_ + 1) % capacity_;
        size_--;
    }

    T& front() {
        if(empty()){
            throw out_of_range("the queue is empty");
        }
        return data_[head_];
    }
    const T& front() const {
        if(empty()){
            throw out_of_range("the queue is empty");
        }
        return data_[head_];
    }
};
int main() {
    aqueue<string> q;

    cout << "--- 测试入队 ---" << endl;
    q.push("第一名");
    q.push("第二名");
    q.push("第三名");
    cout << "当前队头: " << q.front() << ", 大小: " << q.size() << endl;

    cout << "\n--- 测试出队 ---" << endl;
    q.pop(); // "第一名" 被送走
    cout << "出队一次后，新队头: " << q.front() << endl;

    cout << "\n--- 测试环形覆盖 (触发扩容与指针环绕) ---" << endl;
    q.push("第四名");
    q.push("第五名");
    q.push("第六名"); // 此时应该触发容量翻倍，你可以想象底层的搬家过程

    while (!q.empty()) {
        cout << "服务中: " << q.front() << endl;
        q.pop();
    }

    return 0;
}