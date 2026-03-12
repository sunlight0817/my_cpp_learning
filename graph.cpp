#include <iostream>
#include <stdexcept>
#include <vector>

using NodeId = int;
using Weight = double;

struct Edge {
  NodeId to;
  Weight weight;
};

class Graph {
  public:
    
    explicit Graph(size_t num_vertices) : adj_list_(num_vertices) {}


    ~Graph() = default;

    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;
    Graph(Graph&&) noexcept = default;
    Graph& operator=(Graph&&) noexcept = default;

    // 添加有向边
    void AddDirectedEdge(NodeId from, NodeId to, Weight weight) {
      ValidateNodeId(from);
      ValidateNodeId(to);
      adj_list_[from].push_back({to, weight});
    }

    void AddUndirectedEdge(NodeId u, NodeId v, Weight weight) {
      AddDirectedEdge(u, v, weight);
      AddDirectedEdge(v, u, weight);
    }
    // 获取某个节点的所有邻居
    // [[nodiscard]] 是 C++17 特性，警告调用者不要忽略这个函数的返回值
    // 返回 const 引用，避免拷贝开销，同时保证外部无法随意修改图的内部结构
    [[nodiscard]] const std::vector<Edge>& GetNeighbors(NodeId node) const {
      ValidateNodeId(node);
      return adj_list_[node];
    }

    [[nodiscard]] size_t GetNumVertices() const {
      return adj_list_.size();
    }

  private:
    //判断越界函数  
    void ValidateNodeId(NodeId node) const {
      if(node < 0 || static_cast<size_t>(node) >= adj_list_.size()) {
        throw std::out_of_range("NodeId is out of bounds");   
      }
    }
    //邻接表
    std::vector<std::vector<Edge>> adj_list_;
};

int main() {
  // 创建一个包含 5 个节点的无向图
  const size_t num_nodes = 5;
  Graph graph(num_nodes);

  // 添加边
  graph.AddUndirectedEdge(0, 1, 4.0);
  graph.AddUndirectedEdge(0, 2, 1.0);
  graph.AddUndirectedEdge(1, 3, 1.0);
  graph.AddUndirectedEdge(2, 3, 5.0);
  graph.AddUndirectedEdge(3, 4, 3.0);

  // 遍历并打印图的结构
  std::cout << "图的邻接表表示：\n";
  std::cout << "------------------------------------\n";
  for (NodeId i = 0; i < num_nodes; ++i) {
    std::cout << "节点 " << i << " 的邻居: ";
    // 使用范围 for 循环和 const auto& 避免拷贝
    for (const auto& edge : graph.GetNeighbors(i)) {
      std::cout << "[" << edge.to << " (权重: " << edge.weight << ")] ";
    }
    std::cout << "\n";
  }

  return 0;
}