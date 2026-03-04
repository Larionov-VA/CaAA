#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <fstream>

#ifndef OUTPUT_FILENAME
    #define OUTPUT_FILENAME "./utilities/matrices/example.txt"
#endif
#ifndef MAX_NUMBER_IN_MATRIX
    #define MAX_NUMBER_IN_MATRIX 100
#endif

using matrix = std::vector<std::vector<int>>;

int getRandomNumber(int mod) {
    static int counter = 0;
    std::mt19937 engine;
    engine.seed(std::time(nullptr) + ++counter);
    return engine() % mod;
}

void saveMatrix(const matrix& M) {
    std::ofstream out(OUTPUT_FILENAME);
    out << M.size() << '\n';
    for (const auto& row : M) {
        for (int val : row) {
            out << val << ' ';
        }
        out << '\n';
    }
}

int main() {
    // для получения значений от 5 до 15 включениено
    int squareMatrixSide = 5 + getRandomNumber(11);
    matrix M(squareMatrixSide, std::vector<int>(squareMatrixSide));
    // цикл по строкам матрицы
    for (int i = 0; i < squareMatrixSide; ++i) {
        // цикл по содержимому строки
        for (int j = i; j < squareMatrixSide; ++j) {
            int randomNumber = 0;
            if (i != j) {
                randomNumber = getRandomNumber(MAX_NUMBER_IN_MATRIX);
            }
            M[i][j] = randomNumber;
            M[j][i] = randomNumber;
        }
    }
    saveMatrix(M);
    return EXIT_SUCCESS;
}