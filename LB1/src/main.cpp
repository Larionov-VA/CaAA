#include <climits>
#include <iostream>
#include <string>
#include <bitset>
#include <vector>

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
    u_int64_t* binGrid;
    u_int64_t* fullGridMask;
    std::vector<Square> currentSolution;
    std::vector<Square> bestSolution;
};


bool fitsInsideGrid(size_t gridSide, size_t subSquareSide, Position pos) {
    return pos.x + subSquareSide <= gridSide && pos.y + subSquareSide <= gridSide;
}


void printRowBits(u_int64_t line, size_t gridSide) {
    std::string s = std::bitset<64>(line).to_string();
    std::cout << s.substr(64 - gridSide) << std::endl;
}


bool canPlace(GridState& s, size_t side, Position pos) {
    if (!fitsInsideGrid(s.gridSide, side, pos)) {
        return false;
    }
    u_int64_t line = ((1ULL << side) - 1) << pos.x;
    for (size_t i = 0; i < side; ++i) {
        if (s.binGrid[pos.y + i] & line) {
            return false;
        }
    }
    return true;
}


void place(GridState& s, size_t side, Position pos) {
    u_int64_t line = ((1ULL << side) - 1) << pos.x;
    for (size_t i = 0; i < side; ++i)
        s.binGrid[pos.y + i] |= line;
}


void remove(GridState& s, size_t side, Position pos) {
    u_int64_t line = ((1ULL << side) - 1) << pos.x;
    for (size_t i = 0; i < side; ++i)
        s.binGrid[pos.y + i] ^= line;
}


Position getFirstPosToNextSquare(GridState& square) {
    for (size_t y = 0; y < square.gridSide; ++y) {
        if ((square.binGrid[y] & square.fullGridMask[y]) == square.fullGridMask[y]) {
            continue;
        }
        for (size_t x = 0; x < square.gridSide; ++x) {
            if (!(square.binGrid[y] & (1ULL << x))) {
                return {x, y};
            }
        }
    }
    return {0, 0};
}


bool isGridFull(GridState& square) {
    for (size_t i = 0; i < square.gridSide; ++i) {
        if ((square.binGrid[i] & square.fullGridMask[i]) != square.fullGridMask[i]) {
            return false;
        }
    }
    return true;
}


size_t getMaxPossibleSide(GridState& s, Position pos) {
    size_t maxSide = 0;
    while (pos.x + maxSide < s.gridSide && pos.y + maxSide < s.gridSide) {
        for (size_t i = 0; i <= maxSide; ++i) {
            if (s.binGrid[pos.y + maxSide] & (1ULL << (pos.x + i))) {
                return maxSide;
            }
            if (s.binGrid[pos.y + i] & (1ULL << (pos.x + maxSide))) {
                return maxSide;
            }
        }
        ++maxSide;
    }
    return maxSide;
}


void findMinimalNumberOfPartsRecursive(GridState& square) {
    if (square.gridSide % 2 == 0 && square.sumOfSquaresAreaInGrid == 0) {
        square.bestPartsCount = 4;
        square.bestSolution.push_back({{1,1}, square.gridSide / 2});
        square.bestSolution.push_back({{square.gridSide / 2 + 1,1}, square.gridSide / 2});
        square.bestSolution.push_back({{1, square.gridSide / 2 + 1}, square.gridSide / 2});
        square.bestSolution.push_back({{square.gridSide / 2 + 1,square.gridSide / 2 + 1}, square.gridSide / 2});
        return;
    }
    if (square.currentPartsCount >= square.bestPartsCount)
        return;
    if (isGridFull(square)) {
        square.bestPartsCount = square.currentPartsCount;
        square.bestSolution = square.currentSolution;
        return;
    }
    Position curPosForNextSquare = getFirstPosToNextSquare(square);
    size_t maxCurrentSide = getMaxPossibleSide(square, curPosForNextSquare);
    while (maxCurrentSide) {
        if (canPlace(square, maxCurrentSide, curPosForNextSquare)) {
            place(square, maxCurrentSide, curPosForNextSquare);
            square.currentSolution.push_back({{curPosForNextSquare.x + 1, curPosForNextSquare.y + 1}, maxCurrentSide });
            size_t currentSquareArea = maxCurrentSide * maxCurrentSide;
            square.sumOfSquaresAreaInGrid += currentSquareArea;
            ++square.currentPartsCount;
            findMinimalNumberOfPartsRecursive(square);
            remove(square, maxCurrentSide, curPosForNextSquare);
            square.currentSolution.pop_back();
            square.sumOfSquaresAreaInGrid -= currentSquareArea;
            --square.currentPartsCount;
        }
        --maxCurrentSide;
    }
}


GridState getEmptyGridState(size_t correctSquareSide) {
    u_int64_t* emptyGrid = new u_int64_t[correctSquareSide]{};
    u_int64_t* fullMask = new u_int64_t[correctSquareSide];
    u_int64_t fullLine = (1ULL << correctSquareSide) - 1;
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
    size_t correctSquareSide;
    std::cin >> squareSide;
    if (squareSide < 2 || squareSide > 63) {
        return EXIT_FAILURE;
    }
    else {
        correctSquareSide = static_cast<size_t>(squareSide);
    }
    GridState squareInfo = getEmptyGridState(correctSquareSide);
    if (!STEPIK) {
        int numberOfSquares;
        std::cin >> numberOfSquares;
        Position leftUpCoordOfSquare;
        long long currentSuareSide;
        for (int i = 0; i < numberOfSquares; ++i) {
            std::cin >> leftUpCoordOfSquare.x >> leftUpCoordOfSquare.y >> currentSuareSide;
            if (currentSuareSide < 0) {
                return EXIT_FAILURE;
            }
            Position pos = {leftUpCoordOfSquare.x, leftUpCoordOfSquare.y};
            place(squareInfo, static_cast<size_t>(currentSuareSide), pos);
        }
    }
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