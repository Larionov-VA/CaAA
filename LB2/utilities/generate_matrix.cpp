#include <iostream>
#include <vector>
#include <random>
#include <ctime>

#define MAX_NUMBER_IN_MATRIX 100
static int counter = 0;

int getRandomNumber(int mod) {
    std::mt19937 engine;
    engine.seed(std::time(nullptr) + ++counter);
    return engine() % mod;
}

void saveMatrix(const std::vector<std::vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (int val : row) {
            std::cout << val << ' ';
        }
        std::cout << '\n';
    }
}

int main() {
    // для получения значений от 5 до 15 включениено
    int squareMatrixSide = 5 + getRandomNumber(11);
    std::vector<std::vector<int>> matrix(squareMatrixSide, std::vector<int>(squareMatrixSide));
    // цикл по строкам матрицы
    for (int i = 0; i < squareMatrixSide; ++i) {
        // цикл по содержимому строки
        for (int j = i; j < squareMatrixSide; ++j) {
            int randomNumber = 0;
            if (i != j) {
                randomNumber = getRandomNumber(MAX_NUMBER_IN_MATRIX);
            }
            matrix[i][j] = randomNumber;
            matrix[j][i] = randomNumber;
        }
    }
    saveMatrix(matrix);
    return EXIT_SUCCESS;
}