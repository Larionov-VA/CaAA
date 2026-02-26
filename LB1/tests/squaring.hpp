#ifndef SQUARING_HPP
#define SQUARING_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

#ifndef COMPLEXITY_ANALYSIS
    #define COMPLEXITY_ANALYSIS 0
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
    uint64_t* binGrid;
    uint64_t fullLineMask;

    #if COMPLEXITY_ANALYSIS
        uint64_t calls;
        uint64_t canPlaceChecks;
    #endif

    std::vector<Square> currentSolution;
    std::vector<Square> bestSolution;
};

bool fitsInsideGrid(size_t gridSide, size_t squareSide, Position leftUpCoord);
bool canPlace(GridState& state, size_t squareSide, Position leftUpCoord);
uint64_t getLineMask(size_t squareSide, size_t xCoord);
void place(GridState& state, uint64_t lineMask, size_t squareSide, size_t y);
void remove(GridState& state, uint64_t lineMask, size_t squareSide, size_t y);
Position getFirstPosToNextSquare(GridState& state);
bool isGridFull(GridState& state);
size_t getMaxSquareAtPosition(GridState& state, Position leftUpCoord);
void findMinimalNumberOfPartsRecursive(GridState& state);
GridState getEmptyGridState(size_t correctSquareSide);
bool isPrime(size_t N);
void prefillGridForKnownPatterns(GridState& state);
size_t reduceToPrimeBase(size_t& N);
void getOptimisation(GridState& state, size_t& N, size_t& scale);

#endif