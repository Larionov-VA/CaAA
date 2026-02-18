// Возможно стоит убрать исключения
#include <stdexcept>

#include <iostream>
#include <string>
#include <bitset>

#ifndef STEPIK
#define STEPIK 1
#endif


struct Point {
    size_t x;
    size_t y;
};


struct Square {
    int currentNumberOfParts;
    int minimalNumberOfParts;
    size_t squareSide;
    u_int64_t* binaryGridOfSquare;
};


bool isPointCorrect(size_t gridSide, size_t squareSide, Point squareRigthUpCoordinates) {
    return squareRigthUpCoordinates.x + squareSide <= gridSide &&
           squareRigthUpCoordinates.y + squareSide <= gridSide;
}


void printLineAsBinary(u_int64_t line, size_t gridSide) {
    std::string s = std::bitset<64>(line).to_string();
    std::cout << s.substr(64 - gridSide) << std::endl;
}


u_int64_t* buildSquareMask(size_t gridSide, size_t squareSide, Point squareRigthUpCoordinates) {
    if (!isPointCorrect(gridSide, squareSide, squareRigthUpCoordinates)) {
        throw std::runtime_error(
            "Квадрат с координатами правого верхнего угла x = " + std::to_string(squareRigthUpCoordinates.x) +
            ", y = " + std::to_string(squareRigthUpCoordinates.y) +
            " и стороной a = " + std::to_string(squareSide) +
            " выходит за границы сетки со стороной b = " + std::to_string(gridSide)
        );
    }
    u_int64_t* gridWithSquare = new u_int64_t[gridSide]{};
    u_int64_t lineOfSquare = 0;
    for (size_t i = 0; i < squareSide; ++i) {
        lineOfSquare |= (1ULL << (squareRigthUpCoordinates.x + i));
    }
    for (size_t i = 0; i < squareSide; ++i) {
        gridWithSquare[squareRigthUpCoordinates.y + i] |= lineOfSquare;
    }
    for (size_t i = 0; i < gridSide; ++i) {
        printLineAsBinary(gridWithSquare[i], gridSide);
    }
    return gridWithSquare;
}


int findMinimalNumberOfParts() {
    return EXIT_SUCCESS;
}


int main() {
    long long squareSide;
    std::cin >> squareSide;
    if (squareSide < 2) return EXIT_FAILURE;
    if (!STEPIK) {

    }
    buildSquareMask(static_cast<size_t>(squareSide), 1, {4,1});
    findMinimalNumberOfParts();
    return EXIT_SUCCESS;
}