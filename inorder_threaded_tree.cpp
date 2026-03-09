// 编译命令: g++ -std=c++14 inorder_threaded_tree.cpp -o threaded_tree && ./threaded_tree

#include <iostream>
#include <memory>
#include <utility>
#include <vector>

namespace data_structures {

// 1. 节点定义（纯数据结构）
template <typename T>
struct TreeNode {
  T value;
  // 现代 C++ 处理复杂图/网状结构的最佳实践：裸指针导航，不管理生命周期
  TreeNode* left;
  TreeNode* right;
  
  // 标志位：为 true 表示该指针是线索，为 false 表示指向真正的子节点
  bool is_left_thread;
  bool is_right_thread;

  explicit TreeNode(T val)
      : value(std::move(val)),
        left(nullptr),
        right(nullptr),
        is_left_thread(false),
        is_right_thread(false) {}
};

// 2. 线索二叉树类
template <typename T>
class ThreadedBinaryTree {
 public:
  ThreadedBinaryTree() : root_(nullptr) {}

  // 禁用拷贝，防止双重释放或浅拷贝导致的指针错乱
  ThreadedBinaryTree(const ThreadedBinaryTree&) = delete;
  ThreadedBinaryTree& operator=(const ThreadedBinaryTree&) = delete;
  // 移动构造函数
  ThreadedBinaryTree(ThreadedBinaryTree&&) = default;
  // 移动赋值操作符
  ThreadedBinaryTree& operator=(ThreadedBinaryTree&&) = default;

  // 1. 插入节点：构建一棵普通的二叉搜索树 (BST)
  void Insert(T value) {
    // 将新节点存入内存池 (Arena)
    arena_.push_back(std::make_unique<TreeNode<T>>(std::move(value)));
    TreeNode<T>* new_node = arena_.back().get();

    if (!root_) {
      root_ = new_node;
      return;
    }

    TreeNode<T>* curr = root_;
    while (true) {
      if (new_node->value < curr->value) {
        if (!curr->left) {
          curr->left = new_node;
          break;
        }
        curr = curr->left;
      } else {
        if (!curr->right) {
          curr->right = new_node;
          break;
        }
        curr = curr->right;
      }
    }
  }

  // 2. 核心：将普通二叉树“中序线索化”
  void CreateInorderThreads() {
    TreeNode<T>* prev = nullptr;
    CreateThreadsHelper(root_, prev);
  }

  // 3. 极速遍历：利用线索进行中序遍历 (空间复杂度 O(1)，无递归，无栈)
  void PrintThreadedInorder() const {
    if (!root_) return;

    std::cout << "线索化中序遍历结果: ";

    // 步骤 1: 找到中序遍历的起点（整棵树最左下的节点）
    TreeNode<T>* curr = root_;
    while (curr->left != nullptr && !curr->is_left_thread) {
      curr = curr->left;
    }

    // 步骤 2: 顺着线索或右孩子一路往后走
    while (curr != nullptr) {
      std::cout << curr->value << " ";

      // 如果 right 是一根线索，直接顺着它“飞”到下一个节点
      if (curr->is_right_thread) {
        curr = curr->right;
      } 
      // 如果 right 是真正的右孩子，那么下一个节点是右子树的最左节点
      else {
        curr = curr->right;
        while (curr != nullptr && !curr->is_left_thread) {
          curr = curr->left;
        }
      }
    }
    std::cout << "\n";
  }

 private:
  // 内存池：统一管理所有节点的生命周期。类销毁时，vector 清空，所有节点安全释放。
  std::vector<std::unique_ptr<TreeNode<T>>> arena_;
  TreeNode<T>* root_;

  // 私有辅助：递归实现线索化
  // 注意 `prev` 是指针的引用，这样在递归的各个层级中修改的都是同一个 prev
  void CreateThreadsHelper(TreeNode<T>* curr, TreeNode<T>*& prev) {
    if (curr == nullptr) {
      return;
    }

    // 1. 递归处理左子树
    CreateThreadsHelper(curr->left, prev);

    // 2. 处理当前节点的前驱线索
    if (curr->left == nullptr) {
      curr->is_left_thread = true;
      curr->left = prev; 
    }

    // 3. 处理前一个节点的后继线索
    if (prev != nullptr && prev->right == nullptr) {
      prev->is_right_thread = true;
      prev->right = curr; 
    }

    // 更新 prev 为当前节点，继续往后走
    prev = curr;

    // 4. 递归处理右子树
    CreateThreadsHelper(curr->right, prev);
  }
};

}  // namespace data_structures


// 3. Main 函数测试
int main() {
  data_structures::ThreadedBinaryTree<int> tree;

  // 插入数据，构建如下 BST：
  //         5
  //       /   \
  //      3     7
  //     / \   / \
  //    1   4 6   9
  std::cout << "正在插入节点构建二叉树...\n";
  tree.Insert(5);
  tree.Insert(3);
  tree.Insert(7);
  tree.Insert(1);
  tree.Insert(4);
  tree.Insert(6);
  tree.Insert(9);

  std::cout << "正在进行中序线索化...\n";
  tree.CreateInorderThreads();

  // 预期输出: 1 3 4 5 6 7 9
  // 这里的遍历全程没有调用递归，也没有申请额外的 stack 空间！
  tree.PrintThreadedInorder();

  std::cout << "\n程序结束，内存池(Arena)将自动清理所有存在复杂环状引用的裸指针！\n";

  return 0;
}