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

constexpr std::string_view ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

struct edge {
    std::pair<int, int> vertexes;
    int weight;
    bool operator == (const edge& counter) const
    {
        return vertexes == counter.vertexes && weight == counter.weight;
    }
};

struct recursiveInfoStruct {

};

using edgesStack = std::vector<edge>;
using matrix = std::vector<std::vector<int>>;

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

    std::string matrixToString(const matrix& M) {
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

    std::string vertexesToStringEdge(const int& vertexU, const int& vertexV) {
        return ALPHABET[vertexU] + " -- " + ALPHABET[vertexV];
    }

    std::string edgesToString(const edgesStack& edges) {
        std::string strCurrentEdges;
        for (const edge& edge : edges) {
            int vertexU = edge.vertexes.first;
            int vertexV = edge.vertexes.second;
            strCurrentEdges += vertexesToStringEdge(vertexU, vertexV);
            strCurrentEdges += "\n";
        }
        return strCurrentEdges;
    }

    std::string bestWeightString(const int& bestWeight) {
        return bestWeight == std::numeric_limits<int>::max() ?
        "inf" : std::to_string(bestWeight);
    }
#endif

struct DSU {
    std::vector<int> sets, rank;
    DSU(int setsCount) : sets(setsCount), rank(setsCount, 0) {
        for (int i = 0; i < setsCount; ++i)
            sets[i] = i;
    }
    int find(int indexInSet) {
        if (indexInSet == sets[indexInSet]) {
            return indexInSet;
        }
        return sets[indexInSet] = find(sets[indexInSet]);
    }
    bool unite(int setA, int setB) {
        setA = find(setA);
        setB = find(setB);
        if (setA == setB) {
            return false;
        }
        if (rank[setA] < rank[setB]) {
            std::swap(setA, setB);
        }
        sets[setB] = setA;
        if (rank[setA] == rank[setB]) {
            rank[setA]++;
        }
        return true;
    }
};

matrix getMatrixFromStream() {
    int vertexesCount;
    std::cin >> vertexesCount;
    matrix M(vertexesCount, std::vector<int>(vertexesCount));
    for (int i = 0; i < vertexesCount; ++i) {
        for (int j = 0; j < vertexesCount; ++j) {
            std::cin >> M[i][j];
        }
    }
    return M;
}

bool isEdgeIn(const edgesStack& edges, const edge& edge) {
    return std::find(edges.begin(), edges.end(), edge) == edges.end();
}

void edgesToDot(
    const edgesStack& edges,
    const edgesStack& MSTedges,
    const std::string& filename
) {
    std::ofstream dotFile(filename);
    if (!dotFile.is_open()) {
        return;
    }
    std::string dotInfoString("graph G {\n\tnode [shape=circle];\n");
    for (const auto& currentEdge : edges) {
        const int& vertexU = currentEdge.vertexes.first;
        const int& vertexV = currentEdge.vertexes.second;
        dotInfoString += "\t";
        dotInfoString += ALPHABET[vertexU];
        dotInfoString += " -- ";
        dotInfoString += ALPHABET[vertexV];
        dotInfoString += " [label=\"";
        dotInfoString += std::to_string(currentEdge.weight);
        if (isEdgeIn(MSTedges, currentEdge)) {
            dotInfoString += "\" color=\"red\" penwidth=\"1\"];\n";
        }
        else {
            dotInfoString += "\" color=\"green\" penwidth=\"2\"];\n";
        }
    }
    dotInfoString += "}\n";
    dotFile << dotInfoString;
}

bool isMatrixSymmetric(const matrix& M) {
    for (size_t i = 0; i < M.size(); ++i) {
        for (size_t j = i + 1; j < M.size(); ++j) {
            if (M[i][j] != M[j][i]) {
                return false;
            }
        }
    }
    return true;
}

edgesStack getEdgesStack(const matrix& M) {
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

int kruskalLowerBound(
    const edgesStack& edges,
    DSU dsu,
    int vertexesCount,
    bool digitLimit = false
) {
    int MSTweight = 0;
    int currentVertexNumber = 0;
    std::vector<int> rank(edges.size(), 0);
    for (int i = 0; i < vertexesCount; ++i) {
        if (dsu.find(i) == i) {
            currentVertexNumber++;
        }
    }
    currentVertexNumber = vertexesCount - currentVertexNumber;
    for (const auto& currentEdge : edges) {
        int vertexU = currentEdge.vertexes.first;
        int vertexV = currentEdge.vertexes.second;
        if (digitLimit) {
            if (std::max(rank[vertexU], rank[vertexV]) >= MAX_VERTEX_DEG) {
                continue;
            }
        }
        if (dsu.unite(vertexU, vertexV)) {
            if (digitLimit) {
                rank[vertexU]++; rank[vertexV]++;
            }
            MSTweight += currentEdge.weight;
            currentVertexNumber++;
            if (currentVertexNumber == vertexesCount - 1) {
                return MSTweight;
            }
        }
    }
    if (currentVertexNumber != vertexesCount - 1) {
        return std::numeric_limits<int>::max();
    }
    return MSTweight;
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
    int vertexesCount
) {
    #if SHOW_INFO
    infoToLogFile(
        std::string("Вызвана рекурсивная функция findMSTRecursive\n") +
        "Вес лучшего решения: " + bestWeightString(bestWeight) +
        "\nВес текущего решения: " + std::to_string(currentWeight) +
        "\nРебра в текущем решении:\n" + edgesToString(currentEdges)
    );
    #endif
    if (currentEdges.size() == vertexesCount - 1) {
        #if SHOW_INFO
        infoToLogFile("Текущее решение содержит " +
            std::to_string(vertexesCount - 1) +
            " рёбер, оно окончательно для графа из " +
            std::to_string(vertexesCount) +
            " вершин.\nПроверим лучше ли оно предыдущего:"
        );
        #endif
        if (currentWeight < bestWeight) {
            #if SHOW_INFO
            infoToLogFile("Да, currentWeight = " +
                std::to_string(currentWeight) +
                ", а bestWeight = " +
                bestWeightString(bestWeight) +
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
    int bound = kruskalLowerBound(remaining, dsu, vertexesCount);
    if (bound == std::numeric_limits<int>::max()) {
        #if SHOW_INFO
        infoToLogFile(
            std::string("Оценка МОД на оставшихся вершинах ") +
            "методом Крускала показала, \nчто построить МОД " +
            "с текущими вершинами невозможно, \nобрезую эту ветвь рекурсии."
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
            "\n" + std::to_string(currentWeight) +
            " + " + std::to_string(bound) +
            " >= " + std::to_string(bestWeight) +
            "\nТекущее решение + нижняя оценка МОД" +
            "на оставшихся вершинах больше или равна лучшему решению \n" +
            "дальше эту ветвь продолжать не имеет смысла, обрезаю ее."
        );
        #endif
        return;
    }
    auto& currentEdge = edges[index];
    int vertexU = currentEdge.vertexes.first;
    int vertexV = currentEdge.vertexes.second;
    #if SHOW_INFO
    infoToLogFile(
        std::string("Рассмотрим ребро ") +
        vertexesToStringEdge(vertexU, vertexV)
    );
    #endif
    if (degree[vertexU] < MAX_VERTEX_DEG && degree[vertexV] < MAX_VERTEX_DEG) {
        DSU dsuCopy = dsu;
        if (dsuCopy.unite(vertexU, vertexV)) {
            degree[vertexU]++;
            degree[vertexV]++;
            currentEdges.push_back(currentEdge);
            #if SHOW_INFO
            infoToLogFile(
                std::string("Добавляем ребро ") +
                vertexesToStringEdge(vertexU, vertexV) +
                " с весом " +
                std::to_string(currentEdge.weight) +
                " в текущее решение."
            );
            #endif
            findMSTRecursive(
                edges, index + 1,
                degree, dsuCopy,
                currentWeight + currentEdge.weight,
                bestWeight,
                currentEdges, bestEdges,
                vertexesCount
            );
            #if SHOW_INFO
            infoToLogFile(
                std::string("Удаляем ребро ") +
                vertexesToStringEdge(vertexU, vertexV) +
                " с весом " +
                std::to_string(currentEdge.weight) +
                " из текущего решения"
            );
            #endif
            currentEdges.pop_back();
            degree[vertexU]--;
            degree[vertexV]--;
        }
        #if SHOW_INFO
        else {
            infoToLogFile(
                std::string("Ребро ") +
                vertexesToStringEdge(vertexU, vertexV) +
                " образует цикл, что недопустимо." +
                "\nПерехожу к рассмотрению следующего ребра."
            );
        }
        #endif
    }
    #if SHOW_INFO
    else {
        infoToLogFile(
            std::string("Ребро ") +
            vertexesToStringEdge(vertexU, vertexV) +
            " нарушает правило ограничения на степень вершины." +
            "\nСтепень вершины " +
            ALPHABET[vertexU] + ": " +
            std::to_string(degree[vertexU]) +
            "\nСтепень вершины " +
            ALPHABET[vertexV] + ": " +
            std::to_string(degree[vertexV]) +
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
        vertexesCount
    );
    #if SHOW_INFO
    infoToLogFile(
        std::string("Рассмотрены все ветви рекурсии для ребра ")  +
        vertexesToStringEdge(vertexU, vertexV) +
        ". Функция нашла МОД с весом: " +
        bestWeightString(bestWeight) + "\n"
    );
    #endif
}

int findMSTBranchAndBound() {
    matrix M = getMatrixFromStream();
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
        [](const edge& a, const edge& b) {
            return a.weight < b.weight;
        });
    int vertexesCount = M.size();
    std::vector<int> degree(vertexesCount, 0);
    DSU dsu(vertexesCount);
    std::cout << kruskalLowerBound(edges, dsu, vertexesCount, true) << '\n';
    int bestWeight = std::numeric_limits<int>::max();
    edgesStack currentEdges, bestEdges;
    findMSTRecursive(
        edges, 0,
        degree, dsu,
        0,
        bestWeight,
        currentEdges,
        bestEdges,
        vertexesCount
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