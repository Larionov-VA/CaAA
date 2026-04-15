#include "levenstein.hpp"


int main() {
    std::string firstString, secondString;
    std::cin >> firstString >> secondString;
    std::cout << getLevenshteinDistance(firstString, secondString) << '\n';
    return EXIT_SUCCESS;
}