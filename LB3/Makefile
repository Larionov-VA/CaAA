

createDir:
	mkdir -p build
main: createDir src/levenstein.hpp
	g++ src/main.cpp src/levenstein.hpp -o build/main
genStr: createDir
	g++ src/generateStrings.cpp -o build/generateStrings -DFIRST_STRING_LENGTH=$(FIRST) -DSECOND_STRING_LENGTH=$(SECOND)
all: main
	./build/main
autotest: main genStr
	./build/generateStrings | ./build/main
test-default:
	$(MAKE) autotest FIRST=2500 SECOND=2500
test-small:
	$(MAKE) autotest FIRST=10 SECOND=10
test-diff:
	$(MAKE) autotest FIRST=100 SECOND=500
clean:
	rm -rf build