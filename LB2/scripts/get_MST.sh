#!/bin/bash
mkdir -p ./build
if [ $# -eq 0 ]; then
    g++ ./src/main.cpp -o ./build/main
elif [ $# -eq 1 ]; then
    g++ ./src/main.cpp -o ./build/main -DMAX_VERTEX_DEG=$1
elif [ $# -eq 2 ]; then
    if [ "$1" -eq 0 ]; then
        g++ ./src/main.cpp -o ./build/main -DSHOW_INFO=$2
    else
        g++ ./src/main.cpp -o ./build/main -DMAX_VERTEX_DEG=$1 -DSHOW_INFO=$2
    fi
fi
./build/main < ./files/matrices/example.txt
dot -Tsvg ./files/graph.dot -o ./files/graph.svg