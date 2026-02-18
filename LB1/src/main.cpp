// Возможно стоит убрать исключения
#include <stdexcept>

#include <climits>
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


void addSquare(Square& sourceSquare, u_int64_t* destinationSquare, size_t squareSide) {
    for (int i = 0; i < squareSide; ++i) {
        sourceSquare.binaryGridOfSquare[i] |= destinationSquare[i];
    }
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
    return gridWithSquare;
}


int findMinimalNumberOfParts() {
    return EXIT_SUCCESS;
}


int main() {
    long long squareSide;
    std::cin >> squareSide;
    if (squareSide < 2) return EXIT_FAILURE;
    Square squareInfo = {
        0,
        INT_MAX,
        static_cast<size_t>(squareSide),
        buildSquareMask(static_cast<size_t>(squareSide), 0, {0,0})};
    if (!STEPIK) {
        int numberOfSquares;
        std::cin >> numberOfSquares;
        Point p;
        long long currentSuareSide;
        for (int i = 0; i < numberOfSquares; ++i) {
            std::cin >> p.x >> p.y >> currentSuareSide;
            if (currentSuareSide < 0) return EXIT_FAILURE;
            addSquare(
                squareInfo,
                buildSquareMask(
                    static_cast<size_t>(squareSide),
                    static_cast<size_t>(currentSuareSide),
                    {p.x,p.y}
                ),
                squareSide
            );
            for (size_t i = 0; i < static_cast<size_t>(squareSide); ++i) {
                printLineAsBinary(squareInfo.binaryGridOfSquare[i], squareSide);
            }
        }
    }

    // findMinimalNumberOfParts();
    return EXIT_SUCCESS;
}