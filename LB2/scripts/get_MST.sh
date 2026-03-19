#!/bin/bash
mkdir -p ./build
if [ $# -eq 0 ]; then
    g++ ./src/main.cpp -o ./build/main
else
    g++ ./src/main.cpp -o ./build/main -DMAX_VERTEX_DEG=$1
fi
./build/main < ./files/matrices/example.txt
dot -Tsvg ./files/graph.dot -o ./files/graph.svg