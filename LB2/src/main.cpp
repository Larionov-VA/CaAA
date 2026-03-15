#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <limits>

#define GRAPH_FILENAME "./files/graph.dot"
#ifndef MAX_VERTEX_DEG
    #define MAX_VERTEX_DEG 5
#endif
#ifndef SHOW_INFO
    #define SHOW_INFO 0
#endif
#if SHOW_INFO
    #define LOG_FILENAME "./files/log.txt"
    std::ofstream logFile(LOG_FILENAME);
    int logCounter = 0;
#endif

constexpr std::string_view ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

struct Edge {
    std::pair<int, int> vertexes;
    int weight;
    bool operator == (const Edge& counter) const
    {
        return vertexes == counter.vertexes && weight == counter.weight;
    }
};

using edgesStack = std::vector<Edge>;
using Matrix = std::vector<std::vector<int>>;

struct DSU {
    std::vector<int> parent, rank;
    DSU(int n) : parent(n), rank(n, 0) {
        for (int i = 0; i < n; ++i)
            parent[i] = i;
    }
    int find(int v) {
        if (v == parent[v]) {
            return v;
        }
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

Matrix getMatrixFromStream() {
    int n;
    std::cin >> n;
    Matrix M(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::cin >> M[i][j];
        }
    }
    return M;
}

#if SHOW_INFO
void infoToLogFile(const std::string info) {
    if (logFile) {
        logFile << logCounter++ << "\n";
        std::stringstream ss(info);
        std::string token;
        while (std::getline(ss, token, '\n')) {
            logFile << "\t" << token << std::endl;
        }
    }
}

std::string matrixToString(const Matrix& M) {
    std::string strMatrix;
    size_t squareMatrixSide = M.size();
    for (size_t i = 0; i < squareMatrixSide; ++i) {
        strMatrix += "\t";
        strMatrix += ALPHABET[i];
    }
    strMatrix += "\n\n";
    for (size_t i = 0; i < squareMatrixSide; ++i) {
        strMatrix += ALPHABET[i];
        strMatrix += "\t";
        for (size_t j = 0; j < squareMatrixSide; ++j) {
            strMatrix += std::to_string(M[i][j]);
            strMatrix += "\t";
        }
        strMatrix += "\n\n";
    }
    return strMatrix;
}
#endif

void edgesToDot(
    const edgesStack& edges,
    const edgesStack& MSTedges,
    const std::string& filename
) {
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

bool isMatrixSymmetric(const Matrix& M) {
    for (size_t i = 0; i < M.size(); ++i) {
        for (size_t j = i + 1; j < M.size(); ++j) {
            if (M[i][j] != M[j][i]) {
                return false;
            }
        }
    }
    return true;
}

edgesStack getEdgesStack(const Matrix& M) {
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

int kruskalLowerBound(const edgesStack& edges, DSU dsu, int n, bool digitLimit = false) {
    int weight = 0;
    int cnt = 0;
    std::vector<int> rank(edges.size(), 0);
    for (int i = 0; i < n; ++i) {
        if (dsu.find(i) == i) {
            cnt++;
        }
    }
    cnt = n - cnt;
    for (const auto& e : edges) {
        int u = e.vertexes.first;
        int v = e.vertexes.second;
        if (digitLimit) {
            if (rank[u] >= MAX_VERTEX_DEG || rank[v] >= MAX_VERTEX_DEG) {
                continue;
            }
        }
        if (dsu.unite(u, v)) {
            if (digitLimit) {
                rank[u]++; rank[v]++;
            }
            weight += e.weight;
            cnt++;
            if (cnt == n - 1) {
                return weight;
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
    #if SHOW_INFO
    std::string strBestWeight = \
    bestWeight == std::numeric_limits<int>::max() ? "inf" : std::to_string(bestWeight);
    infoToLogFile(
        std::string("Вызвана рекурсивная функция findMSTRecursive\n") +
        "Лучшее решение: " + strBestWeight +
        "\nТекущее решение: " + std::to_string(currentWeight)
    );
    #endif
    if (currentEdges.size() == n - 1) {
        #if SHOW_INFO
        infoToLogFile("Текущее решение содержит " +
            std::to_string(n - 1) +
            " рёбер, оно окончательно для графа из " +
            std::to_string(n) +
            " вершин.\nПроверим лучше ли оно предыдущего:"
        );
        #endif
        if (currentWeight < bestWeight) {
            #if SHOW_INFO
            std::string strBestWeight = \
            bestWeight == std::numeric_limits<int>::max() ? "inf" : std::to_string(bestWeight);
            infoToLogFile("Да, currentWeight = " +
                std::to_string(currentWeight) +
                ", а bestWeight = " +
                strBestWeight +
                ". Заменяем лучшее решение текущим."
            );
            #endif
            bestWeight = currentWeight;
            bestEdges = currentEdges;
        }
        #if SHOW_INFO
        else {
            infoToLogFile("Нет, currentWeight = " +
                std::to_string(currentWeight) +
                ", а bestWeight = " +
                std::to_string(bestWeight) +
                ". Оставляем все как есть."
            );
        }
        #endif
        return;
    }
    if (index >= edges.size()) {
        #if SHOW_INFO
        infoToLogFile(
            "Обращение к несуществующему ребру, \
            обрезаю текущую ветвь рекурсии"
        );
        #endif
        return;
    }
    edgesStack remaining(edges.begin() + index, edges.end());
    int bound = kruskalLowerBound(remaining, dsu, n);
    if (bound == std::numeric_limits<int>::max()) {
        #if SHOW_INFO
        infoToLogFile(
            std::string("Оценка МОД на оставшихся вершинах методом Крускала показала, \n") +
            "что построить МОД с текущими вершинами невозможно, \n" +
            "обрезую эту ветвь рекурсии."
        );
        #endif
        return;
    }
    if (currentWeight + bound >= bestWeight) {
        #if SHOW_INFO
        infoToLogFile(
            "currentWeight = " +
            std::to_string(currentWeight) +
            "\tbound = " +
            std::to_string(bound) +
            "\tbestWeight = " +
            std::to_string(bestWeight) +
            "\n" + std::to_string(currentWeight) + " + " + std::to_string(bound) + " >= " + std::to_string(bestWeight) +
            "\nТекущее решение + нижняя оценка МОД на оставшихся вершинах больше или равна лучшему решению \n" +
            "дальше эту ветвь продолжать не имеет смысла, обрезаю ее."
        );
        #endif
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
            #if SHOW_INFO
            infoToLogFile(
                std::string("Добавляем ребро ") +
                ALPHABET[u] +
                " -- " +
                ALPHABET[v] +
                + " с весом " +
                std::to_string(e.weight) +
                " в текущее решение"
            );
            #endif
            findMSTRecursive(
                edges, index + 1,
                degree, dsuCopy,
                currentWeight + e.weight,
                bestWeight,
                currentEdges, bestEdges,
                n
            );
            #if SHOW_INFO
            infoToLogFile(
                std::string("Удаляем ребро ") +
                ALPHABET[u] +
                " -- " +
                ALPHABET[v] +
                + " с весом " +
                std::to_string(e.weight) +
                " из текущего решения"
            );
            #endif
            currentEdges.pop_back();
            degree[u]--;
            degree[v]--;
        }
        #if SHOW_INFO
        else {
            infoToLogFile(
                std::string("Ребро ") +
                ALPHABET[u] +
                " -- " +
                ALPHABET[v] +
                + " образует цикл, что недопустимо." +
                "\nПерехожу к рассмотрению следующего ребра."
            );
        }
        #endif
    }
    #if SHOW_INFO
    else {
        infoToLogFile(
            std::string("Ребро ") +
            ALPHABET[u] +
            " -- " +
            ALPHABET[v] +
            + " нарушает правило ограничения на степень вершины." +
            "\nСтепень вершины " +
            ALPHABET[u] + ": " +
            std::to_string(degree[u]) +
            "\nСтепень вершины " +
            ALPHABET[v] + ": " +
            std::to_string(degree[v]) +
            "\nПерехожу к рассмотрению следующего ребра."
        );
    }
    #endif
    findMSTRecursive(
        edges, index + 1,
        degree, dsu,
        currentWeight,
        bestWeight,
        currentEdges, bestEdges,
        n
    );
    #if SHOW_INFO
    strBestWeight = \
    bestWeight == std::numeric_limits<int>::max() ? "inf" : std::to_string(bestWeight);
    std::string strDSU;
    for (auto p : dsu.parent) {
        strDSU += std::to_string(p);
        strDSU += " ";
    }
    infoToLogFile(
        std::string("Рассмотрены все ветви рекурсии. Функция нашла МОД с весом: ") +
        strBestWeight + "\n" +
        strDSU
    );
    #endif
}

int findMSTBranchAndBound() {
    Matrix M = getMatrixFromStream();
    #if SHOW_INFO
    infoToLogFile(matrixToString(M));
    #endif
    if (!isMatrixSymmetric(M)) {
        #if SHOW_INFO
        infoToLogFile("Матрица не симметрична, прерываю");
        #endif
        return EXIT_FAILURE;
    }
    edgesStack edges = getEdgesStack(M);
    std::sort(edges.begin(), edges.end(),
        [](const Edge& a, const Edge& b) {
            return a.weight < b.weight;
        });
    int n = M.size();
    std::vector<int> degree(n, 0);
    DSU dsu(n);
    std::cout << kruskalLowerBound(edges, dsu, n, true) << '\n';
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
    if (bestWeight == std::numeric_limits<int>::max()) {
        std::cout << "no path\n";
        return EXIT_FAILURE;
    }
    else {
        std::cout << bestWeight << '\n';
        for (auto& e : bestEdges) {
            std::cout << ALPHABET[e.vertexes.first] << " -- "
                      << ALPHABET[e.vertexes.second] << "\t["
                      << e.weight
                      << "]"
                      << "\n";
        }
        edgesToDot(edges, bestEdges, GRAPH_FILENAME);
    }
    return EXIT_SUCCESS;
}

int main() {
    return findMSTBranchAndBound();
}