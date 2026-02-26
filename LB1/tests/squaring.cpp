#include "squaring.hpp"

#include <climits>
#include <algorithm>


bool fitsInsideGrid(size_t gridSide, size_t squareSide, Position leftUpCoord) {
    return leftUpCoord.x + squareSide <= gridSide &&
           leftUpCoord.y + squareSide <= gridSide;
}


bool canPlace(GridState& state, size_t squareSide, Position leftUpCoord) {
    #if COMPLEXITY_ANALYSIS
        ++state.canPlaceChecks;
    #endif

    if (!fitsInsideGrid(state.gridSide, squareSide, leftUpCoord)) {
        return false;
    }

    uint64_t line = ((1ULL << squareSide) - 1) << leftUpCoord.x;

    for (size_t i = 0; i < squareSide; ++i) {
        if (state.binGrid[leftUpCoord.y + i] & line) {
            return false;
        }
    }
    return true;
}

uint64_t getLineMask(size_t squareSide, size_t xCoord) {
    return ((1ULL << squareSide) - 1) << xCoord;
}

void place(GridState& state, uint64_t lineMask, size_t squareSide, size_t y) {
    for (size_t i = 0; i < squareSide; ++i) {
        state.binGrid[y + i] |= lineMask;
    }
}

void remove(GridState& state, uint64_t lineMask, size_t squareSide, size_t y) {
    for (size_t i = 0; i < squareSide; ++i) {
        state.binGrid[y + i] &= ~lineMask;
    }
}

Position getFirstPosToNextSquare(GridState& state) {
    for (size_t y = 0; y < state.gridSide; ++y) {
        uint64_t free = ~state.binGrid[y] & state.fullLineMask;
        if (free) {
            size_t x = __builtin_ctzll(free);
            return {x, y};
        }
    }
    return {0, 0};
}

bool isGridFull(GridState& state) {
    for (size_t i = 0; i < state.gridSide; ++i) {
        if ((state.binGrid[i] & state.fullLineMask) != state.fullLineMask) {
            return false;
        }
    }
    return true;
}

size_t getMaxSquareAtPosition(GridState& state, Position leftUpCoord) {
    size_t dx = state.gridSide - leftUpCoord.x;
    size_t dy = state.gridSide - leftUpCoord.y;
    size_t maxPossibleSquareSide = std::min(dx, dy);
    if (maxPossibleSquareSide > state.gridSide - 1) {
        maxPossibleSquareSide = state.gridSide - 1;
    }
    for (size_t side = maxPossibleSquareSide; side > 0; --side) {
        if (canPlace(state, side, leftUpCoord)) {
            return side;
        }
    }
    return 0;
}


void findMinimalNumberOfPartsRecursive(GridState& state) {
    #if COMPLEXITY_ANALYSIS
        ++state.calls;
    #endif
    if (state.currentPartsCount >= state.bestPartsCount) {
        return;
    }
    if (isGridFull(state)) {
        state.bestPartsCount = state.currentPartsCount;
        state.bestSolution = state.currentSolution;
        return;
    }
    Position leftUpCoord = getFirstPosToNextSquare(state);
    size_t maxSide = getMaxSquareAtPosition(state, leftUpCoord);
    for (size_t side = maxSide; side > 0; --side) {
        uint64_t currentLineMask = getLineMask(side, leftUpCoord.x);
        place(state, currentLineMask, side, leftUpCoord.y);
        state.currentSolution.push_back(
            {{leftUpCoord.x + 1, leftUpCoord.y + 1}, side}
        );
        ++state.currentPartsCount;
        findMinimalNumberOfPartsRecursive(state);
        remove(state, currentLineMask, side, leftUpCoord.y);
        state.currentSolution.pop_back();
        --state.currentPartsCount;
    }
}


GridState getEmptyGridState(size_t correctSquareSide) {
    uint64_t* emptyGrid = new uint64_t[correctSquareSide]{};
    uint64_t fullLine = (1ULL << correctSquareSide) - 1;
    return {
        0,
        INT_MAX,
        correctSquareSide,
        emptyGrid,
        fullLine,
        #if COMPLEXITY_ANALYSIS
            0,
            0,
        #endif
        {},
        {}
    };
}

bool isPrime(size_t N) {
    if (N <= 1) return false;
    if (N == 2 || N == 3) return true;
    if (N % 2 == 0 || N % 3 == 0) return false;
    for (size_t i = 5; i * i <= N; i += 6) {
        if (N % i == 0 || N % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

void prefillGridForKnownPatterns(GridState& state) {
    size_t N = state.gridSide;
    if (N % 2 == 0) {
        size_t half = N / 2;
        size_t coords[2] = {0, half};
        for (size_t y0 : coords) {
            for (size_t x0 : coords) {
                place(state, getLineMask(half, x0), half, y0);
                state.currentSolution.push_back({{x0 + 1, y0 + 1}, half});
                ++state.currentPartsCount;
            }
        }
    }
    else if (N % 3 == 0) {
        size_t k = N / 3;
        size_t k2 = 2 * k;
        size_t X[6] = {0, k2, k2, 0, k, k2};
        size_t Y[6] = {0, 0, k, k2, k2, k2};
        size_t S[6] = {k2, k, k, k, k, k};
        for (int i = 0; i < 6; ++i) {
            place(state, getLineMask(S[i], X[i]), S[i], Y[i]);
            state.currentSolution.push_back({{X[i] + 1, Y[i] + 1}, S[i]});
            ++state.currentPartsCount;
        }
    }
    else if (isPrime(N)) {
        size_t s1 = N / 2 + 1;
        size_t s2 = N / 2;
        place(state, getLineMask(s1, 0), s1, 0);
        state.currentSolution.push_back({{1, 1}, s1});
        ++state.currentPartsCount;
        place(state, getLineMask(s2, 0), s2, s2 + 1);
        state.currentSolution.push_back({{1, s2 + 2}, s2});
        ++state.currentPartsCount;
        place(state, getLineMask(s2, s2 + 1), s2, 0);
        state.currentSolution.push_back({{s2 + 2, 1}, s2});
        ++state.currentPartsCount;
    }
}

size_t reduceToPrimeBase(size_t& N) {
    for (size_t p = 2; p * p <= N; ++p) {
        if (N % p == 0 && isPrime(p)) {
            size_t scale = N / p;
            N = p;
            return scale;
        }
    }
    return 1;
}

void getOptimisation(GridState& state, size_t& N, size_t& scale) {
    if (state.currentPartsCount == 0) {
        scale = reduceToPrimeBase(N);
        if (scale > 1) {
            state = getEmptyGridState(N);
        }
        prefillGridForKnownPatterns(state);
    }
}