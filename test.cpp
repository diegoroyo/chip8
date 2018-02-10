#include <chip8.hpp>
#include <iostream>
#include <cstdint>
#include <thread>
#include <chrono>

int main() {
	uint16_t testprog[] = {
		0x6011,	// LD V0, 0x11
		0x6112,	// LD V1, 0x12
		0x6213	// LD V2, 0x13
	};
	int numInstr = 1 + sizeof(testprog) / 2; // contando la primera instruccion (JP 0x200)
	
	Chip8 chip8;
	chip8.loadProgram(testprog, numInstr);
	int i = 0;
	while (i < numInstr) {
		chip8.step();
		
		// Estado del interprete 
		i++;
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		std::cout << "---------------- INSTRUCCION " << i << " ----------------" << std::endl;
		std::cout << std::endl;
		std::cout << chip8.getDisplayState() << std::endl;
		std::cout << chip8.getRegistersState() << std::endl;
	}
}