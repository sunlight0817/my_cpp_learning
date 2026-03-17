#include <iostream>
#include <utility>
#include <memory>

template <typename T>
class RedBlackTree {
  public:
    RedBlackTree() : root_ (nullptr) {}  

    ~RedBlackTree() { 
      DestroyTree(root_);
    }
    RedBlackTree(const RedBlackTree&) = delete;
    RedBlackTree& operator=(const RedBlackTree&) = delete;
    /*
    RedBlackTree(RedBlackTree&&) noexcept = default; 
    RedBlackTree&& operator=(RedBlackTree&&) noexcept = default;
    不正确，在红黑树内部使用裸指针，裸指针默认浅拷贝，会出现double free 需要手动实现移动函数，并写noexcept
    */
    RedBlackTree(RedBlackTree&& other) noexcept : root_(other.root_) {
      other.root_ = nullptr;
    }
    RedBlackTree& operator=(RedBlackTree&& other) noexcept {
      if (this != &other) {
        DestroyTree(root_);
        root_ = other.root_;
        other.root_ = nullptr;
      }
      return *this;
    }
    std::pair<bool, T> Insert(T key) {
      Node* new_node = new Node(key);
      Node* parent_node = nullptr;
      Node* current = root_;

      while (current != nullptr) {
        parent_node = current;
        if(new_node->data < current->data) {
          current = current->left;
        } else if (new_node->data > current->data) {
          current = current->right;
        } else {
          delete new_node;
          return std::make_pair(false, current->data);
        }
      }

      new_node->parent = parent_node;
      if (parent_node == nullptr) {
        root_ = new_node;
      } else if (new_node->data < parent_node->data) {
        parent_node->left = new_node;
      } else {
        parent_node->right = new_node;
      }
      
      if (new_node->parent == nullptr) {
        new_node->color = Color::kBlack;
        return std::make_pair(true, new_node->data);
      }
      if (new_node->parent->parent == nullptr) {
        return std::make_pair(true, new_node->data);
      }

      InsertFixup(new_node);
      return std::make_pair(true, new_node->data);
    }

    void PrintOrder() const {
      InorderHelper(root_);
      std::cout << std::endl;
    }
  private:
    enum class Color {
      kRed,
      kBlack
    };

    struct Node {
      T data;
      Color color;
      Node* left;
      Node* right;
      Node* parent;

      explicit Node(T val)
          : data(std::move(val)),
            color(Color::kRed),
            left(nullptr),
            right(nullptr),
            parent(nullptr) {}
    };

    Node* root_;
    //左旋操作
    void LeftRotate(Node* x) {
      Node* y = x->right;
      x->right = y->left;
      if (y->left != nullptr) {
        y->left->parent = x;
      }
      y->parent = x->parent;
      if (x->parent == nullptr) {
        root_ = y;
      } else if (x == x->parent->left) {
        x->parent->left = y;
      } else {
        x->parent->right = y;
      }
      y->left = x;
      x->parent = y;
    }
    //右旋操作
    void RightRotate(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        if (y->right != nullptr) {
          y->right->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr) {
          root_ = y;
        } else if (x == x->parent->left) {
          x->parent->left = y;
        } else {
          x->parent->right = y;
        }
        y->right = x;
        x->parent = y;
    }
    //递归析构
    void DestroyTree(Node* node) {
      if (node != nullptr) {
        DestroyTree(node->left);
        DestroyTree(node->right);
        delete node;
      }
    }
    //修复函数
    void InsertFixup(Node* z) {
      while (z->parent != nullptr && z->parent->color == Color::kRed) { //z为根结点和z的父母节点为黑的情况不需要修复
        if (z->parent == z->parent->parent->left) {// 父节点是左子节点
          Node* y = z->parent->parent->right;  //找叔节点
          if (y != nullptr && y->color == Color::kRed) { //叔节点为红 -- 需要修复
            z->parent->color = Color::kBlack;
            y->color = Color::kBlack;
            z->parent->parent->color = Color::kRed;
            z = z->parent->parent; 
            //变色并向上传导
          } else {
            // 不为红 有空位 需要旋转+染色
            if (z == z->parent->right) { // LR 变为 LL
              z = z->parent;
              LeftRotate(z);
            }
            z->parent->color = Color::kBlack;
            z->parent->parent->color = Color::kRed;
            RightRotate(z->parent->parent);
            // 换颜色 右旋
          }
        } else {// 相反的逻辑
          Node* y = z->parent->parent->left;
          if (y != nullptr && y->color == Color::kRed) {
            z->parent->color = Color::kBlack;
            y->color = Color::kBlack;
            z->parent->parent->color = Color::kRed;
            z = z->parent->parent;
          } else {
            if (z == z->parent->left) {
              z = z->parent;
              RightRotate(z);
            }
            z->parent->color = Color::kBlack;
            z->parent->parent->color = Color::kRed;
            LeftRotate(z->parent->parent);
          }
        }
      }
      root_->color = Color::kBlack;
      // 满足根结点必黑
    }
    //中序遍历
    void InorderHelper(Node* node) const {
      if (node != nullptr) {
        InorderHelper(node->left);
        std::cout << node->data << (node->color == Color::kRed ? "(R) " : "(B) ");
        InorderHelper(node->right);
      }
    }
};
int main(){
  RedBlackTree<int> rbt;
  
  std::cout << "--- 正在插入数据 ---" << std::endl;
  // 这组数据会触发红黑树的各种变色和旋转机制
  int elements[] = {10, 20, 30, 15, 25, 5, 1};
  
  for (int el : elements) {
    auto result = rbt.Insert(el);
    if (result.first) {
      std::cout << "成功插入: " << el << std::endl;
    } else {
      std::cout << "插入失败，元素已存在: " << el << std::endl;
    }
  }

  std::cout << "\n--- 测试插入重复数据 ---" << std::endl;
  auto dup_result = rbt.Insert(15);
  if (!dup_result.first) {
    std::cout << "拦截重复数据成功: " << dup_result.second << std::endl;
  }

  std::cout << "\n--- 红黑树中序遍历结果 ---" << std::endl;
  // 中序遍历如果打印出来是从小到大严格递增的，说明二叉搜索树的性质得到了维护
  rbt.PrintOrder();

  std::cout << "\n程序运行结束，准备调用析构函数释放内存..." << std::endl;
  return 0;
}