#!/bin/bash
mkdir -p ./build
g++ ./src/main.cpp -o ./build/main
./build/main < ./utilities/matrices/example.txt