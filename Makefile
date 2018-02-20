CFLAGS=-I . -std=c++11
LFLAGS=-lSDL2

CHIP8=chip8
MAIN=main
INT=chip8

LEX=ch8asm
PAR=ch8asm
ASM=ch8asm

all: ${INT} ${ASM}

${INT}: ${MAIN}.o ${CHIP8}.o
	g++ ${CHIP8}.o ${MAIN}.o ${CFLAGS} ${LFLAGS} -o ${INT}
	
${MAIN}.o: ${MAIN}.cpp
	g++ ${MAIN}.cpp ${CFLAGS} -c 
	
${CHIP8}.o: ${CHIP8}.cpp ${CHIP8}.hpp
	g++ ${CHIP8}.cpp ${CFLAGS} -c 
	
${ASM}: lex.yy.c y.tab.c
	gcc y.tab.c lex.yy.c -lfl -o ${ASM}
	
lex.yy.c: ${LEX}.l
	flex ${LEX}.l

y.tab.c: ${PAR}.y
	bison -yd ${PAR}.y
	
clean:
	rm *.o
	rm lex.yy.c
	rm y.tab.c
	rm y.tab.h
	rm ${INT}
	rm ${ASM}