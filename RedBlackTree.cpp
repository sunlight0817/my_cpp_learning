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
    // ================= 新增：删除操作入口 =================
    bool Remove(const T& key) {
      Node* z = root_;
      // 1. 查找要删除的节点
      while (z != nullptr) {
        if (key < z->data) {
          z = z->left;
        } else if (key > z->data) {
          z = z->right;
        } else {
          break; // 找到了
        }
      }
      
      if (z == nullptr) return false; // 没找到该节点

      Node* y = z;
      Color y_original_color = y->color;
      Node* x = nullptr;
      Node* x_parent = nullptr; // 关键：当 x 是 nullptr 时，用于记录其父节点

      // 2. 标准的二叉搜索树删除逻辑
      if (z->left == nullptr) {
        x = z->right;
        x_parent = z->parent;
        Transplant(z, z->right);
      } else if (z->right == nullptr) {
        x = z->left;
        x_parent = z->parent;
        Transplant(z, z->left);
      } else {
        // 找右子树中的最小节点作为后继
        y = Minimum(z->right);
        y_original_color = y->color;
        x = y->right;
        
        if (y->parent == z) {
          x_parent = y; 
        } else {
          x_parent = y->parent;
          Transplant(y, y->right);
          y->right = z->right;
          y->right->parent = y;
        }
        
        Transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
      }

      delete z; // 释放原节点内存

      // 3. 如果删掉（或移走）的节点是黑色，红黑树性质被破坏，需要修复
      if (y_original_color == Color::kBlack) {
        DeleteFixup(x, x_parent);
      }
      return true;
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
    // ================= 新增：子树移植操作 (用于删除) =================
    void Transplant(Node* u, Node* v) {
      if (u->parent == nullptr) {
        root_ = v;
      } else if (u == u->parent->left) {
        u->parent->left = v;
      } else {
        u->parent->right = v;
      }
      if (v != nullptr) {
        v->parent = u->parent;
      }
    }
    // ================= 新增：查找最小节点 (用于找后继) =================
    Node* Minimum(Node* node) {
      while (node->left != nullptr) {
        node = node->left;
      }
      return node;
    }

    // ================= 新增：删除修复逻辑 (解决双黑问题) =================
    void DeleteFixup(Node* x, Node* x_parent) {
      while (x != root_ && (x == nullptr || x->color == Color::kBlack)) {
        if (x == x_parent->left) {
          Node* w = x_parent->right; // w 是 x 的兄弟节点
          if (w == nullptr) break;

          // 情况 1：兄弟节点是红色的
          if (w->color == Color::kRed) {
            w->color = Color::kBlack;
            x_parent->color = Color::kRed;
            LeftRotate(x_parent);
            w = x_parent->right;
          }

          // 情况 2：兄弟节点是黑色的，且兄弟的两个子节点都是黑色的 (nullptr 视为黑色)
          if ((w->left == nullptr || w->left->color == Color::kBlack) &&
              (w->right == nullptr || w->right->color == Color::kBlack)) {
            w->color = Color::kRed;
            x = x_parent;
            x_parent = x->parent; // 向上回溯
          } else {
            // 情况 3：兄弟节点是黑色的，兄弟的左孩子是红色的，右孩子是黑色的
            if (w->right == nullptr || w->right->color == Color::kBlack) {
              if (w->left != nullptr) w->left->color = Color::kBlack;
              w->color = Color::kRed;
              RightRotate(w);
              w = x_parent->right;
            }
            // 情况 4：兄弟节点是黑色的，兄弟的右孩子是红色的
            w->color = x_parent->color;
            x_parent->color = Color::kBlack;
            if (w->right != nullptr) w->right->color = Color::kBlack;
            LeftRotate(x_parent);
            x = root_; // 结束循环
          }
        } else {
          // 对称情况：x 是父节点的右孩子
          Node* w = x_parent->left;
          if (w == nullptr) break;

          if (w->color == Color::kRed) {
            w->color = Color::kBlack;
            x_parent->color = Color::kRed;
            RightRotate(x_parent);
            w = x_parent->left;
          }

          if ((w->right == nullptr || w->right->color == Color::kBlack) &&
              (w->left == nullptr || w->left->color == Color::kBlack)) {
            w->color = Color::kRed;
            x = x_parent;
            x_parent = x->parent;
          } else {
            if (w->left == nullptr || w->left->color == Color::kBlack) {
              if (w->right != nullptr) w->right->color = Color::kBlack;
              w->color = Color::kRed;
              LeftRotate(w);
              w = x_parent->left;
            }
            w->color = x_parent->color;
            x_parent->color = Color::kBlack;
            if (w->left != nullptr) w->left->color = Color::kBlack;
            RightRotate(x_parent);
            x = root_;
          }
        }
      }
      // 将最终上移的节点染成黑色
      if (x != nullptr) {
        x->color = Color::kBlack;
      }
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
  int elements[] = {10, 20, 30, 15, 25, 5, 1};
  for (int el : elements) {
    rbt.Insert(el);
  }

  std::cout << "\n插入完毕，当前红黑树中序遍历：" << std::endl;
  rbt.PrintOrder();

  std::cout << "\n--- 测试删除数据 ---" << std::endl;
  int to_delete[] = {15, 10, 30}; // 测试删除不同位置的节点
  for (int el : to_delete) {
    if (rbt.Remove(el)) {
      std::cout << "成功删除: " << el << " -> 树状: ";
      rbt.PrintOrder();
    } else {
      std::cout << "未找到要删除的元素: " << el << std::endl;
    }
  }

  std::cout << "\n程序运行结束，准备调用析构函数释放内存..." << std::endl;
  return 0;
}