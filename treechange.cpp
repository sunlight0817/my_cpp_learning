#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace tree_conversion {

template <typename T>
struct TreeNode {
  T value;
  std::vector<std::unique_ptr<TreeNode>> children;

  explicit TreeNode(T val) : value(val) {}
};

template <typename T>
struct BinaryTreeNode {
  T value;
  std::unique_ptr<BinaryTreeNode> left;
  std::unique_ptr<BinaryTreeNode> right;

  explicit BinaryTreeNode(T val) : value(val) {}
};

namespace detail {
template <typename T>
std::unique_ptr<BinaryTreeNode<T>> ConvertForestToBinaryTreeHelper(
    const std::vector<std::unique_ptr<TreeNode<T>>>& forest,
    size_t index) {

  if (index >= forest.size() || forest[index] == nullptr) {
    return nullptr;
  }

  auto binary_node =
      std::make_unique<BinaryTreeNode<T>>(forest[index]->value);

  binary_node->left =
      ConvertForestToBinaryTreeHelper<T>(forest[index]->children, 0);
  
  binary_node->right =
      ConvertForestToBinaryTreeHelper<T>(forest, index + 1);

  return binary_node;
}
}  // namespace detail

template <typename T>
std::unique_ptr<BinaryTreeNode<T>> ConvertForestToBinaryTree(
    const std::vector<std::unique_ptr<TreeNode<T>>>& forest) {
  return detail::ConvertForestToBinaryTreeHelper<T>(forest, 0);
}

template <typename T>
std::vector<std::unique_ptr<TreeNode<T>>> ConvertBinaryTreeToForest(
    const BinaryTreeNode<T>* binary_root) {
  
  std::vector<std::unique_ptr<TreeNode<T>>> forest;
  const BinaryTreeNode<T>* current = binary_root;

  while (current != nullptr) {
    auto tree_node = std::make_unique<TreeNode<T>>(current->value);
    
    // 左子树还原为当前节点的所有孩子
    tree_node->children = ConvertBinaryTreeToForest<T>(current->left.get());
    
    forest.push_back(std::move(tree_node));

    // 移动到下一个兄弟节点 (右指针)
    current = current->right.get();
  }

  return forest;
}
// 打印多叉树/森林
template <typename T>
void PrintForest(const std::vector<std::unique_ptr<TreeNode<T>>>& forest, 
                 int depth = 0) {
  std::string indent(depth * 2, ' ');
  for (const auto& node : forest) {
    if (node) {
      std::cout << indent << "- Node " << node->value << "\n";
      PrintForest<T>(node->children, depth + 1);
    }
  }
}

// 打印二叉树
template <typename T>
void PrintBinaryTree(const BinaryTreeNode<T>* root, 
                     const std::string& prefix = "", 
                     bool is_left = true) {
  if (root == nullptr) return;
  
  std::cout << prefix;
  std::cout << (is_left ? "├── L: " : "└── R: ") << root->value << "\n";
  
  std::string next_prefix = prefix + (is_left ? "│   " : "    ");
  PrintBinaryTree<T>(root->left.get(), next_prefix, true);
  PrintBinaryTree<T>(root->right.get(), next_prefix, false);
}

}  // namespace tree_conversion
int main() {
  using namespace tree_conversion;

  // 1. 构建一个测试森林 (此次使用泛型指定为 int)
  // 森林结构:
  // Tree 1 (Root: 1)       Tree 2 (Root: 4)
  //  ├── 2                  └── 5
  //  └── 3
  
  std::vector<std::unique_ptr<TreeNode<int>>> original_forest;

  // 构建第一棵树
  auto tree1 = std::make_unique<TreeNode<int>>(1);
  tree1->children.push_back(std::make_unique<TreeNode<int>>(2));
  tree1->children.push_back(std::make_unique<TreeNode<int>>(3));
  original_forest.push_back(std::move(tree1));

  // 构建第二棵树
  auto tree2 = std::make_unique<TreeNode<int>>(4);
  tree2->children.push_back(std::make_unique<TreeNode<int>>(5));
  original_forest.push_back(std::move(tree2));

  std::cout << "=== 原始森林结构 ===\n";
  PrintForest<int>(original_forest);
  std::cout << "\n";

  // 2. 将森林转换为二叉树
  // 调用公开 API，无需传递 index=0 参数
  std::unique_ptr<BinaryTreeNode<int>> binary_tree = 
      ConvertForestToBinaryTree<int>(original_forest);

  std::cout << "=== 转换后的二叉树 (L: 左孩子, R: 右兄弟) ===\n";
  PrintBinaryTree<int>(binary_tree.get()); 
  std::cout << "\n";

  // 3. 将二叉树还原回森林
  std::vector<std::unique_ptr<TreeNode<int>>> restored_forest = 
      ConvertBinaryTreeToForest<int>(binary_tree.get());

  std::cout << "=== 还原后的森林结构 ===\n";
  PrintForest<int>(restored_forest);
  std::cout << "\n";

  return 0;
}