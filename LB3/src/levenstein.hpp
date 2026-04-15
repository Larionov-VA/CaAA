#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <limits>
#include <optional>
#include <codecvt>
#include <locale>


namespace OperationsCosts {
    int replaceCost = 1;
    int deleteCost = 1;
    int insertCost = 1;
}


std::wstring to_wstring(const std::string& s) {
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(s);
}


std::vector<std::optional<wchar_t>> getUniqueCharsWithNone(const std::wstring& str) {
    std::set<wchar_t> uniqueSet(str.begin(), str.end());
    std::vector<std::optional<wchar_t>> chars;
    for (wchar_t c : uniqueSet) {
        chars.push_back(c);
    }
    chars.push_back(std::nullopt);
    return chars;
}


int getLevenshteinDistance(const std::string& firstString, const std::string& secondString) {
    std::wstring wFirstString = to_wstring(firstString);
    std::wstring wSecondString = to_wstring(secondString);
    int firstStringLength = wFirstString.length();
    int secondStringLength = wSecondString.length();
    auto chars = getUniqueCharsWithNone(wSecondString);
    int K = chars.size();
    std::vector<std::vector<int>> pref;
    for (const auto& opt_c : chars) {
        if (opt_c.has_value()) {
            wchar_t c = opt_c.value();
            std::vector<int> arr(secondStringLength + 1, 0);
            int cnt = 0;
            for (int j = 1; j <= secondStringLength; ++j) {
                if (wSecondString[j - 1] == c) {
                    ++cnt;
                }
                arr[j] = cnt;
            }
            pref.push_back(arr);
        } else {
            pref.push_back(std::vector<int>(secondStringLength + 1, 0));
        }
    }
    std::vector<int> prev(secondStringLength + 1);
    for (int j = 0; j <= secondStringLength; ++j) {
        prev[j] = j * OperationsCosts::insertCost;
    }
    std::vector<int> cur(secondStringLength + 1);
    for (int i = 1; i <= firstStringLength; ++i) {
        wchar_t currSymbol = wFirstString[i - 1];
        cur[0] = prev[0] + (currSymbol == L'*' ? 0 : OperationsCosts::deleteCost);
        if (currSymbol != L'*') {
            for (int j = 1; j <= secondStringLength; ++j) {
                if (currSymbol == wSecondString[j - 1]) {
                    cur[j] = prev[j - 1];
                } else {
                    int costReplace = prev[j - 1] + OperationsCosts::replaceCost;
                    int costDelete = prev[j] + OperationsCosts::deleteCost;
                    int costInsert = cur[j - 1] + OperationsCosts::insertCost;
                    cur[j] = std::min({costReplace, costDelete, costInsert});
                }
            }
        }
        else {
            std::vector<int> minimalValue(K);
            for (int idx = 0; idx < K; ++idx) {
                minimalValue[idx] = prev[0];
            }
            for (int j = 1; j <= secondStringLength; ++j) {
                int val = prev[j] - j * OperationsCosts::insertCost;
                for (int idx = 0; idx < K; ++idx) {
                    int new_val = val + pref[idx][j] * OperationsCosts::replaceCost;
                    if (new_val < minimalValue[idx]) {
                        minimalValue[idx] = new_val;
                    }
                }
                int best = std::numeric_limits<int>::max();
                for (int idx = 0; idx < K; ++idx) {
                    int cost = minimalValue[idx] - pref[idx][j] * OperationsCosts::replaceCost;
                    if (cost < best) {
                        best = cost;
                    }
                }
                cur[j] = j * OperationsCosts::insertCost + best;
            }
        }
        std::swap(prev, cur);
    }
    return prev[secondStringLength];
}