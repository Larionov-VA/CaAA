#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

#ifndef REPCOST
#define REPCOST 1
#endif
#ifndef DELCOST
#define DELCOST 1
#endif
#ifndef INSCOST
#define INSCOST 1
#endif

#ifdef LOGGING
ofstream logFile("file.log");
#endif


/*
Структура для хранения и передачи информации о вводе.
    firstString - первая строка
    secondString - вторая строка
    replaceChar - специальный заменяемый символ
    deleteChar - специальный удаляемый символ
    replaceTargetCharCost - стоимость замены на спец. заменяемый символ
    deleteTargetCharCost - стоимость удаления спец. удаляемого символа
*/
struct infoStruct {
    string firstString = "";
    string secondString = "";
    char replaceChar = 'a';
    char deleteChar = 'a';
    int replaceTargetCharCost = 1;
    int deleteTargetCharCost = 1;
};


/*
Функция для получения ввода. Ожидает ввод в формате
<Строка_1>
<Строка_2>
<Спец. заменяемый символ> <цена его замены> <Спец. удаляемый символ> <цена его замены>
Возвращает true, если ввод принят корректно и false, если что-то пошло не так
*/
bool getInput(infoStruct& input) {
    if (!getline(cin, input.firstString)) {
        return false;
    }
    if (!getline(cin, input.secondString)) {
        return false;
    }
    if (!(cin >> input.replaceChar >> input.replaceTargetCharCost)) {
        return false;
    }
    if (!(cin >> input.deleteChar >> input.deleteTargetCharCost)) {
        return false;
    }
    return true;
}


/*
Функция для получения корректной стоимости удаления текущего символа.
Проверяет является ли char currentChar специальным удаляемым символом
specialDeleteChar, если да, то возвращает int specialDeleteCost,
в противном случае DELCOST (по умолчанию 1)
*/
inline int getDelCost(
    char currentChar,
    char specialDeleteChar,
    int specialDeleteCost
) {
    return (currentChar == specialDeleteChar) ? specialDeleteCost : DELCOST;
}


/*
Функция для получения корректной стоимости замены текущего символа.
Проверяет является ли char currentChar специальным заменяемым символом
specialReplaceChar, если да, то возвращает int specialReplaceCost,
в противном случае REPCOST (по умолчанию 1)
*/
inline int getRepCost(
    char currentChar,
    char specialReplaceChar,
    int specialReplaceCost
) {
    return (currentChar == specialReplaceChar) ? specialReplaceCost : REPCOST;
}


/*
Функций для вывода результата в формате:
<Редакционное расстояние>
<Редакционное предписание>
На вход принимает матрицу операций vector<vector<char>>& choice
и значение расстояния int& resultDistance
Ничего не возвращает
*/
inline void printResult(
    vector<vector<char>>& choice,
    int& resultDistance
) {
    // Восстанавливаем путь обратным ходом
    string path;
    size_t i = choice.size() - 1, j = choice[0].size() - 1;
    while (i > 0 || j > 0) {
        char& op = choice[i][j];
        path += op;
        if (op == 'M' || op == 'R') {
            --i; --j;
        } else if (op == 'D') {
            --i;
        } else {
            --j;
        }
    }
    // Путь был собран с конца, переворачиваем
    reverse(path.begin(), path.end());
    // Вывод результата
    cout << resultDistance << endl;
    cout << path << endl;
#ifdef LOGGING
    logFile << "Редакционное расстояние: " << resultDistance << "\n";
    logFile << "Редакционное предписание: " << path << "\n";
#endif
}


#ifdef LOGGING
/*
Функция для журналирования. Принимает матрицу const vector<vector<char>>& choice
возращает ее в строковом виде
*/
string matrixToString(const vector<vector<char>>& choice) {
    string strMatrix;
    int n = (int)choice.size();
    if (n == 0) return "";
    int m = (int)choice[0].size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            strMatrix += choice[i][j];
            if (j != m - 1) strMatrix += ' ';
        }
        strMatrix += '\n';
    }
    return strMatrix;
}
#endif


/*
Основной алгоритм, функция создает структуру для вводимых данных,
ждет ввода через фукнцию getInput(infoStruct& input), создает два сменяющих
друг друга вектора, в которые итеративно будут записаны значения расстояния,
параллельно с этим в матрицу choice записываются все действия совершенные
над строками для восстановления операций и вывода результата через
фукнцию printResult(vector<vector<char>>& choice, int& resultDistance)
*/
int main() {
    infoStruct input;
    if (!getInput(input)) {
        return EXIT_FAILURE;
    }
    int N = input.firstString.length();
    int M = input.secondString.length();

#ifdef LOGGING
    logFile << "Первая строка:  \"" << input.firstString << "\"\n";
    logFile << "Вторая строка: \"" << input.secondString << "\"\n";
    logFile << "Спец. символ замены: '" << input.replaceChar
        << "' (стоимость замены на него: " << input.replaceTargetCharCost << ")\n";
    logFile << "Спец. символ удаления: '" << input.deleteChar
        << "' (стоимость удаления: " << input.deleteTargetCharCost << ")\n";
    logFile << "Базовые стоимости: замена=" << REPCOST
        << ", удаление=" << DELCOST
        << ", вставка=" << INSCOST << "\n\n";
#endif

    vector<int> prevDist(M + 1), currDist(M + 1);
    vector<vector<char>> choice(N + 1, vector<char>(M + 1, '?'));

    // Инициализация первой строки
    for (int j = 0; j <= M; ++j) {
        prevDist[j] = j;
        if (j > 0) {
            choice[0][j] = 'I';
        }
    }

    for (int i = 1; i <= N; ++i) {
        char& currFirstStrChar = input.firstString[i-1];
#ifdef LOGGING
        logFile << "Рассматриваем символ из первый строки: " << currFirstStrChar << '\n';
        logFile << "Текущая матрица операций: \n";
        logFile << matrixToString(choice);
        logFile << "\n\n";
#endif
        int delCost = getDelCost(
            currFirstStrChar,
            input.deleteChar,
            input.deleteTargetCharCost
        );
        currDist[0] = prevDist[0] + delCost;
        choice[i][0] = 'D';

        for (int j = 1; j <= M; ++j) {
            char& currSecondStrChar = input.secondString[j-1];
#ifdef LOGGING
            logFile << "Сравнимаем с символом из второй строки: " << currSecondStrChar << '\n';
#endif
            if (currFirstStrChar == currSecondStrChar) {
                // Совпадение
                currDist[j] = prevDist[j-1];
                choice[i][j] = 'M';
#ifdef LOGGING
                logFile << "Совпадение, расстояние не изменится\n";
#endif
            } else {
#ifdef LOGGING
                logFile << "Символы '" << currFirstStrChar << "' и '" << currSecondStrChar << "' не совпали, рассчитываем стоимость операций\n";
#endif
                // Замена
                int repCost = getRepCost(
                    currSecondStrChar,
                    input.replaceChar,
                    input.replaceTargetCharCost
                );
                int rep = prevDist[j-1] + repCost;
#ifdef LOGGING
                logFile << "Стоимость замены " << rep << '\n';
#endif
                // Удаление
                int del = prevDist[j] + delCost;
#ifdef LOGGING
                logFile << "Стоимость удаления " << del << '\n';
#endif
                // Вставка
                int ins = currDist[j-1] + INSCOST;
#ifdef LOGGING
                logFile << "Стоимость вставки " << ins << '\n';
                logFile << "Выбираем наименьшее. Приоритет R < D < I\n";
#endif
                if (rep <= del && rep <= ins) {
                    currDist[j] = rep;
                    choice[i][j] = 'R';
#ifdef LOGGING
                    logFile << "Выбрали замену\n\n";
#endif
                } else if (del <= ins) {
                    currDist[j] = del;
                    choice[i][j] = 'D';
#ifdef LOGGING
                    logFile << "Выбрали удаление\n\n";
#endif
                } else {
                    currDist[j] = ins;
                    choice[i][j] = 'I';
#ifdef LOGGING
                    logFile << "Выбрали вставку\n\n";
#endif
                }
            }
        }
        // Переставляем векторы расстояний для следующей итерации
        prevDist.swap(currDist);
    }
    printResult(choice,prevDist[M]);
    return EXIT_SUCCESS;
}