.PHONY: all run clean

all: clean build

.ONESHELL:
build:
	mkdir build
	cd build
	clang -flto -O3 -c -I../src/vendor/uWebSockets/uSockets/src ../src/vendor/uWebSockets/uSockets/src/*.c ../src/vendor/uWebSockets/uSockets/src/eventing/*.c
	clang++ -flto -O3 -c -std=c++17 -I../src/vendor/uWebSockets/src -I../src/vendor/uWebSockets/uSockets/src ../src/main.cpp
	clang++ -flto -O3 -s *.o -o ./program -lz

run:
	./build/program

clean:
	rm -fr ./build
