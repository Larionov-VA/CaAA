#!/bin/bash
mkdir -p ./build
mkdir -p ./build/matrices
mkdir -p ./files
g++ ./utilities/generate_matrix.cpp -o ./build/generate_matrix
./build/generate_matrix