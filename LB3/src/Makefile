createDir:
	mkdir -p build
main: createDir levenstein.hpp
	g++ main.cpp levenstein.hpp -o build/main
genStr: createDir
	g++ generateStrings.cpp -o build/generateStrings -DFIRST_STRING_LENGTH=2500 -DSECOND_STRING_LENGTH=2500
all: main
	./build/main
autotest: main genStr

