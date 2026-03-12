#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <limits>
#include <queue>
#include <algorithm>

using Node = int;
using Weight = double;

constexpr Weight kInfinity = std::numeric_limits<Weight>::infinity();

struct Edge {
  Node to;
  Weight weight;
};

struct PathResult {
  std::vector<Weight> distances;
  std::vector<Node> predecessors;
};

PathResult Dijkstra(const std::vector<std::vector<Edge>>& graph, Node start) {
  const size_t n = graph.size();

  std::vector<Weight> dist(n, kInfinity);
  std::vector<Node> prev(n, -1);
  dist[start] = 0;

  using QueueElement = std::pair<Weight, Node>;
  std::priority_queue<QueueElement, std::vector<QueueElement>,
                      std::greater<QueueElement>>
      pq;
  pq.emplace(0, start);

  while (!pq.empty()) {
    auto [current_dist, u] = pq.top();
    pq.pop();

    if (current_dist > dist[u]) {
      continue;
    } 
    /*
    Lazy Deletion 
    std::priority_queue 不支持直接删或修改，直接压入多个历史记录
    遇到过时数据时跳过
    */


    for (const auto& edge : graph[u]) {
      const Weight new_dist = dist[u] + edge.weight;

      if (new_dist < dist[edge.to]) {
        dist[edge.to] = new_dist;
        prev[edge.to] = u;
        pq.emplace(new_dist, edge.to);
      }
    }
  }

  return {dist, prev};
} 

void PrintPath(const std::vector<Node>& prev, Node target) {
  if (prev[target] == -1) {
    std::cout << target;
    return;
  }
  PrintPath(prev, prev[target]);
  std::cout << "->" << target;
}

int main() {
  const size_t num_nodes = 5;
  std::vector<std::vector<Edge>> graph(num_nodes);

  graph[0].push_back({1, 4.0});
  graph[0].push_back({2, 1.0});
  graph[1].push_back({3, 1.0});
  graph[2].push_back({1, 2.0});
  graph[2].push_back({3, 5.0});
  graph[3].push_back({4, 3.0});

  const Node start_node = 0;

  const PathResult result = Dijkstra(graph, start_node);

  std::cout << "从节点 " << start_node << " 出发的最短路径结果：\n";
  std::cout << "------------------------------------\n";
  for (Node i = 0; i < num_nodes; ++i) {
    std::cout << "到节点 " << i << " 的最短距离: ";
    if (result.distances[i] == kInfinity) {
      std::cout << "不可达\n";
    } else {
      std::cout << result.distances[i] << " \t路径: ";
      PrintPath(result.predecessors, i);
      std::cout << "\n";
    }
  }

  return 0;
}