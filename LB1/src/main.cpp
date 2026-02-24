#include <climits>
#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>

#ifndef STEPIK
    #define STEPIK 1
#endif

#ifndef PRINTTIME
    #define PRINTTIME 0
#endif

#if PRINTTIME
    #include <cstdio>
    #include <ctime>
#endif

#ifndef PRINTINFO
    #define PRINTINFO 0
#endif

#if PRINTINFO
    #include <bitset>
    #include <string>
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
    uint64_t* fullGridMask;
    std::vector<Square> currentSolution;
    std::vector<Square> bestSolution;
};


bool fitsInsideGrid(size_t gridSide, size_t subSquareSide, Position pos) {
    return pos.x + subSquareSide <= gridSide && pos.y + subSquareSide <= gridSide;
}


bool canPlace(GridState& s, size_t side, Position pos) {
    if (!fitsInsideGrid(s.gridSide, side, pos)) {
        return false;
    }
    uint64_t line = ((1ULL << side) - 1) << pos.x;
    for (size_t i = 0; i < side; ++i) {
        if (s.binGrid[pos.y + i] & line) {
            return false;
        }
    }
    return true;
}


void place(GridState& s, size_t side, Position pos) {
    uint64_t line = ((1ULL << side) - 1) << pos.x;
    for (size_t i = 0; i < side; ++i) {
        s.binGrid[pos.y + i] |= line;
    }
}


void remove(GridState& s, size_t side, Position pos) {
    uint64_t line = ((1ULL << side) - 1) << pos.x;
    for (size_t i = 0; i < side; ++i) {
        s.binGrid[pos.y + i] &= ~line;
    }
}


Position getFirstPosToNextSquare(GridState& s) {
    for (size_t y = 0; y < s.gridSide; ++y) {
        uint64_t free = ~s.binGrid[y] & s.fullGridMask[y];
        if (free) {
            size_t x = __builtin_ctzll(free);
            return {x, y};
        }
    }
    return {0, 0};
}


bool isGridFull(GridState& s) {
    for (size_t i = 0; i < s.gridSide; ++i) {
        if ((s.binGrid[i] & s.fullGridMask[i]) != s.fullGridMask[i]) {
            return false;
        }
    }
    return true;
}


size_t getMaxSquareAtPosition(GridState& s, Position pos) {
    size_t maxPossibleSquareSide = std::min(s.gridSide - pos.x, s.gridSide - pos.y);
    if (maxPossibleSquareSide > s.gridSide - 1) {
        maxPossibleSquareSide = s.gridSide - 1;
    }
    for (size_t side = maxPossibleSquareSide; side > 0; --side) {
        if (canPlace(s, side, pos)) {
            return side;
        }
    }
    return 0;
}

#if PRINTINFO
void printGrid(GridState& s) {
    for (size_t y = 0; y < s.gridSide; ++y) {
        std::string stringBinLine = std::bitset<64>(s.binGrid[y]).to_string();
        std::reverse(stringBinLine.begin(), stringBinLine.end());
        std::cout << stringBinLine.substr(0, s.gridSide) << '\n';
    }
}
#endif

void findMinimalNumberOfPartsRecursive(GridState& s) {
    #if PRINTINFO
        printGrid(s);
    #endif
    if (s.currentPartsCount >= s.bestPartsCount) {
        return;
    }
    if (isGridFull(s)) {
        s.bestPartsCount = s.currentPartsCount;
        s.bestSolution = s.currentSolution;
        return;
    }

    Position pos = getFirstPosToNextSquare(s);
    size_t maxSide = getMaxSquareAtPosition(s, pos);
    for (size_t side = maxSide; side > 0; --side) {
        place(s, side, pos);
        s.currentSolution.push_back({{pos.x + 1, pos.y + 1}, side});
        ++s.currentPartsCount;
        findMinimalNumberOfPartsRecursive(s);
        remove(s, side, pos);
        s.currentSolution.pop_back();
        --s.currentPartsCount;
    }
}


GridState getEmptyGridState(size_t correctSquareSide) {
    uint64_t* emptyGrid = new uint64_t[correctSquareSide]{};
    uint64_t* fullMask = new uint64_t[correctSquareSide];
    uint64_t fullLine = (1ULL << correctSquareSide) - 1;
    for (size_t i = 0; i < correctSquareSide; ++i) {
        fullMask[i] = fullLine;
    }
    return {
        0,
        INT_MAX,
        correctSquareSide,
        emptyGrid,
        fullMask,
        {},
        {}
    };
}


bool isPrime(size_t n) {
    if (n <= 1) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (size_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}


void prefillGridForKnownPatterns(GridState& s) {
    size_t N = s.gridSide;
    if (N % 2 == 0) {
        size_t half = N / 2;
        size_t coords[2] = {0, half};
        for (size_t y0 : coords) {
            for (size_t x0 : coords) {
                place(s, half, {x0, y0});
                s.currentSolution.push_back({{x0 + 1, y0 + 1}, half});
                ++s.currentPartsCount;
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
            place(s, S[i], {X[i], Y[i]});
            s.currentSolution.push_back({{X[i] + 1, Y[i] + 1}, S[i]});
            ++s.currentPartsCount;
        }
    }
    else if (isPrime(N)) {
        size_t s1 = N / 2 + 1;
        size_t s2 = N / 2;
        place(s, s1, {0, 0});
        s.currentSolution.push_back({{1, 1}, s1});
        ++s.currentPartsCount;
        place(s, s2, {0, s2 + 1});
        s.currentSolution.push_back({{1, s2 + 2}, s2});
        ++s.currentPartsCount;
        place(s, s2, {s2 + 1, 0});
        s.currentSolution.push_back({{s2 + 2, 1}, s2});
        ++s.currentPartsCount;
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


#if !STEPIK
    bool individualizationFunction(GridState& state) {
        int numberOfSquares = 0;
        std::cin >> numberOfSquares;
        Position leftUpCoordOfSquare;
        long long currentSuareSide;
        for (int i = 0; i < numberOfSquares; ++i) {
            std::cin >> leftUpCoordOfSquare.x >> leftUpCoordOfSquare.y >> currentSuareSide;
            if (currentSuareSide < 1 || currentSuareSide > state.gridSide) {
                return false;
            }
            Position currentPosition = {leftUpCoordOfSquare.x - 1, leftUpCoordOfSquare.y - 1};
            place(state, currentSuareSide, currentPosition);
            Square currentSquare = {{leftUpCoordOfSquare.x, leftUpCoordOfSquare.y}, (size_t)currentSuareSide};
            state.currentSolution.push_back(currentSquare);
            state.currentPartsCount++;
        }
        if (!numberOfSquares) {
            prefillGridForKnownPatterns(state);
        }
        return true;
    }
#endif


void printStepikAnswer(GridState& state, size_t scale) {
    std::cout << state.bestPartsCount << "\n";
    for (const auto& sq : state.bestSolution) {
        size_t x = sq.squareLeftUpCoordinates.x;
        size_t y = sq.squareLeftUpCoordinates.y;
        size_t s = sq.squareSide;
        if (scale > 1) {
            if (x != 1) x = (x - 1) * scale + 1;
            if (y != 1) y = (y - 1) * scale + 1;
            s *= scale;
        }
        std::cout << x << " " << y << " " << s << "\n";
    }
}


void getOptimisation(GridState& state, size_t& N, size_t& scale) {
    if (state.currentPartsCount == 0) {
        scale = reduceToPrimeBase(N);
        if (scale > 1) {
            state = getEmptyGridState(N);
        }
        state = getEmptyGridState(N);
        prefillGridForKnownPatterns(state);
    }
}


int findTheMinimumPartition() {
    long long squareSide;
    std::cin >> squareSide;
    if (squareSide < 2 || squareSide > 63) {
        return EXIT_FAILURE;
    }
    size_t N = static_cast<size_t>(squareSide);
    GridState state = getEmptyGridState(N);
    size_t scale = 1;
    #if PRINTTIME
        clock_t start = clock();
    #endif
    #if !STEPIK
        if (!individualizationFunction(state)) {
            return EXIT_FAILURE;
        }
    #endif
    getOptimisation(state, N, scale);
    findMinimalNumberOfPartsRecursive(state);
    printStepikAnswer(state, scale);
    #if PRINTTIME
        clock_t end = clock();
        double seconds = (double)(end - start) / CLOCKS_PER_SEC;
        printf("Время выполнения: %.6f секунд\n", seconds);
    #endif
    delete[] state.binGrid;
    delete[] state.fullGridMask;
    return EXIT_SUCCESS;
}


int main() {
    return findTheMinimumPartition();
}