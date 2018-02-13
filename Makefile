CFLAGS=-I . -std=c++11
LFLAGS=-lSDL2

CHIP8=chip8
MAIN=main
OUT=chip8

all: main

${MAIN}: ${MAIN}.o ${CHIP8}.o
	g++ ${CHIP8}.o ${MAIN}.o ${CFLAGS} ${LFLAGS} -o ${OUT}
	
${MAIN}.o: ${MAIN}.cpp
	g++ ${MAIN}.cpp ${CFLAGS} -c 
	
${CHIP8}.o: ${CHIP8}.cpp ${CHIP8}.hpp
	g++ ${CHIP8}.cpp ${CFLAGS} -c 
	
clean:
	rm *.o
	rm $(OUT)