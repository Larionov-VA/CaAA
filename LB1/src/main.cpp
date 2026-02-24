#include <climits>
#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>

/*
Если при компиляции добавить флаг STEPIK=0, то будет скопмилированна версия
предусмотренная индивидуализацией. При проверке на stepik.org по умолчанию будет
выставлено значение STEPIK=1 и код будет корректно выполен в проверяющей
системе.
*/
#ifndef STEPIK
    #define STEPIK 1
#endif
/*
Если при компиляции добавить флаг COMPLEXITY_ANALYSIS=1, то будет
скопмилированна версия для записи и вывода в консоль времени работы программы,
количетсва вызовов рекурсии и количества вызовов функции canPlace. По умолчанию
COMPLEXITY_ANALYSIS=0, что позволяет проходить проверку на stepik.org.
*/
#ifndef COMPLEXITY_ANALYSIS
    #define COMPLEXITY_ANALYSIS 0
#endif
/*
Если COMPLEXITY_ANALYSIS == 1, то добавляет нужные для вывода и записи
библиотеки.
*/
#if COMPLEXITY_ANALYSIS
    #include <cstdio>
    #include <ctime>
#endif
/*
Если при компиляции добавить флаг PRINT_INFO=1, то будет скомпилированна версия
программы, которая выводит больше информации для проверки работы или отладки.
*/
#ifndef PRINT_INFO
    #define PRINT_INFO 0
#endif
/*
Если PRINT_INFO == 1, то добавляет нужные для вывода битового представления
строк матрицы.
*/
#if PRINT_INFO
    #include <bitset>
    #include <string>
#endif


/*
Структура для хранения информации о координатах.
*/
struct Position {
    size_t x;
    size_t y;
};


/*
Структура для хранения инфомарции о квадрате.
- Position squareLeftUpCoordinates -- позиция левого верхнего угла квадрата
- size_t squareSide -- сторона квадрата
*/
struct Square {
    Position squareLeftUpCoordinates;
    size_t squareSide;
};


/*
Структура для хранения и передачи информации о квадрате, который планируем
квадрировать (далее - сетка) и вспомогательной информации через рекурсивые
вызовы.
- size_t currentPartsCount -- количество размещенных в сетке квадратов в текущей
ветке рекурсии.
- size_t bestPartsCount -- лучшее среди всех ветвей количество размещенных
в сетке квадратов.
- size_t gridSide -- сторона сетки.
- uint64_t* binGrid -- массив битового представления строк сетки.
- uint64_t* fullGridMask -- маска для основной сетки, чтобы не вычислять каждый
раз.
- std::vector<Square> currentSolution -- вектор для хранения координат
и сторон квадратов размещенных в сетке в текущей ветке рекурсии.
- std::vector<Square> bestSolution -- вектор для хранения лучших координат и
сторон квадратов размещенных в сетке среди всех веток рекурсии.
Если при компиляции был задан флаг COMPLEXITY_ANALYSIS=1, то в структуру
добавляются еще два поля, которые нужны для определения сложности алгоритма:
- uint64_t calls -- количество рекурсивных вызовов. Увеличивается при каждом
вызове findMinimalNumberOfPartsRecursive().
- uint64_t canPlaceChecks -- количество вызовов функции canPlace.
*/
struct GridState {
    size_t currentPartsCount;
    size_t bestPartsCount;
    size_t gridSide;
    uint64_t* binGrid;
    uint64_t* fullGridMask;
    #if COMPLEXITY_ANALYSIS
        uint64_t calls;
        uint64_t canPlaceChecks;
    #endif
    std::vector<Square> currentSolution;
    std::vector<Square> bestSolution;
};


/*
Функция принимает на вход размер сетки gridSide, размер вставляемого квадрата
subSquareSide и координаты верхнего левого угла этого квадрата leftUpCoord.
Возвращает true, если квадрат не выходит за границу сетки и false, если выходит.
*/
bool fitsInsideGrid(size_t gridSide, size_t subSquareSide, Position leftUpCoord) {
    return leftUpCoord.x + subSquareSide <= gridSide &&
    leftUpCoord.y + subSquareSide <= gridSide;
}

/*
Функция принимает на вход размер сетки gridSide, размер вставляемого квадрата
subSquareSide и координаты верхнего левого угла этого квадрата leftUpCoord.
*/
bool canPlace(GridState& state, size_t side, Position leftUpCoord) {
    #if COMPLEXITY_ANALYSIS
        ++s.canPlaceChecks;
    #endif
    if (!fitsInsideGrid(state.gridSide, side, leftUpCoord)) {
        return false;
    }
    uint64_t line = ((1ULL << side) - 1) << leftUpCoord.x;
    for (size_t i = 0; i < side; ++i) {
        if (state.binGrid[leftUpCoord.y + i] & line) {
            return false;
        }
    }
    return true;
}


void place(GridState& state, size_t side, Position leftUpCoord) {
    uint64_t line = ((1ULL << side) - 1) << leftUpCoord.x;
    for (size_t i = 0; i < side; ++i) {
        state.binGrid[leftUpCoord.y + i] |= line;
    }
}


void remove(GridState& state, size_t side, Position leftUpCoord) {
    uint64_t line = ((1ULL << side) - 1) << leftUpCoord.x;
    for (size_t i = 0; i < side; ++i) {
        state.binGrid[leftUpCoord.y + i] &= ~line;
    }
}


Position getFirstPosToNextSquare(GridState& state) {
    for (size_t y = 0; y < state.gridSide; ++y) {
        uint64_t free = ~state.binGrid[y] & state.fullGridMask[y];
        if (free) {
            size_t x = __builtin_ctzll(free);
            return {x, y};
        }
    }
    return {0, 0};
}


bool isGridFull(GridState& state) {
    for (size_t i = 0; i < state.gridSide; ++i) {
        if ((state.binGrid[i] & state.fullGridMask[i]) != state.fullGridMask[i]) {
            return false;
        }
    }
    return true;
}


size_t getMaxSquareAtPosition(GridState& s, Position leftUpCoord) {
    size_t maxPossibleSquareSide = std::min(s.gridSide - leftUpCoord.x, s.gridSide - leftUpCoord.y);
    if (maxPossibleSquareSide > s.gridSide - 1) {
        maxPossibleSquareSide = s.gridSide - 1;
    }
    for (size_t side = maxPossibleSquareSide; side > 0; --side) {
        if (canPlace(s, side, leftUpCoord)) {
            return side;
        }
    }
    return 0;
}

#if PRINT_INFO
void printGrid(GridState& state) {
    for (size_t y = 0; y < state.gridSide; ++y) {
        std::string stringBinLine = std::bitset<64>(state.binGrid[y]).to_string();
        std::reverse(stringBinLine.begin(), stringBinLine.end());
        std::cout << stringBinLine.substr(0, state.gridSide) << '\n';
    }
}
#endif

void findMinimalNumberOfPartsRecursive(GridState& state) {
    #if COMPLEXITY_ANALYSIS
        ++state.calls;
    #endif
    #if PRINT_INFO
        printGrid(state);
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
        place(state, side, leftUpCoord);
        state.currentSolution.push_back({{leftUpCoord.x + 1, leftUpCoord.y + 1}, side});
        ++state.currentPartsCount;
        findMinimalNumberOfPartsRecursive(state);
        remove(state, side, leftUpCoord);
        state.currentSolution.pop_back();
        --state.currentPartsCount;
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
                place(state, half, {x0, y0});
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
            place(state, S[i], {X[i], Y[i]});
            state.currentSolution.push_back({{X[i] + 1, Y[i] + 1}, S[i]});
            ++state.currentPartsCount;
        }
    }
    else if (isPrime(N)) {
        size_t s1 = N / 2 + 1;
        size_t s2 = N / 2;
        place(state, s1, {0, 0});
        state.currentSolution.push_back({{1, 1}, s1});
        ++state.currentPartsCount;
        place(state, s2, {0, s2 + 1});
        state.currentSolution.push_back({{1, s2 + 2}, s2});
        ++state.currentPartsCount;
        place(state, s2, {s2 + 1, 0});
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


#if !STEPIK
    bool individualizationFunction(GridState& state) {
        int numberOfSquares = 0;
        std::cin >> numberOfSquares;
        Position leftUpCoordOfSquare;
        long long currentSuareSide;
        for (int i = 0; i < numberOfSquares; ++i) {
            std::cin >> leftUpCoordOfSquare.x >> leftUpCoordOfSquare.y >> currentSuareSide;
            if (currentSuareSide < 1 || currentSuareSide > (long long)state.gridSide) {
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

/*
Функция принимает на вход ссылку на структуру GridState state, ссылку на размер
сетки N и ссылку на показатель масштаба scale. Ничего не возвращает, но может
изменять state, N и scale в зависимости от значений N.
Если количество размещенных в сетке квадратов = 0, то применяется оптимизация
сокращения стороны сетки до простого числа, в ином случае, работа функции
заканчивается.
Если оптимизация сокращения стороны применяется, то создается новая пустая
сетка, меньше предыдущей на scale.
После создания новой сетки (если требуется) GridState& state передается
в функцию void prefillGridForKnownPatterns(GridState& state).
*/
void getOptimisation(GridState& state, size_t& N, size_t& scale) {
    if (state.currentPartsCount == 0) {
        scale = reduceToPrimeBase(N);
        if (scale > 1) {
            state = getEmptyGridState(N);
        }
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

    #if COMPLEXITY_ANALYSIS
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

    #if COMPLEXITY_ANALYSIS
        clock_t end = clock();
        double seconds = (double)(end - start) / CLOCKS_PER_SEC;
        printf("Время выполнения: %.6f секунд\n", seconds);
        printf("Рекурсивных вызовов: %lu\n", state.calls);
        printf("Проверок canPlace: %lu\n", state.canPlaceChecks);
    #endif

    delete[] state.binGrid;
    delete[] state.fullGridMask;
    return EXIT_SUCCESS;
}


int main() {
    return findTheMinimumPartition();
}