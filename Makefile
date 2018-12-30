.PHONY: all
all:
	clang -flto -O3 -c -Ivendor/uWebSockets/uSockets/src vendor/uWebSockets/uSockets/src/*.c vendor/uWebSockets/uSockets/src/eventing/*.c
	clang++ -flto -O3 -c -std=c++17 -Ivendor/uWebSockets/src -Ivendor/uWebSockets/uSockets/src main.cpp
	clang++ -flto -O3 -s *.o -o ./program -lz

run:
	./program

.PHONY: clean
clean:
	rm ./program
	rm ./*.o
