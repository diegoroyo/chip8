//*****************************************************************
// File:    chip8.hpp
// Author:  Diego Royo
// Date:    febrero 2018
// Coms:    CPU para el intérprete de Chip8
//*****************************************************************

#ifndef _CHIP_8_
#define _CHIP_8_

#include <fstream>
#include <cstring>
#include <cstdint>

const int RAM_MAX = 0x1000; // 4 KB
const int W = 64; // Tamaño de la pantalla
const int H = 32;
const int DISP_MAX = W * H / 8;
const int FONT_START = 0x100; // Donde empieza la fuente
const uint8_t FONT[16 * 5] { // Del 0 a la F, cada una representada por 5 bytes
	0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
	0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
	0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
	0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
	0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
	0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
	0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
	0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
	0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
	0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
	0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
	0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
	0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
	0xE0, 0x90, 0x90, 0x90, 0xE0, /* D */
	0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
	0xF0, 0x80, 0xF0, 0x80, 0x80  /* F */
};

class Chip8 {
	
	private:
	
		// Memoria
		uint8_t ram[RAM_MAX];
		
		// Registros
		uint8_t V[16]; // Registros de Chip8 (v0 hasta vF)
		uint16_t I; // Index register
		uint8_t delayTimer, soundTimer; // Delay y Sound timers
		
		// Registros no accesibles desde programas
		uint16_t pc; // Program counter
		uint8_t sp; // Stack pointer
		uint16_t stack[16]; // Pila
	
	public:
		
		// Display y teclado
		bool displayUpdated; // Si se ha actualizado el display (para dibujarlo)
		uint8_t display[DISP_MAX]; // 0: Pixel apagado / 1: Pixel encendido
		bool waitingForKey; // Si esta esperando a que el usuario pulse una tecla
		uint16_t keys; // Teclas pulsadas (y no tratadas)
	
		/* Constructor */
		Chip8();
		
		/* Carga un programa en ram almacenado en start con len instrucciones a partir de pos */
		/* Devuelve true si se ha cargado correctamente, false en caso contrario */
		bool loadProgram(uint16_t* start, int len, int pos);
		
		/* Carga un programa en ram almacenado en un fichero con nombre name a partir de pos */
		/* Devuelve true si se ha cargado correctamente, false en caso contrario */
		bool loadProgram(char* name, int pos = 0x200);
	
		/* Ejecuta una instrucción */
		void step();
		
		/* Pasa a pixels el contenido de la pantalla con formato ARGB (8 bits cada uno) */
		void renderTo(uint32_t* pixels);
		
		// Funciones de prueba
		std::string getDisplayState();
		
		std::string getRegistersState();
	
};

#endif