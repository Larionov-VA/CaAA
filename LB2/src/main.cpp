#include <iostream>
#include <vector>


using matrix = std::vector<std::vector<int>>;

matrix getMatrix() {
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

void printMatrix(const matrix& M) {
    for (const auto& row : M) {
        for (int val : row) {
            std::cout << val << ' ';
        }
        std::cout << '\n';
    }
}

int main() {
    matrix M = getMatrix();
    printMatrix(M);
    return EXIT_SUCCESS;
}