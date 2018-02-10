FLAGS=-I . -std=c++11

all: test

test: test.o chip8.o
	g++ ${FLAGS} test.o chip8.o -o test
	
test.o: test.cpp
	g++ ${FLAGS} -c test.cpp
	
chip8.o: chip8.cpp chip8.hpp
	g++ ${FLAGS} -c chip8.cpp