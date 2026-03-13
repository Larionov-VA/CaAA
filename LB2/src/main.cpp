#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <functional>
#include <algorithm>

#define GRAPH_FILENAME "./files/graph.dot"
#define MST_FILENAME "./files/MST.dot"

#ifndef MAX_VERTEX_DEG
    #define MAX_VERTEX_DEG 5
#endif

struct edge {
    std::pair<int, int> vertexes;
    int weight;
};

using edgesStack = std::vector<edge>;
using matrix = std::vector<std::vector<int>>;

matrix getMatrixFromFile() {
    int squareMatrixSide;
    std::cin >> squareMatrixSide;
    matrix M(squareMatrixSide, std::vector<int>(squareMatrixSide));
    for (int i = 0; i < squareMatrixSide; ++i) {
        for (int j = 0; j < squareMatrixSide; ++j) {
            std::cin >> M[i][j];
        }
    }
    return M;
}

void matrixToDot(const matrix& M, const std::string& filename) {
    std::ofstream dotFile(filename);
    if (!dotFile.is_open()) {
        return;
    }
    dotFile << "graph G {\n";
    dotFile << "    node [shape=circle];\n\n";
    for (size_t i = 0; i < M.size(); ++i) {
        for (size_t j = i + 1; j < M.size(); ++j) {
            if(M[i][j]) {
                dotFile << "    " << i << " -- " << j
                        << " [label=\"" << M[i][j] << "\" color=\"green\" penwidth=\"2\"];\n";
            }
        }
    }
    dotFile << "}\n";
}

void printMatrix(const matrix& M) {
    for (const auto& row : M) {
        for (int val : row) {
            std::cout << val << ' ';
        }
        std::cout << '\n';
    }
}

edgesStack getEdgesStack(matrix& M) {
    edgesStack edges;
    for (size_t i = 0; i < M.size(); ++i) {
        for (size_t j = i + 1; j < M.size(); ++j) {
            if(M[i][j]) {
                edges.push_back({{i, j}, M[i][j]});
            }
        }
    }
    return edges;
}

bool isGraphComplete(matrix& M, edgesStack& edges) {
    return (M.size() * (M.size() - 1)) / 2 == edges.size();
}

bool isMatrixSimmetric(matrix& M) {
    for (size_t i = 0; i < M.size(); ++i) {
        for (size_t j = i + 1; j < M.size(); ++j) {
            if(M[i][j] != M[j][i]) {
                return false;
            }
        }
    }
    return true;
}

bool isEdgesLooped(edgesStack& edges, int numVertices) {
    if (edges.empty()) {
        return false;
    }
    std::vector<int> parent(numVertices);
    for (int i = 0; i < numVertices; ++i) {
        parent[i] = i;
    }
    for (const auto& e : edges) {
        int root1 = e.vertexes.first;
        while (parent[root1] != root1) {
            root1 = parent[root1];
        }
        int root2 = e.vertexes.second;
        while (parent[root2] != root2) {
            root2 = parent[root2];
        }
        if (root1 == root2) {
            return true;
        }
        parent[root1] = root2;
    }
    return false;
}

void kruskalAlgorithm() {

}

int findMSTBranchAndBound(int maxVertexDeg) {
    matrix M = getMatrixFromFile();
    if (!isMatrixSimmetric(M)) {
        return EXIT_FAILURE;
    }
    edgesStack edges = getEdgesStack(M);
    // std::cout << '\n';
    // std::cout << isGraphComplete(M, edges);
    // std::cout << '\n';
    // std::cout << isMatrixSimmetric(M);
    // std::cout << '\n';
    std::cout << isEdgesLooped(edges, M.size());
    std::cout << '\n';
    for (auto& edge : getEdgesStack(M)) {
        std::cout << edge.vertexes.first << ' ' << edge.vertexes.second << ' ' << edge.weight << '\n';
    }
    // printMatrix(M);
    // matrixToDot(M, GRAPH_FILENAME);
    // matrixToDot(M, MST_FILENAME);
    return EXIT_SUCCESS;
}

int main() {
    return findMSTBranchAndBound(MAX_VERTEX_DEG);
}