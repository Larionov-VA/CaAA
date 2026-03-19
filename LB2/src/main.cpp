#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <functional>
#include <algorithm>
#include <limits>

#define ALPHABET "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

#define GRAPH_FILENAME "./files/graph.dot"
#define MST_FILENAME "./files/MST.dot"

#ifndef MAX_VERTEX_DEG
    #define MAX_VERTEX_DEG 5
#endif

struct edge {
    std::pair<int, int> vertexes;
    int weight;
    bool operator == (const edge& counter) const
    {
        return vertexes == counter.vertexes && weight == counter.weight;
    }
};

using edgesStack = std::vector<edge>;
using matrix = std::vector<std::vector<int>>;

struct DSU {
    std::vector<int> parent, rank;
    DSU(int n) : parent(n), rank(n, 0) {
        for (int i = 0; i < n; ++i)
            parent[i] = i;
    }
    int find(int v) {
        if (v == parent[v]) return v;
        return parent[v] = find(parent[v]);
    }
    bool unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) {
            return false;
        }
        if (rank[a] < rank[b]) {
            std::swap(a, b);
        }
        parent[b] = a;
        if (rank[a] == rank[b]) {
            rank[a]++;
        }
        return true;
    }
};

matrix getMatrixFromFile() {
    int n;
    std::cin >> n;
    matrix M(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::cin >> M[i][j];
        }
    }
    return M;
}

// void matrixToDot(const matrix& M, const std::string& filename) {
//     std::ofstream dotFile(filename);
//     if (!dotFile.is_open()) {
//         return;
//     }
//     dotFile << "graph M {\n";
//     dotFile << "    node [shape=circle];\n\n";
//     for (size_t i = 0; i < M.size(); ++i) {
//         for (size_t j = i + 1; j < M.size(); ++j) {
//             if (M[i][j]) {
//                 dotFile << "    " << ALPHABET[i] << " -- " << ALPHABET[j]
//                         << " [label=\"" << M[i][j]
//                         << "\" color=\"red\" penwidth=\"2\"];\n";
//             }
//         }
//     }
//     // dotFile << "}\n";
// }

void edgesToDot(const edgesStack& edges, const edgesStack& MSTedges, const std::string& filename) {
    std::ofstream dotFile(filename);
    if (!dotFile.is_open()) {
        return;
    }
    dotFile << "graph G {\n";
    dotFile << "    node [shape=circle];\n\n";
    dotFile << "\n";
    for (const auto& e : edges) {
        dotFile << "    "
                << ALPHABET[e.vertexes.first] << " -- "
                << ALPHABET[e.vertexes.second]
                << " [label=\"" << e.weight;
        if (std::find(MSTedges.begin(), MSTedges.end(), e) == MSTedges.end()) {
            dotFile << "\" color=\"red\" penwidth=\"1\"];\n";
        }
        else {
            dotFile << "\" color=\"green\" penwidth=\"2\"];\n";
        }
    }
    dotFile << "}\n";
}

bool isMatrixSimmetric(matrix& M) {
    for (size_t i = 0; i < M.size(); ++i) {
        for (size_t j = i + 1; j < M.size(); ++j) {
            if (M[i][j] != M[j][i]) {
                return false;
            }
        }
    }
    return true;
}

edgesStack getEdgesStack(matrix& M) {
    edgesStack edges;
    for (size_t i = 0; i < M.size(); ++i) {
        for (size_t j = i + 1; j < M.size(); ++j) {
            if (M[i][j]) {
                edges.push_back({{{(int)i}, {(int)j}}, M[i][j]});
            }
        }
    }
    return edges;
}

int kruskalLowerBound(const edgesStack& edges, int n, bool degLimit) {
    DSU dsu(n);
    int weight = 0;
    int cnt = 0;
    for (const auto& e : edges) {
        int u = e.vertexes.first;
        int v = e.vertexes.second;
        if (dsu.unite(u, v)) {
            weight += e.weight;
            cnt++;
            if (cnt == n - 1) {
                break;
            }
        }
    }
    if (cnt != n - 1) {
        return std::numeric_limits<int>::max();
    }
    return weight;
}

void findMSTRecursive(
    edgesStack& edges,
    int index,
    std::vector<int>& degree,
    DSU dsu,
    int currentWeight,
    int& bestWeight,
    edgesStack& currentEdges,
    edgesStack& bestEdges,
    int n
) {
    if (currentEdges.size() == n - 1) {
        if (currentWeight < bestWeight) {
            bestWeight = currentWeight;
            bestEdges = currentEdges;
        }
        return;
    }
    if (index >= edges.size()) {
        return;
    }
    edgesStack remaining(edges.begin() + index, edges.end());
    int bound = kruskalLowerBound(remaining, n, false);
    if (bound == std::numeric_limits<int>::max()) {
        return;
    }
    if (currentWeight + bound >= bestWeight) {
        return;
    }
    auto& e = edges[index];
    int u = e.vertexes.first;
    int v = e.vertexes.second;
    if (degree[u] < MAX_VERTEX_DEG && degree[v] < MAX_VERTEX_DEG) {
        DSU dsuCopy = dsu;
        if (dsuCopy.unite(u, v)) {
            degree[u]++;
            degree[v]++;
            currentEdges.push_back(e);
            findMSTRecursive(
                edges, index + 1,
                degree, dsuCopy,
                currentWeight + e.weight,
                bestWeight,
                currentEdges, bestEdges,
                n
            );
            currentEdges.pop_back();
            degree[u]--;
            degree[v]--;
        }
    }
    findMSTRecursive(
        edges, index + 1,
        degree, dsu,
        currentWeight,
        bestWeight,
        currentEdges, bestEdges,
        n
    );
}

int findMSTBranchAndBound(int maxVertexDeg) {
    matrix M = getMatrixFromFile();
    // matrixToDot(M, GRAPH_FILENAME);
    if (!isMatrixSimmetric(M)) {
        return EXIT_FAILURE;
    }
    edgesStack edges = getEdgesStack(M);
    std::sort(edges.begin(), edges.end(),
        [](const edge& a, const edge& b) {
            return a.weight < b.weight;
        });
    int n = M.size();
    std::vector<int> degree(n, 0);
    DSU dsu(n);
    int bestWeight = std::numeric_limits<int>::max();
    edgesStack currentEdges, bestEdges;
    findMSTRecursive(
        edges, 0,
        degree, dsu,
        0,
        bestWeight,
        currentEdges,
        bestEdges,
        n
    );
    std::cout << "BEST MST WEIGHT = " << bestWeight << '\n';
    std::cout << "Edges:\n";
    for (auto& e : bestEdges) {
        std::cout << e.vertexes.first << " - "
                  << e.vertexes.second
                  << " (" << e.weight << ")\n";
    }
    edgesToDot(edges, bestEdges, GRAPH_FILENAME);
    return EXIT_SUCCESS;
}

int main() {
    return findMSTBranchAndBound(MAX_VERTEX_DEG);
}