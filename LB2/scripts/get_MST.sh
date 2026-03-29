#!/bin/bash
mkdir -p ./build
if [ $# -eq 0 ]; then
    g++ ./src/main.cpp -o ./build/main
elif [ $# -eq 1 ]; then
    g++ ./src/main.cpp -o ./build/main -D MAX_VERTEX_DEG=$1
elif [ $# -eq 2 ]; then
    g++ ./src/main.cpp -o ./build/main -D MAX_VERTEX_DEG=$1 -D SHOW_INFO=$2
elif [ $# -eq 3 ]; then
    g++ ./src/main.cpp -o ./build/main -D MAX_VERTEX_DEG=$1 -D SHOW_INFO=$2 -D START_WITH=$3
fi
./build/main < ./files/matrices/example.txt
dot -Tsvg ./files/graph.dot -o ./files/graph.svg