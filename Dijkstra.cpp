#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <limits>
#include <queue>
#include <algorithm>

using Node = int;
using Weight = double;

constexpr Weight INF = std::numeric_limits<Weight>::infinity();

struct Edge {
    Node to;
    Weight weight;
};

struct PathResult {
    std::vector<Weight> distances;
    std::vector<Node> predecessors;
};
