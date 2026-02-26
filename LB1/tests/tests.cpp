#include <cassert>
#include <iostream>

#include "./squaring.hpp"

void test_isPrime() {
    assert(!isPrime(0));
    assert(!isPrime(1));
    assert(isPrime(2));
    assert(isPrime(3));
    assert(!isPrime(4));
    assert(isPrime(5));
    assert(!isPrime(9));
    assert(isPrime(61));
}

void test_fitsInsideGrid() {
    assert(fitsInsideGrid(5, 2, {0,0}));
    assert(fitsInsideGrid(5, 5, {0,0}));
    assert(!fitsInsideGrid(5, 3, {3,3}));
    assert(!fitsInsideGrid(5, 6, {0,0}));
}

void test_place_and_remove() {
    GridState state = getEmptyGridState(4);

    uint64_t mask = getLineMask(2, 1);
    place(state, mask, 2, 0);

    assert(state.binGrid[0] == 0b0110);
    assert(state.binGrid[1] == 0b0110);

    remove(state, mask, 2, 0);

    assert(state.binGrid[0] == 0);
    assert(state.binGrid[1] == 0);

    delete[] state.binGrid;
}

void test_canPlace() {
    GridState state = getEmptyGridState(4);

    assert(canPlace(state, 2, {0,0}));
    place(state, getLineMask(2,0), 2, 0);

    assert(!canPlace(state, 2, {0,0}));
    assert(canPlace(state, 2, {2,0}));

    delete[] state.binGrid;
}

void test_reduceToPrimeBase() {
    size_t N = 25;
    size_t scale = reduceToPrimeBase(N);
    assert(scale == 5);
    assert(N == 5);

    N = 7;
    scale = reduceToPrimeBase(N);
    assert(scale == 1);
    assert(N == 7);
}

void test_prefill_even() {
    GridState state = getEmptyGridState(4);
    prefillGridForKnownPatterns(state);

    assert(state.currentPartsCount == 4);
    assert(isGridFull(state));

    delete[] state.binGrid;
}

void test_prefill_div3() {
    GridState state = getEmptyGridState(9);
    prefillGridForKnownPatterns(state);

    assert(state.currentPartsCount == 6);
    assert(isGridFull(state));

    delete[] state.binGrid;
}

int main() {
    test_isPrime();
    test_fitsInsideGrid();
    test_place_and_remove();
    test_canPlace();
    test_reduceToPrimeBase();
    test_prefill_even();
    test_prefill_div3();

    std::cout << "All tests passed!\n";
}