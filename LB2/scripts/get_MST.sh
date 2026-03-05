#!/bin/bash
mkdir -p ./build
g++ ./src/main.cpp -o ./build/main
./build/main < ./files/matrices/example.txt
dot -Tsvg ./files/graph.dot -o ./files/graph.svg
dot -Tsvg ./files/MST.dot -o ./files/MST.svg