#!/bin/bash
mkdir -p ./build
g++ ./utilities/generate_matrix.cpp -o ./build/generate_matrix
./build/generate_matrix