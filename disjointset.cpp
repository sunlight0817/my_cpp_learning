#include <iostream>
#include <memory>
#include <utility>
#include <vector>
//路径压缩加按大小合并的并查集
class disjointset {
  public:
    explicit disjointset(int n) : parent_(n, -1) {}
    int find(int i) {
      if (parent_[i] < 0)
      return parent_[i] = find(parent_[i]);// 递归调用的同时进行路径压缩
    }
    /* 防止栈溢出的版本
    int find(int i) {
      int root = i;
      while (parent_[root] >= 0) {
        root = parent_[root];
      }
      
      int curr = i;
      while (curr != root) {
        int next_node = parent_[curr];
        parent_[curr] = root;
        curr = next_node;
      }
    }
    */

    bool Union(int i, int j) {
      int root_i = find(i);
      int root_j = find(j);
      if(root_i == root_j) return false;
      if(parent_[root_i] > parent_[root_j]) {
        parent_[root_j] += parent_[root_j];
        parent_[root_i] = root_j;
      }else {
        parent_[root_i] += parent_[root_j];
        parent_[root_j] = root_i;
      }
      return true;
    }
  private:
    std::vector<int> parent_;
};