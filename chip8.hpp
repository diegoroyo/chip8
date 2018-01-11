//*****************************************************************
// File:    chip8.hpp
// Author:  Diego Royo
// Date:    enero 2018
// Coms:    Intérprete de Chip8
//*****************************************************************

#ifndef _CHIP_8_
#define _CHIP_8_

class Chip8 {
	
	private:
	
		// Memoria
		uint8_t ram[0x1000]; // 4 KB de memoria RAM
		
		// Registros
		uint8_t v[16]; // Registros de Chip8 (v0 hasta vF)
		uint16_t i; // Index register
		uint8_t delayTimer, soundTimer; // Delay y Sound timers
		
		// Registros no accesibles desde programas
		uint16_t pc; // Program counter
		uint8_t sp; // Stack pointer
		uint16_t stack[16]; // Pila
		
		// Display
		const int W = 64; // Tamaño de la pantalla
		const int H = 32;
		uint8_t display[W * H / 8]; // 0: Pixel apagado / 1: Pixel encendido
		bool displayUpdated; // Cambio en la pantalla al ejecutar una instrucción
	
	public:
	
		// TODO constructor
	
		/* Ejecuta una instrucción */
		void Step();
		
		// TODO resto de funciones
	
};

#endif