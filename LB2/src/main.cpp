#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#define GRAPH_FILENAME "./files/graph.dot"
#define MST_FILENAME "./files/MST.dot"

#ifndef MAXIMUM_VERTEX
    #define MAXIMUM_VERTEX 5
#endif

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
                        << " [label=\"" << M[i][j] << "\"];\n";
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

bool findMST(matrix& M) {

    return true;
}

int main() {
    matrix M = getMatrixFromFile();
    printMatrix(M);
    matrixToDot(M, GRAPH_FILENAME);

    matrixToDot(M, MST_FILENAME);
    return EXIT_SUCCESS;
}