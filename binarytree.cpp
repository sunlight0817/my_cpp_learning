// 这是一个完整可运行的现代 C++ 二叉树示例
// 编译命令示例: g++ -std=c++14 main.cpp -o main && ./main

#include <iostream>
#include <memory>
#include <queue>
#include <utility>

namespace data_structures {

// 1. 节点定义（纯数据结构）
template <typename T>
struct TreeNode {
  T value;
  std::unique_ptr<TreeNode<T>> left;
  std::unique_ptr<TreeNode<T>> right;

  // explicit 防止隐式转换，std::move 优化拷贝
  explicit TreeNode(T val)
      : value(std::move(val)), left(nullptr), right(nullptr) {}
};

// 2. 二叉树类定义
template <typename T>
class BinaryTree {
 public:
  BinaryTree() = default;

  // 禁用拷贝，因为 unique_ptr 独占内存
  BinaryTree(const BinaryTree&) = delete;
  BinaryTree& operator=(const BinaryTree&) = delete;

  // 允许移动
  BinaryTree(BinaryTree&&) = default;
  BinaryTree& operator=(BinaryTree&&) = default;

  // 层序插入：保证树尽可能是一棵完全二叉树
  void Insert(T value) {
    if (!root_) {
      root_ = std::make_unique<TreeNode<T>>(std::move(value));
      return;
    }

    // 使用裸指针队列来寻找空位，绝不影响 unique_ptr 的所有权
    std::queue<TreeNode<T>*> q;
    q.push(root_.get());

    while (!q.empty()) {
      TreeNode<T>* current = q.front();
      q.pop();

      if (!current->left) {
        current->left = std::make_unique<TreeNode<T>>(std::move(value));
        return;
      } else {
        q.push(current->left.get());
      }

      if (!current->right) {
        current->right = std::make_unique<TreeNode<T>>(std::move(value));
        return;
      } else {
        q.push(current->right.get());
      }
    }
  }

  // 打印前序遍历 (根 -> 左 -> 右)
  void PrintPreorder() const {
    std::cout << "前序遍历: ";
    PreorderHelper(root_.get());
    std::cout << '\n';
  }

  // 打印中序遍历 (左 -> 根 -> 右)
  void PrintInorder() const {
    std::cout << "中序遍历: ";
    InorderHelper(root_.get());
    std::cout << '\n';
  }

 private:
  std::unique_ptr<TreeNode<T>> root_; // 根节点，自动管理整棵树的内存

  // 辅助函数：传递裸指针用于只读遍历
  void PreorderHelper(const TreeNode<T>* node) const {
    if (node != nullptr) {
      std::cout << node->value << " ";
      PreorderHelper(node->left.get());
      PreorderHelper(node->right.get());
    }
  }

  void InorderHelper(const TreeNode<T>* node) const {
    if (node != nullptr) {
      InorderHelper(node->left.get());
      std::cout << node->value << " ";
      InorderHelper(node->right.get());
    }
  }
};

}  // namespace data_structures


// 3. Main 函数测试
int main() {
  // 实例化一个存储 int 类型的二叉树
  data_structures::BinaryTree<int> tree;

  // 依次插入 1 到 7
  // 层序插入后，树的结构将会是这样的：
  //        1
  //      /   \
  //     2     3
  //    / \   / \
  //   4   5 6   7
  std::cout << "正在往二叉树中插入数据: 1, 2, 3, 4, 5, 6, 7...\n";
  for (int i = 1; i <= 7; ++i) {
    tree.Insert(i);
  }

  std::cout << "插入完成！\n\n";

  // 预期输出: 1 2 4 5 3 6 7
  tree.PrintPreorder(); 

  // 预期输出: 4 2 5 1 6 3 7
  tree.PrintInorder();  

  std::cout << "\n程序即将结束，整棵树的内存会被 unique_ptr 自动、安全地释放！\n";

  return 0;
}