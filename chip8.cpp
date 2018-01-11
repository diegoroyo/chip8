//*****************************************************************
// File:    chip8.cpp
// Author:  Diego Royo
// Date:    enero 2018
//*****************************************************************

#include <chip8.hpp>

void Chip8::Step() {
	
	// Leer la instrucción y obtener los valores
	uint16_t opcode = ram[pc];
	pc += 2;
	
	uint16_t nnn = opcode & 0xFFF; 		// 12 bits de menor peso (addr)
	uint8_t n = opcode & 0xF; 			// 4 bits de menor peso (nibble)
	uint8_t x = (opcode >> 8) & 0xF; 	// Bits 8 a 11
	uint8_t y = (opcode >> 4) & 0xF; 	// Bits 4 a 7
	uint8_t kk = opcode & 0xFF; 		// 8 bits de menor peso (byte)
	
	displayUpdated = false;
	
	// Ejecutar la instrucción
	switch (opcode & 0xF000) {
		case 0x0000:
			if (opcode == 0x00E0) {
				// 00E0 - CLS: Clear the display
				memset(display, 0, sizeof(display));
				displayUpdated = true;
			} else if (opcode == 0x00EE) {
				// 00EE - RET: Return from a subroutine
				if (sp == 0) return; // No hay nada apilado
				pc = stack[--sp];
			} else {
				// 0nnn - SYS addr: Jump to a machine code routine at nnn
				// No soportada por el intérprete
			}
			break;
		case 0x1000:
			// 1nnn - JP addr: Jump to location nnn
			pc = nnn;
			break;
		case 0x2000:
			// 2nnn - CALL addr: Call subroutine at nnn
			if (sp >= 16) return; // Pila llena
			stack[sp++] = pc;
			pc = nnn;
			break;
		case 0x3000:
			// 3xkk - SE Vx, byte: Skip next instruction if Vx = kk
			if (v[x] == kk) pc += 2;
			break;
		case 0x4000:
			// 4xkk - SNE Vx, byte: Skip next instruction if Vx != kk
			if (v[x] != kk) pc += 2;
			break;
		case 0x5000:
			// 5xy0 - SE Vx, Vy: Skip next instruction if Vx = Vy
			if (v[x] == v[y]) pc += 2;
			break;
		case 0x6000:
			// 6xkk - LD Vx, byte: Set Vx = kk
			v[x] = kk;
			break;
		case 0x7000:
			// 7xkk - ADD Vx, byte: Set Vx = Vx + kk
			v[x] += kk;
			break;
		case 0x8000:
			if (n == 0x0) {
				// 8xy0 - LD Vx, Vy: Set Vx = Vy
				v[x] = v[y];
			} else if (n == 0x1) {
				// 8xy1 - OR Vx, Vy: Set Vx = Vx OR Vy
				v[x] |= v[y];
			} else if (n == 0x2) {
				// 8xy2 - AND Vx, Vy: Set Vx = Vx AND Vy
				v[x] &= v[y];
			} else if (n == 0x3) {
				// 8xy3 - XOR Vx, Vy: Set Vx = Vx XOR Vy
				v[x] ^= v[y];
			} else if (n == 0x4) {
				// 8xy4 - ADD Vx, Vy: Set Vx = Vx + Vy, set VF = carry
				uint8_t oldvx = v[x];
				v[x] += v[y];
				if (v[x] < oldvx)
					v[0xF] = 1;
				else
					v[0xF] = 0;
			} else if (n == 0x5) {
				// 8xy5 - SUB Vx, Vy: Set Vx = Vx - Vy, set VF = NOT borrow
				if (v[x] > v[y])
					v[0xF] = 1;
				else
					v[0xF] = 0;
				v[x] -= v[y];
			} else if (n == 0x6) {
				// 8x06 - SHR Vx {, Vy}: Set Vx = Vx SHL 1
				// Desplazar los bits de Vx a la derecha y poner en VF el de menos peso
				v[0xF] = v[x] & 0x1;
				v[x] = v[x] >> 1;
			} else if (n == 0x7) {
				// 8xy7 - SUBN Vx, Vy: Set Vx = Vy - Vx, set VF = NOT borrow
				if (v[y] > v[x])
					v[0xF] = 1;
				else
					v[0xF] = 0;
				v[x] = v[y] - v[x];
			} else if (n == 0xE) {
				// 8xyE - SHL Vx {, Vy}: Set Vx = Vx SHL 1
				// Desplazar los bits de Vx a la izquierda y poner en VF el de más peso
				v[0xF] = v[x] & 0x8000;
				v[x] = v[x] << 1;
			}
			break;
			
		// TODO resto de operaciones
		
	}
	
}
