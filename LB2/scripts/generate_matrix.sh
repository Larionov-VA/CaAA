#!/bin/bash
mkdir -p ./build
mkdir -p ./files/matrices
g++ ./utilities/generate_matrix.cpp -o ./build/generate_matrix
./build/generate_matrix