#include <iostream>
#include <random>
#include <string>

#ifndef FIRST_STRING_LENGTH
#define FIRST_STRING_LENGTH 1000
#endif
#ifndef SECOND_STRING_LENGTH
#define SECOND_STRING_LENGTH 1000
#endif

const std::string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> distrib(0, 100);


inline char getRandomSymbol() {
    return ALPHABET[distrib(gen) % ALPHABET.length()];
}


std::string getString(int length) {
    std::string result;
    result.reserve(length);
    for(int i = 0; i < length; ++i) {
        result += getRandomSymbol();
    }
    return result;
}


int main() {
    std::cout << getString(FIRST_STRING_LENGTH) << '\n'
              << getString(SECOND_STRING_LENGTH) << '\n';
    return EXIT_SUCCESS;
}