//*****************************************************************
// File:    chip8.cpp
// Author:  Diego Royo
// Date:    febrero 2018
//*****************************************************************

#include <chip8.hpp>

Chip8::Chip8() {
	// RAM
	memset(ram, 0, sizeof(ram));
	ram[0] = 0x12; // La primera instrucción es JP 0x200 ya que ahi empiezan los programas
	
	// V, I, delayTimer, soundTimer
	memset(v, 0, sizeof(v));
	i = 0;
	delayTimer = 0;
	soundTimer = 0;
	
	// PC, SP y pila
	pc = 0;
	sp = 0;
	memset(stack, 0, sizeof(stack));
	
	// Display y teclas pulsadas
	memset(display, 0, sizeof(display));
	keys = 0;
}

bool Chip8::loadProgram(uint16_t *start, int len, int pos) {
	int i = 0;
	while (i < len && pos < RAM_MAX - 1) {
		ram[pos] = start[i] >> 8;
		ram[pos + 1] = start[i] & 0xFF;
		pos += 2;
		i++;
	}
	return true;
}

bool Chip8::loadProgram(std::string name, int pos) {
	std::ifstream file(name);
	if (file.is_open()) {
		while (!file.eof() && pos < RAM_MAX) {
			ram[pos] = file.get();
			pos++;
		}
		return true;
	} else {
		return false;
	}
}

void Chip8::step() {
	
	// Leer la instrucción y obtener los valores
	uint16_t opcode = ram[pc] << 8 | ram[pc + 1];
	pc += 2;
	
	uint16_t nnn = opcode & 0xFFF; 		// 12 bits de menor peso (addr)
	uint8_t n = opcode & 0xF; 			// 4 bits de menor peso (nibble)
	uint8_t x = (opcode >> 8) & 0xF; 	// Bits 8 a 11
	uint8_t y = (opcode >> 4) & 0xF; 	// Bits 4 a 7
	uint8_t kk = opcode & 0xFF; 		// 8 bits de menor peso (byte)
	
	bool displayUpdated = false;
	
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
		case 0x9000:
			// 9xy0 - SNE Vx, Vy: Skip next instruction if Vx != Vy
			if (v[x] != v[y]) pc += 2;
			break;
		case 0xA000:
			// Annn - LD I, addr: Set I = nnn
			i = nnn;
			break;
		case 0xB000:
			// Bnnn - JP V0, addr: Jump to location nnn + V0
			pc = v[0] + nnn;
			break;
		case 0xC000:
			// Cxkk - RND Vx, byte: Set Vx = random byte AND kk
			v[x] = (rand() % 256) & kk;
			break;
		case 0xD000: {
			// Dxny - DRW Vx, Vy, nibble: Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
			v[0xF] = 0;
			int offset = v[x] % 8;
			int dpos = v[y] * W / 8 + v[x]; // Posicion en el display a dibujar
			uint8_t draw = 0;
			for (int j = 0; j < n; j++) {
				// Comprobar que vamos a dibujar dentro del display
				dpos = dpos % DISP_MAX;
				// Dibujar la primera parte
				draw = ram[i + j] >> offset;
				if ((display[dpos] >> offset) & draw) {
					v[0xF] = 1; // colision
				}
				display[dpos] ^= draw;
				// Si no se dibuja en una posicion multiplo de 8 hay que hacerlo en 2 partes
				if (offset > 0) {
					int nextpos = dpos + 1; // nueva posicion (byte de la derecha)
					if (nextpos % (W / 8) == 0) {
						nextpos -= W / 8; // se sale por la derecha
					}
					draw = ram[i + j] << (8 - offset);
					if ((display[nextpos] << (8 - offset)) & draw) {
						v[0xF] = 1; // colision
					}
					display[nextpos] ^= draw;
				}
				// Calcular nueva posicion para dibujar
				dpos += W / 8;
			}
		}	break;
		case 0xE000:
			if (kk == 0x9E) {
				// Ex9E - SKP Vx - Skip next instruction if key with the value of Vx is pressed
				if (keys & (1 << v[x])) pc += 2;
			} else if (kk == 0xA1) {
				// ExA1 - SKNP Vx - Skip next instruction if key with the value of Vx is not pressed
				if (!(keys & (1 << v[x]))) pc += 2;
			}
			break;
		case 0xF000:
			if (kk == 0x07) {
				// Fx07 - LD Vx, DT - Set Vx = delay timer value
				v[x] = delayTimer;
			} else if (kk == 0x0A) {
				// Fx0A - LD Vx, K - Wait for a key press, store the value of the key in Vx
				if (keys) {
					for (int j = 0; j < 0xF; j++) {
						 if (keys & (1 << j)) {
							 v[x] = j;
						 }
					}
					keys = 0;
				} else {
					pc -= 2; // Ninguna tecla pulsada, comprobar luego
				}
				
			} else if (kk == 0x15) {
				// Fx15 - LD DT, Vx: Set delay timer = Vx
				delayTimer = v[x];
			} else if (kk == 0x18) {
				// Fx18 - LD ST, Vx: Set sound timer = Vx
				soundTimer = v[x];
			} else if (kk == 0x1E) {
				// Fx1E - ADD I, Vx: Set I = I + Vx
				i += v[x];
			} else if (kk == 0x29) {
				// Fx29 - LD F, Vx: Set I = location of sprite for digit Vx
				i = v[x] * 5;
			} else if (kk == 0x33) {
				// Fx33 - LD B, Vx: Store BCD representation of Vx in memory locations I, I+1 and I+2
				ram[i] = v[x] / 100;
				ram[i + 1] = (v[x] / 10) % 10;
				ram[i + 2] = v[x] % 10;
			} else if (kk == 0x55) {
				// Fx55 - LD [I], Vx: Store registers V0 through Vx in memory starting at location I
				if (x + i > RAM_MAX) x = RAM_MAX - i;
				uint8_t *p = &ram[i];
				for (int j = 0; j < x; p++, j++) *p = v[j];
			} else if (kk == 0x65) {
				// Fx65 - LD Vx, [I]: Read registers V0 through Vx from memory starting at location I
				if (x + i > RAM_MAX) x = RAM_MAX - i;
				uint8_t *p = &ram[i];
				for (int j = 0; j < x; p++, j++) v[j] = *p;
			}
			break;
	}
	
}

std::string Chip8::getDisplayState() {
	// TODO mejorar el funcionamiento si decido dejar esto
	std::string res = "";
	for (int y = 0; y < H; y++) {
		for (int x = 0; x < W / 8; x++) {
			for (int b = 0; b < 8; b++) {
				if (display[y * W / 8 + x] & (1 << b)) {
					res += "X";
				} else {
					res += ".";
				}
			}
		}
		res += "\n";
	}
	return res;
}

std::string Chip8::getRegistersState() {
	// TODO mejorar el funcionamiento si decido dejar esto
	// TODO añadir el resto de registros
	std::string res = "";
	// V
	for (int j = 0; j < 16; j++) {
		char bufferv[50];
		sprintf(bufferv, "V%X=0x%X", j, v[j]);
		res += bufferv;
		if (j != 15) res += " | ";
	}
	res += "\n";
	// I
	char bufferi[50];
	sprintf(bufferi, "I=%d (ram[I] = %x)\n", i, ram[i]);
	res += bufferi;
	// PC
	char bufferpc[50];
	sprintf(bufferpc, "PC=0x%X (instruccion = 0x%X)\n", pc, ram[pc] << 8 | ram[pc + 1]);
	res += bufferpc;
	
	return res;
}
