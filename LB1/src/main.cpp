#include <climits>
#include <iostream>
#include <vector>
#include <cstdint>

#ifndef STEPIK
#define STEPIK 1
#endif

struct Position {
    size_t x;
    size_t y;
};

struct Square {
    Position squareLeftUpCoordinates;
    size_t squareSide;
};

struct GridState {
    size_t currentPartsCount;
    size_t bestPartsCount;
    size_t gridSide;
    size_t gridArea;
    size_t sumOfSquaresAreaInGrid;
    uint64_t* binGrid;
    uint64_t* fullGridMask;
    std::vector<Square> currentSolution;
    std::vector<Square> bestSolution;
};

bool fitsInsideGrid(size_t gridSide, size_t subSquareSide, Position pos) {
    return pos.x + subSquareSide <= gridSide && pos.y + subSquareSide <= gridSide;
}

bool canPlace(GridState& s, size_t side, Position pos) {
    if (!fitsInsideGrid(s.gridSide, side, pos))
        return false;
    uint64_t line = ((1ULL << side) - 1) << pos.x;
    for (size_t i = 0; i < side; ++i)
        if (s.binGrid[pos.y + i] & line)
            return false;
    return true;
}

void place(GridState& s, size_t side, Position pos) {
    uint64_t line = ((1ULL << side) - 1) << pos.x;
    for (size_t i = 0; i < side; ++i)
        s.binGrid[pos.y + i] |= line;
}

void remove(GridState& s, size_t side, Position pos) {
    uint64_t line = ((1ULL << side) - 1) << pos.x;
    for (size_t i = 0; i < side; ++i)
        s.binGrid[pos.y + i] &= ~line;
}

Position getFirstPosToNextSquare(GridState& square) {
    for (size_t y = 0; y < square.gridSide; ++y) {
        uint64_t free = ~square.binGrid[y] & square.fullGridMask[y];
        if (free) {
            size_t x = __builtin_ctzll(free);
            return {x, y};
        }
    }
    return {0, 0};
}

bool isGridFull(GridState& square) {
    for (size_t i = 0; i < square.gridSide; ++i)
        if ((square.binGrid[i] & square.fullGridMask[i]) != square.fullGridMask[i])
            return false;
    return true;
}

size_t getMaxSquareAtPosition(GridState& square, Position pos) {
    size_t maxPossible = std::min(square.gridSide - pos.x, square.gridSide - pos.y);
    if (maxPossible > square.gridSide - 1)
        maxPossible = square.gridSide - 1;
    for (size_t side = maxPossible; side > 0; --side)
        if (canPlace(square, side, pos))
            return side;
    return 0;
}

void findMinimalNumberOfPartsRecursive(GridState& square) {
    if (square.currentPartsCount >= square.bestPartsCount)
        return;

    if (isGridFull(square)) {
        square.bestPartsCount = square.currentPartsCount;
        square.bestSolution = square.currentSolution;
        return;
    }

    Position pos = getFirstPosToNextSquare(square);
    size_t maxSide = getMaxSquareAtPosition(square, pos);

    for (size_t side = maxSide; side > 0; --side) {
        if (!canPlace(square, side, pos))
            continue;

        place(square, side, pos);
        square.currentSolution.push_back({{pos.x + 1, pos.y + 1}, side});
        square.sumOfSquaresAreaInGrid += side * side;
        ++square.currentPartsCount;

        findMinimalNumberOfPartsRecursive(square);

        remove(square, side, pos);
        square.currentSolution.pop_back();
        square.sumOfSquaresAreaInGrid -= side * side;
        --square.currentPartsCount;
    }
}

GridState getEmptyGridState(size_t correctSquareSide) {
    uint64_t* emptyGrid = new uint64_t[correctSquareSide]{};
    uint64_t* fullMask = new uint64_t[correctSquareSide];
    uint64_t fullLine = (1ULL << correctSquareSide) - 1;
    for (size_t i = 0; i < correctSquareSide; ++i)
        fullMask[i] = fullLine;
    return {
        0,
        INT_MAX,
        correctSquareSide,
        correctSquareSide * correctSquareSide,
        0,
        emptyGrid,
        fullMask,
        {},
        {}
    };
}

int main() {
    long long squareSide;
    std::cin >> squareSide;
    if (squareSide < 2 || squareSide > 63)
        return EXIT_FAILURE;

    size_t correctSquareSide = static_cast<size_t>(squareSide);
    GridState squareInfo = getEmptyGridState(correctSquareSide);

#if !STEPIK
    int numberOfSquares;
    std::cin >> numberOfSquares;
    Position leftUpCoordOfSquare;
    long long currentSuareSide;
    for (int i = 0; i < numberOfSquares; ++i) {
        std::cin >> leftUpCoordOfSquare.x >> leftUpCoordOfSquare.y >> currentSuareSide;
        Position pos = {leftUpCoordOfSquare.x - 1, leftUpCoordOfSquare.y - 1};
        place(squareInfo, static_cast<size_t>(currentSuareSide), pos);
        squareInfo.currentSolution.push_back({{leftUpCoordOfSquare.x, leftUpCoordOfSquare.y}, static_cast<size_t>(currentSuareSide)});
        squareInfo.sumOfSquaresAreaInGrid += static_cast<size_t>(currentSuareSide) * static_cast<size_t>(currentSuareSide);
        squareInfo.currentPartsCount++;
    }
#endif

    findMinimalNumberOfPartsRecursive(squareInfo);

    std::cout << squareInfo.bestPartsCount << "\n";
    for (const auto& sq : squareInfo.bestSolution) {
        std::cout << sq.squareLeftUpCoordinates.x << " "
                  << sq.squareLeftUpCoordinates.y << " "
                  << sq.squareSide << "\n";
    }

    delete[] squareInfo.binGrid;
    delete[] squareInfo.fullGridMask;
    return EXIT_SUCCESS;
}
