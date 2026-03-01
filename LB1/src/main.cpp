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
    #include <fstream>
    std::ofstream out("intermediate_results.txt");
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
- uint64_t fullLineMask -- маска для строки, чтобы не вычислять каждый раз.
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
    uint64_t fullLineMask;
    #if COMPLEXITY_ANALYSIS
        uint64_t calls;
        uint64_t canPlaceChecks;
    #endif
    std::vector<Square> currentSolution;
    std::vector<Square> bestSolution;
};


/*
Функция принимает на вход размер сетки gridSide, размер вставляемого квадрата
squareSide и координаты верхнего левого угла этого квадрата leftUpCoord.
Возвращает true, если квадрат не выходит за границу сетки и false, если выходит.
*/
bool fitsInsideGrid(size_t gridSide, size_t squareSide, Position leftUpCoord) {
    return leftUpCoord.x + squareSide <= gridSide &&
    leftUpCoord.y + squareSide <= gridSide;
}


/*
Функция принимает на вход ссылку на структуру GridState state,
размер вставляемого квадрата squareSide и координаты верхнего левого угла этого
квадрата leftUpCoord.
Возвращает true, если квадрат со стороной squareSide и координатами  leftUpCoord
можно вставить в сетку.
*/
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

/*
Функция принимает на вход размер вставляемого квадрата squareSide и координату.
Возвращает маску для строки, соответствующую вставляемому квадрату.
*/
inline uint64_t getLineMask(size_t squareSide, size_t xCoord) {
    return ((1ULL << squareSide) - 1) << xCoord;
}


/*
Функция принимает на вход ссылку на структуру GridState state, маску для строк
lineMask, размер вставляемого квадрата squareSide и координату верхнего левого
угла этого квадрата yCoord.
Итерируясь от верхней координаты квадрата к нижней, вставляет максу строки в
строку сетки с помощью побитового ИЛИ с присваиванием.
Пример:
state.binGrid | squareSide | leftUpCoord |   line    | result  |
 0b00111      | 2          | {3,0}       |  0b11000  | 0b11111 |
 0b00111      |            |             |           | 0b11111 |
 0b00111      |            |             |           | 0b00111 |
 0b00000      |            |             |           | 0b00000 |
 0b00000      |            |             |           | 0b00000 |
*/
void place(GridState& state, uint64_t lineMask, size_t squareSide, size_t y) {
    for (size_t i = 0; i < squareSide; ++i) {
        state.binGrid[y + i] |= lineMask;
    }
}


/*
Функция принимает на вход ссылку на структуру GridState state, маску для строк
lineMask, размер вставляемого квадрата squareSide и координату верхнего левого
угла этого квадрата yCoord.
Итерируясь от верхней координаты квадрата к нижней, вставляет инвертированную
максу строки в строку сетки с помощью побитового И с присваиванием.
Пример:
state.binGrid | squareSide | leftUpCoord |   line    | result  |
 0b11111      | 2          | {3,0}       |  0b11000  | 0b00111 |
 0b11111      |            |             |           | 0b00111 |
 0b00111      |            |             |           | 0b00111 |
 0b00000      |            |             |           | 0b00000 |
 0b00000      |            |             |           | 0b00000 |
*/
void remove(GridState& state, uint64_t lineMask, size_t squareSide, size_t y) {
    for (size_t i = 0; i < squareSide; ++i) {
        state.binGrid[y + i] &= ~lineMask;
    }
}


/*
Функция получает на вход ссылку на структуру GridState, возвращает первую
левую сверху позицию.
Встроенная функция компилятора gcc (g++)__builtin_ctzll используется для
получения количества нулей в бинарном представлении строки стоящих в
младших битах.

Примеры:
 state.binGrid | free       | __builtin_ctzll | result  |
 0b00111       | 0b11000    | 3               | 3,0     |
 0b00111       |            |                 |         |
 0b00111       |            |                 |         |
 0b00000       |            |                 |         |
 0b00000       |            |                 |         |

 state.binGrid | free       | __builtin_ctzll | result  |
 0b1111111     | 0b0000000  | -               | -       |
 0b1111111     | 0b0000000  | -               | -       |
 0b1111111     | 0b0000000  | -               | -       |
 0b1111111     | 0b0000000  | -               | -       |
 0b0000011     | 0b1111100  | 2               | 2,5     |
 0b0000000     |            |                 |         |
 0b0000000     |            |                 |         |
*/
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

/*
Функция получает на вход ссылку на структуру GridState, возвращает false, если
находит не до конца заполненную строку и true, если прошла по всем строкам и
не нашла строк не равных полной маске строки.
*/
bool isGridFull(GridState& state) {
    for (size_t i = 0; i < state.gridSide; ++i) {
        if ((state.binGrid[i] & state.fullLineMask) != state.fullLineMask) {
            return false;
        }
    }
    return true;
}


/*
Функция получает на вход ссылку на структуру GridState и координаты левого
верхнего угла квадрата leftUpCoord. Возвращает максимальную сторону квадрата,
который можно поместить в leftUpCoord.
*/
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


#if PRINT_INFO
/*
Фунция принимает на вход ссылку на структуру GridState. Выводит в консоль
сетку переворачивая и обрезая битовое представление.
*/
void printGrid(GridState& state) {
    out << '\n';
    for (size_t y = 0; y < state.gridSide; ++y) {
        std::string strBinLine = std::bitset<64>(state.binGrid[y]).to_string();
        std::reverse(strBinLine.begin(), strBinLine.end());
        out << strBinLine.substr(0, state.gridSide) << '\n';
    }
    out << '\n';
}

void printFilledGrid(GridState& state, std::vector<Square> solution) {
    out << "\n";
    size_t N = state.gridSide;
    std::vector<std::vector<size_t>> grid(N, std::vector<size_t>(N, 0));
    for (size_t idx = 0; idx < solution.size(); ++idx) {
        size_t squareNumber = idx + 1;
        size_t x = solution[idx].squareLeftUpCoordinates.x - 1;
        size_t y = solution[idx].squareLeftUpCoordinates.y - 1;
        size_t side = solution[idx].squareSide;
        for (size_t dy = 0; dy < side; ++dy) {
            for (size_t dx = 0; dx < side; ++dx) {
                grid[y + dy][x + dx] = squareNumber;
            }
        }
    }
    for (size_t i = 0; i < N; ++i) {
        for (size_t j = 0; j < N; ++j) {
            out << grid[i][j] << " ";
        }
        out << "\n";
    }
    out << "\n";
}
#endif


/*
Функция получает на вход ссылку на структуру GridState state.
Рекурсивно перебирает все возможные варианты разбиения сетки на квадраты
с использованием возврата (backtracking).

Алгоритм работы:
1) Если текущее количество квадратов в ветке >= лучшему найденному,
   дальнейший перебор не имеет смысла — происходит отсечение ветки.
2) Если сетка полностью заполнена, обновляется лучшее решение.
3) Находится первая свободная сверху слева позиция.
4) В этой позиции перебираются все возможные стороны квадрата
   (от максимальной к минимальной).
5) Квадрат размещается, вызывается рекурсия, затем происходит откат состояния.

Если при компиляции задан флаг COMPLEXITY_ANALYSIS=1,
то дополнительно увеличивается счетчик рекурсивных вызовов.
*/
void findMinimalNumberOfPartsRecursive(GridState& state) {
    #if COMPLEXITY_ANALYSIS
        ++state.calls;
    #endif
    if (state.currentPartsCount == state.bestPartsCount) {
        #if PRINT_INFO
            out << "Текущее разбиение currentPartsCount = "
                << state.currentPartsCount
                << " квадратов,\nчто равно лучшему разбиению bestPartsCount = "
                << state.bestPartsCount << ".\nОбрезаем эту ветвь рекурсии.\n";
            printFilledGrid(state, state.currentSolution);
        #endif
        return;
    }
    if (isGridFull(state)) {
        #if PRINT_INFO
            out << "Найдено полное разбиение.\nПредыдущее лучшее разбиение: "
                << (state.bestPartsCount == SIZE_MAX ? "-" : std::to_string(state.bestPartsCount))
                << "\nНовое лучшее разбиение: "
                << state.currentPartsCount << "\n";
            printFilledGrid(state, state.currentSolution);
        #endif
        state.bestSolution = state.currentSolution;
        state.bestPartsCount = state.currentPartsCount;
        return;
    }
    Position leftUpCoord = getFirstPosToNextSquare(state);
    size_t maxSide = getMaxSquareAtPosition(state, leftUpCoord);
    for (size_t side = maxSide; side > 0; --side) {
        uint64_t currentLineMask = getLineMask(side, leftUpCoord.x);
        #if PRINT_INFO
            out << "Квадрат со стороной " << side
                << " поставлен в координаты x = " << leftUpCoord.x
                << " y = " << leftUpCoord.y << "\n";
        #endif
        place(state, currentLineMask, side, leftUpCoord.y);
        Square currentSquare = {{leftUpCoord.x + 1, leftUpCoord.y + 1}, side};
        state.currentSolution.push_back(currentSquare);
        ++state.currentPartsCount;
        findMinimalNumberOfPartsRecursive(state);
        #if PRINT_INFO
            out << "Квадрат со стороной " << side
                << " удален по координатам x = " << leftUpCoord.x
                << " y = " << leftUpCoord.y << "\n";
        #endif
        remove(state, currentLineMask, side, leftUpCoord.y);
        state.currentSolution.pop_back();
        --state.currentPartsCount;
    }
}


/*
Функция принимает на вход размер сетки correctSquareSide.
Создает пустую структуру GridState:
- выделяет динамический массив строк binGrid,
- инициализирует его нулями,
- вычисляет полную маску строки fullLine,
- устанавливает currentPartsCount = 0,
- устанавливает bestPartsCount = INT_MAX,
- инициализирует векторы решений как пустые.

Возвращает созданную структуру GridState.
*/
GridState getEmptyGridState(size_t correctSquareSide) {
    uint64_t* emptyGrid = new uint64_t[correctSquareSide]{};
    uint64_t fullLine = (1ULL << correctSquareSide) - 1;
    return {
        0,
        SIZE_MAX,
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


/*
Функция принимает на вход число N.
Возвращает true, если N является простым числом, иначе false.

Используется оптимизированная проверка:
- сначала отсекаются N <= 3,
- затем проверка делимости на 2 и 3,
- далее перебор делителей вида 6k ± 1 до sqrt(N).
*/
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


/*
Функция принимает на вход ссылку на структуру GridState state.
В зависимости от размера сетки N выполняет предзаполнение сетки
известными шаблонами разбиений, что позволяет сократить перебор.

1) Если N кратно 2:
   Сетка разбивается на 4 равных квадрата со стороной N/2.

2) Если N кратно 3:
   Используется известное разбиение на 6 квадратов:
   один большой 2k x 2k и пять квадратов k x k.

3) Если N — простое:
   Используется стандартная конфигурация из трех квадратов:
   - один большой (N/2 + 1),
   - два квадрата (N/2).

Во всех случаях:
- квадраты размещаются в binGrid,
- добавляются в currentSolution,
- увеличивается currentPartsCount.
*/
void prefillGridForKnownPatterns(GridState& state) {
    size_t N = state.gridSide;
    if (N % 2 == 0) {
        #if PRINT_INFO
            out << "Сторона квадрата " << N << " делится на два.\n"
                << "Лучшее разбиение 4."
                << " Рекурсия не требуется, задача решена.";
        #endif
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
        #if PRINT_INFO
            out << "Сторона квадрата " << N << " делится на три.\n"
                << "Лучшее разбиение 6."
                << " Рекурсия не требуется, задача решена.";
        #endif
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
        #if PRINT_INFO
            out << "Сторона квадрата " << N << " -- простое число.\n"
                << "Можно использовать оптимизацию, в лучшем результате всегда"
                << " будет 3 квадрата: \nодин со стороной N / 2 + 1 в координатах 0,0 "
                << "\nи два со сторонами N / 2 в координатах (N / 2 + 1,0) и (0, N / 2 + 1)\n";
        #endif
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


/*
Функция принимает на вход ссылку на размер сетки N.
Пытается сократить размер сетки до простого делителя.

Если находится простой делитель p:
- вычисляется scale = N / p,
- N заменяется на p,
- возвращается scale.

Если делитель не найден,
возвращается 1 (масштабирование не требуется).
*/
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
/*
Функция принимает на вход ссылку на структуру GridState state.
Используется только при компиляции с флагом STEPIK=0.

Позволяет задать начальное состояние сетки:
1) Считывается количество квадратов numberOfSquares.
2) Для каждого квадрата считываются:
   - координаты левого верхнего угла (x, y) в 1-индексации,
   - сторона квадрата.
3) Проверяется:
   - корректность координат (>= 1),
   - корректность стороны квадрата,
   - размещение квадрата внутри сетки,
   - отсутствие пересечения с уже размещёнными квадратами.
4) Координаты переводятся во внутреннюю 0-индексацию.
5) Квадрат размещается в битовой сетке с помощью place().
6) Информация о квадрате сохраняется в currentSolution.
7) Увеличивается счётчик currentPartsCount.

Возвращает:
- true — если ввод корректен,
- false — если обнаружена ошибка входных данных.
*/
bool individualizationFunction(GridState& state) {
    int numberOfSquares = 0;
    std::cin >> numberOfSquares;

    Position leftUpCoord;
    long long currentSquareSide;

    for (int i = 0; i < numberOfSquares; ++i) {
        std::cin >> leftUpCoord.x >> leftUpCoord.y >> currentSquareSide;
        if (leftUpCoord.x < 1 || leftUpCoord.y < 1) {
            return false;
        }
        if (currentSquareSide < 1 || currentSquareSide > (long long)state.gridSide) {
            return false;
        }
        size_t xCoord = leftUpCoord.x - 1;
        size_t yCoord = leftUpCoord.y - 1;
        if (!fitsInsideGrid(state.gridSide, (size_t)currentSquareSide, {xCoord, yCoord})) {
            return false;
        }
        if (!canPlace(state, (size_t)currentSquareSide, {xCoord, yCoord})) {
            return false;
        }
        uint64_t lineMask = getLineMask((size_t)currentSquareSide, xCoord);
        place(state, lineMask, (size_t)currentSquareSide, yCoord);
        Square currentSquare = {{xCoord + 1, yCoord + 1}, (size_t)currentSquareSide};
        state.currentSolution.push_back(currentSquare);
        state.currentPartsCount++;
    }
    return true;
}
#endif


/*
Функция принимает на вход ссылку на структуру GridState state
и коэффициент масштабирования scale.

Выводит:
1) минимальное количество квадратов,
2) координаты и стороны каждого квадрата.

Если применялось масштабирование (scale > 1),
то координаты и стороны квадратов домножаются на scale,
при этом координата 1 остается неизменной.
*/
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
После, если scale был изменен (>1), создается новая сетка state, меньше
предыдущей на scale, далее сетка передается в функцию
void prefillGridForKnownPatterns(GridState& state) для предзаполнения, если
это возможно.
*/
void getOptimisation(GridState& state, size_t& N, size_t& scale) {
    if (state.currentPartsCount == 0) {
        scale = reduceToPrimeBase(N);
        #if PRINT_INFO
            out << "Запущена функция getOptimisation().\n";
        #endif
        if (scale > 1) {
            #if PRINT_INFO
                out << "Удалось сократить N = " << N*scale << " до простого числа "
                    << N << ".\n";
            #endif
            state = getEmptyGridState(N);
        }
        prefillGridForKnownPatterns(state);
    }
}


/*
Функция является основной управляющей функцией программы.

Алгоритм работы:
1) Считывает размер исходного квадрата.
2) Проверяет корректность входных данных (2 <= N <= 63).
3) Создает пустую сетку.
4) При необходимости выполняет индивидуализацию.
5) Применяет оптимизации (масштабирование и предзаполнение).
6) Запускает рекурсивный поиск минимального разбиения.
7) Выводит ответ.
8) При COMPLEXITY_ANALYSIS=1 выводит статистику.
9) Освобождает динамически выделенную память.

Возвращает EXIT_SUCCESS или EXIT_FAILURE.
*/
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
    #if PRINT_INFO
        out << "\nИтоговое разбиение:\n";
        printFilledGrid(state, state.bestSolution);
    #endif
    #if COMPLEXITY_ANALYSIS
        clock_t end = clock();
        double seconds = (double)(end - start) / CLOCKS_PER_SEC;
        printf("Время выполнения: %.6f секунд\n", seconds);
        printf("Рекурсивных вызовов: %lu\n", state.calls);
        printf("Проверок canPlace: %lu\n", state.canPlaceChecks);
    #endif

    delete[] state.binGrid;
    return EXIT_SUCCESS;
}


int main() {
    return findTheMinimumPartition();
}