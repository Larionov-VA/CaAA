#!/bin/bash
mkdir -p ./build
mkdir -p ./files/matrices
if [ $# -eq 0 ]; then
    g++ ./utilities/generate_matrix.cpp -o ./build/generate_matrix
else
    g++ ./utilities/generate_matrix.cpp -o ./build/generate_matrix -DMATRIX_SIZE=$1
fi
./build/generate_matrix
