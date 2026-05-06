#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

int main() {
    string firstString, secondString;
    char replaceChar = '\0', deleteChar = '\0';
    int replaceTargetCharCost = 1, deleteTargetCharCost = 1;

    if (!getline(cin, firstString)) return 1;
    if (!getline(cin, secondString)) return 1;
    if (!(cin >> replaceChar >> replaceTargetCharCost >> deleteChar >> deleteTargetCharCost)) return 1;

    int firStrLength = firstString.length();
    int secStrLength = secondString.length();

    // Два вектора для расстояний (текущий и предыдущий)
    vector<int> prevDistanceVec(secStrLength + 1);
    vector<int> currDistanceVec(secStrLength + 1);

    // Два вектора для хранения путей (накапливаем строки предписаний)
    vector<string> prevPathStr(secStrLength + 1);
    vector<string> currPathStr(secStrLength + 1);

    // Инициализация первой строки (вставки)
    for (int j = 0; j <= secStrLength; j++) {
        prevDistanceVec[j] = j;
        for(int k=0; k<j; k++) prevPathStr[j] += 'I';
    }
    for (int i = 1; i <= firStrLength; i++) {
        // Базовый случай для начала строки (удаление)
        int currentDeleteCost = (firstString[i - 1] == deleteChar) ? deleteTargetCharCost : 1;
        currDistanceVec[0] = prevDistanceVec[0] + currentDeleteCost;
        currPathStr[0] = prevPathStr[0] + 'D';
        for (int j = 1; j <= secStrLength; j++) {
            if (firstString[i - 1] == secondString[j - 1]) {
                currDistanceVec[j] = prevDistanceVec[j - 1];
                currPathStr[j] = prevPathStr[j - 1] + 'M';
            } else {
                // Замена
                int currentReplaceCost = (secondString[j - 1] == replaceChar) ? replaceTargetCharCost : 1;
                int rep = prevDistanceVec[j - 1] + currentReplaceCost;

                // Удаление
                int del = prevDistanceVec[j] + currentDeleteCost;

                // Вставка
                int ins = currDistanceVec[j - 1] + 1;

                int res = min({rep, del, ins});
                currDistanceVec[j] = res;

                if (res == rep) {
                    currPathStr[j] = prevPathStr[j - 1] + 'R';
                }
                else if (res == del) {
                    currPathStr[j] = prevPathStr[j] + 'D';
                }
                else {
                    currPathStr[j] = currPathStr[j - 1] + 'I';
                }
            }
        }
        prevDistanceVec = currDistanceVec;
        prevPathStr = currPathStr;
    }
    cout << currDistanceVec[secStrLength] << endl;
    cout << currPathStr[secStrLength] << endl;
    return 0;
}