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


const std::string ALPHABET = \
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";


/**
 * Структура ребра графа.
 * Содержит пару вершин (индексы) и вес.
 * Определён оператор == для поиска в векторе.
 */
struct edge {
    std::pair<int, int> vertices;
    int weight;
    bool operator==(const edge& counter) const {
        return vertices == counter.vertices && weight == counter.weight;
    }
};


using edgesStack = std::vector<edge>;
using matrix = std::vector<std::vector<int>>;


#if SHOW_INFO
void infoToLogFile(const std::string& info) {
    if (logFile) {
        logFile << logCounter++ << "\n";
        std::stringstream ss(info);
        std::string token;
        while (std::getline(ss, token, '\n')) {
            logFile << "\t" << token << std::endl;
        }
    }
}


/**
 * Преобразование матрицы весов в строку с буквенными обозначениями вершин.
 */
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


/**
 * Возвращает строку вида "A -- B" для ребра между вершинами u и v.
 */
std::string verticesToStringEdge(const int vertexU, const int vertexV) {
    std::stringstream ss;
    ss << ALPHABET[vertexU] << " -- " << ALPHABET[vertexV];
    return ss.str();
}


/**
 * Формирует многострочное текстовое представление списка рёбер.
 */
std::string edgesToString(const edgesStack& edges) {
    std::string strCurrentEdges;
    for (const edge& edge : edges) {
        int vertexU = edge.vertices.first;
        int vertexV = edge.vertices.second;
        strCurrentEdges += verticesToStringEdge(vertexU, vertexV);
        strCurrentEdges += "\n";
    }
    return strCurrentEdges;
}


/**
 * Возвращает строковое представление веса: число либо "inf".
 */
std::string bestWeightString(const int& bestWeight) {
    return bestWeight == std::numeric_limits<int>::max() ?
    "inf" : std::to_string(bestWeight);
}
#endif


/**
 * Система непересекающихся множеств (DSU) с эвристиками сжатия путей и объединения по рангу.
 * Используется в алгоритме Краскала для проверки циклов и объединения компонент связности.
 */
struct DSU {
    std::vector<int> sets, rank;
    DSU(int setsCount) : sets(setsCount), rank(setsCount, 0) {
        for (int i = 0; i < setsCount; ++i)
            sets[i] = i;
    }
    /**
     * Находит представителя множества для элемента indexInSet.
     * Применяется сжатие путей.
     */
    int find(int indexInSet) {
        if (indexInSet == sets[indexInSet]) {
            return indexInSet;
        }
        return sets[indexInSet] = find(sets[indexInSet]);
    }
    /**
     * Объединяет множества, содержащие setA и setB.
     * Возвращает true, если объединение выполнено (элементы были в разных множествах),
     * иначе false.
     */
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


/**
 * Считывает матрицу весов из стандартного ввода.
 * Формат: первое число – количество вершин N,
 * затем N строк по N чисел (веса рёбер).
 */
matrix getMatrixFromStream() {
    int verticesCount;
    std::cin >> verticesCount;
    matrix M(verticesCount, std::vector<int>(verticesCount));
    for (int i = 0; i < verticesCount; ++i) {
        for (int j = 0; j < verticesCount; ++j) {
            std::cin >> M[i][j];
        }
    }
    return M;
}


/**
 * Проверяет, содержится ли заданное ребро в списке (сравнение по ==).
 */
bool isEdgeIn(const edgesStack& edges, const edge& edge) {
    return std::find(edges.begin(), edges.end(), edge) == edges.end();
}


/**
 * Генерирует файл в формате Graphviz DOT, отображающий исходный граф
 * и выделяющий рёбра МОД зеленым цветом.
 */
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
        const int& vertexU = currentEdge.vertices.first;
        const int& vertexV = currentEdge.vertices.second;
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


/**
 * Проверяет симметричность матрицы.
 */
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


/**
 * Преобразует матрицу смежности в список рёбер.
 * Включаются только рёбра с ненулевым весом (i < j, чтобы избежать дублирования).
 */
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


/**
 * Сортирует список рёбер по возрастанию веса.
 */
void sortEdgesStack(edgesStack& edges) {
    auto comporator = [](const edge& a, const edge& b) {
        return a.weight < b.weight;
    };
    std::sort(edges.begin(), edges.end(), comporator);
}


/**
 * Вычисляет вес минимального остовного дерева (МОД) на основе алгоритма Краскала.
 * Используется как для получения приближённого решения (с ограничением степени),
 * так и для вычисления нижней оценки в методе ветвей и границ (без ограничения степени).
 *
 * Параметры:
 *   edges         -- отсортированный по весу список доступных рёбер.
 *   dsu           -- текущее состояние компонент связности (копия, модифицируется).
 *   verticesCount -- общее число вершин графа.
 *   digitLimit    -- если true, учитывается ограничение MAX_VERTEX_DEG на степень вершин.
 *   mstEdges      -- если не nullptr, в него записываются рёбра построенного МОД.
 *
 * Возвращает вес построенного дерева либо std::numeric_limits<int>::max(),
 * если построить дерево невозможно (не хватает рёбер или нарушены ограничения).
 */
int kruskalLowerBound(
    edgesStack& edges,
    DSU dsu,
    int verticesCount,
    bool digitLimit = false,
    edgesStack* mstEdges = nullptr
) {
    int MSTweight = 0;
    int currentVertexNumber = 0;
    std::vector<int> rank(edges.size(), 0);
    for (int i = 0; i < verticesCount; ++i) {
        if (dsu.find(i) == i) {
            currentVertexNumber++;
        }
    }
    currentVertexNumber = verticesCount - currentVertexNumber;
    for (const auto& currentEdge : edges) {
        int vertexU = currentEdge.vertices.first;
        int vertexV = currentEdge.vertices.second;
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
            if (mstEdges != nullptr) {
                mstEdges->push_back(currentEdge);
            }
            if (currentVertexNumber == verticesCount - 1) {
                return MSTweight;
            }
        }
    }
    if (currentVertexNumber != verticesCount - 1) {
        return std::numeric_limits<int>::max();
    }
    return MSTweight;
}


/**
 * Рекурсивная функция поиска точного решения методом ветвей и границ.
 * Перебирает рёбра в порядке сортировки, формируя допустимые остовные деревья
 * с ограничением степени MAX_VERTEX_DEG.
 *
 * Параметры:
 *   edges         -- полный список рёбер (отсортирован).
 *   index         -- текущий индекс рассматриваемого ребра.
 *   degree        -- текущие степени вершин в частичном решении.
 *   dsu           -- состояние компонент связности для частичного решения (копия).
 *   currentWeight -- суммарный вес рёбер в частичном решении.
 *   bestWeight    -- вес лучшего найденного на данный момент решения (изменяется при улучшении).
 *   currentEdges  -- рёбра текущего частичного решения.
 *   bestEdges     -- рёбра лучшего решения (обновляется вместе с bestWeight).
 *   verticesCount -- общее число вершин.
 */
void findMSTRecursive(
    edgesStack& edges,
    int index,
    std::vector<int>& degree,
    DSU dsu,
    int currentWeight,
    int& bestWeight,
    edgesStack& currentEdges,
    edgesStack& bestEdges,
    int verticesCount
) {
    #if SHOW_INFO
    std::string info("Вызвана рекурсивная функция findMSTRecursive\n");
    info += "Вес лучшего решения: ";
    info += bestWeightString(bestWeight);
    info += "\nВес текущего решения: ";
    info += std::to_string(currentWeight);
    info += "\nРебра в текущем решении:\n";
    info += edgesToString(currentEdges);
    infoToLogFile(info);
    #endif
    // Если собрано V-1 рёбер, проверяем на улучшение рекорда
    if (currentEdges.size() == verticesCount - 1) {
        #if SHOW_INFO
        info.clear();
        info += "Текущее решение содержит ";
        info += std::to_string(verticesCount - 1);
        info += " рёбер, оно окончательно для графа из ";
        info += std::to_string(verticesCount);
        info += " вершин.\nПроверим лучше ли оно предыдущего:";
        infoToLogFile(info);
        #endif
        if (currentWeight < bestWeight) {
            #if SHOW_INFO
            info.clear();
            info += "Да, currentWeight = ";
            info += std::to_string(currentWeight);
            info += ", а bestWeight = ";
            info += bestWeightString(bestWeight);
            info += ". Заменяем лучшее решение текущим.";
            infoToLogFile(info);
            #endif
            bestWeight = currentWeight;
            bestEdges = currentEdges;
        }
        #if SHOW_INFO
        else {
            info.clear();
            info += "Нет, currentWeight = ";
            info += std::to_string(currentWeight);
            info += ", а bestWeight = ";
            info += std::to_string(bestWeight);
            info += ". Оставляем все как есть.";
            infoToLogFile(info);
        }
        #endif
        return;
    }
    if (index >= edges.size()) {
        #if SHOW_INFO
        infoToLogFile("Index out of range, отсекаю ветвь рекурсии");
        #endif
        return; // рёбра кончились, дерево не достроено
    }
    edgesStack remaining(edges.begin() + index, edges.end());
    int bound = kruskalLowerBound(remaining, dsu, verticesCount);
    if (bound == std::numeric_limits<int>::max()) {
        #if SHOW_INFO
        info.clear();
        info += "Оценка МОД на оставшихся вершинах методом Крускала показала,";
        info += "\nчто построить МОД с текущими вершинами невозможно,";
        info += "\nотсекаю эту ветвь рекурсии.";
        infoToLogFile(info);
        #endif
        return; // невозможно достроить до остова
    }
    if (currentWeight + bound >= bestWeight) {
        #if SHOW_INFO
        info.clear();
        info += "currentWeight = ";
        info += std::to_string(currentWeight);
        info += "\tbound = ";
        info += std::to_string(bound);
        info += "\tbestWeight = ";
        info += std::to_string(bestWeight);
        info += "\n";
        info += std::to_string(currentWeight);
        info += " + ";
        info += std::to_string(bound);
        info += " >= ";
        info += std::to_string(bestWeight);
        info += "\nТекущее решение + нижняя оценка МОД на оставшихся вершинах ";
        info += "больше или равна лучшему решению";
        info += "\nдальше эту ветвь продолжать не имеет смысла, отсекаю ее.";
        infoToLogFile(info);
        #endif
        return; // отсечение по верхней границе
    }
    auto& currentEdge = edges[index];
    int vertexU = currentEdge.vertices.first;
    int vertexV = currentEdge.vertices.second;
    #if SHOW_INFO
    info.clear();
    info += "Рассмотрим ребро ";
    info += verticesToStringEdge(vertexU, vertexV);
    infoToLogFile(info);
    #endif
    // Попытка включить ребро в решение
    if (degree[vertexU] < MAX_VERTEX_DEG && degree[vertexV] < MAX_VERTEX_DEG) {
        DSU dsuCopy = dsu;
        if (dsuCopy.unite(vertexU, vertexV)) {
            degree[vertexU]++;
            degree[vertexV]++;
            currentEdges.push_back(currentEdge);

            #if SHOW_INFO
            info.clear();
            info += "Добавляем ребро ";
            info += verticesToStringEdge(vertexU, vertexV);
            info += " с весом ";
            info += std::to_string(currentEdge.weight);
            info += " в текущее решение.";
            infoToLogFile(info);
            #endif
            findMSTRecursive(
                edges, index + 1,
                degree, dsuCopy,
                currentWeight + currentEdge.weight,
                bestWeight,
                currentEdges, bestEdges,
                verticesCount
            );
            #if SHOW_INFO
            info.clear();
            info += "Удаляем ребро ";
            info += verticesToStringEdge(vertexU, vertexV);
            info += " с весом ";
            info += std::to_string(currentEdge.weight);
            info += " из текущего решения";
            infoToLogFile(info);
            #endif
            currentEdges.pop_back();
            degree[vertexU]--;
            degree[vertexV]--;
        }
        #if SHOW_INFO
        else {
            info.clear();
            info += "Ребро ";
            info += verticesToStringEdge(vertexU, vertexV);
            info += " образует цикл, что недопустимо.";
            info += "\nПереходим к рассмотрению следующего ребра.";
            infoToLogFile(info);
        }
        #endif
    }
    #if SHOW_INFO
    else {
        info.clear();
        info += "Ребро ";
        info += verticesToStringEdge(vertexU, vertexV);
        info += " нарушает правило ограничения на степень вершины.\n";
        info += "Степень вершины ";
        info += ALPHABET[vertexU];
        info += ": ";
        info += std::to_string(degree[vertexU]);
        info += "\nСтепень вершины ";
        info += ALPHABET[vertexV];
        info += ": ";
        info += std::to_string(degree[vertexV]);
        info += "\nПерехожу к рассмотрению следующего ребра.";
        infoToLogFile(info);
    }
    #endif
    // Ветвь без включения текущего ребра
    findMSTRecursive(
        edges, index + 1,
        degree, dsu,
        currentWeight,
        bestWeight,
        currentEdges, bestEdges,
        verticesCount
    );
    #if SHOW_INFO
    info.clear();
    info += "Рассмотрены все ветви рекурсии для ребра ";
    info += verticesToStringEdge(vertexU, vertexV);
    info += ". Функция нашла МОД с весом: ";
    info += bestWeightString(bestWeight);
    info += "\n";
    infoToLogFile(info);
    #endif
}


/**
 * Выводит в стандартный поток результаты работы алгоритмов:
 * точное решение и (если оно отличается) приближённое решение.
 */
void printResult(
    edgesStack& bestEdges,
    edgesStack& approximationEdges,
    int bestWeight,
    int approximationWeight
) {
    std::string result;
    if (bestWeight == approximationWeight) {
        result += "Приближенное и точное значения равны: ";
    }
    else {
        result += "Точное значение: ";
    }
    result += std::to_string(bestWeight);
    result += '\n';
    for (auto& currentEdge : bestEdges) {
        int vertexU = currentEdge.vertices.first;
        int vertexV = currentEdge.vertices.second;
        result += ALPHABET[vertexU];
        result += " -- ";
        result += ALPHABET[vertexV];
        result += "\t[";
        result += std::to_string(currentEdge.weight);
        result += "]\n";
    }
    if (bestWeight != approximationWeight) {
        result += "Приближенное значение: ";
        result += std::to_string(approximationWeight);
        result += '\n';
        for (auto& currentEdge : approximationEdges) {
            int vertexU = currentEdge.vertices.first;
            int vertexV = currentEdge.vertices.second;
            result += ALPHABET[vertexU];
            result += " -- ";
            result += ALPHABET[vertexV];
            result += "\t[";
            result += std::to_string(currentEdge.weight);
            result += "]\n";
        }
    }
    std::cout << result;
}


/**
 * Основная функция точного поиска МОД с ограничением степени.
 * Считывает матрицу, строит приближённое решение (Краскал с ограничением),
 * затем запускает метод ветвей и границ.
 * Выводит результат и генерирует DOT-файл.
 */
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
    sortEdgesStack(edges);
    int verticesCount = M.size();
    std::vector<int> degree(verticesCount, 0);
    DSU dsu(verticesCount);
    edgesStack currentEdges, bestEdges, approximationEdges;
    int approximation = kruskalLowerBound(
        edges,
        dsu,
        verticesCount,
        true,
        &approximationEdges
    );
    std::cout << "Приближенное значение: ";
    std::cout << approximation << '\n';
    int bestWeight = approximation;
    findMSTRecursive(
        edges, 0,
        degree, dsu,
        0,
        bestWeight,
        currentEdges,
        bestEdges,
        verticesCount
    );
    if (bestWeight == std::numeric_limits<int>::max()) {
        std::cout << "no path\n";
        return EXIT_FAILURE;
    }
    else {
        if (bestEdges.size() == 0) {
            bestEdges = approximationEdges;
        }
        printResult(bestEdges, approximationEdges, bestWeight, approximation);
        edgesToDot(edges, bestEdges, GRAPH_FILENAME);
    }
    return EXIT_SUCCESS;
}


/**
 * Точка входа.
 * Проверяет корректность макроса MAX_VERTEX_DEG и запускает алгоритм.
 */
int main() {
    if (MAX_VERTEX_DEG < 2) {
        #if SHOW_INFO
        infoToLogFile("MAX_VERTEX_DEG < 2, решение невозможно.");
        #endif
        return EXIT_FAILURE;
    }
    return findMSTBranchAndBound();
}