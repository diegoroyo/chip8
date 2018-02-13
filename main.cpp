#include <chip8.hpp>
#include <iostream>
#include <cstdint>
#include <thread>
#include <chrono>

#include <SDL2/SDL.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CPU_FREQ = 60; // Instrucciones por segundo

uint16_t getMappedKey(SDL_Keycode k) {
	switch (k) {
		case SDLK_KP_0: return 0x0001; /* 0 */
		case SDLK_KP_1: return 0x0002; /* 1 */
		case SDLK_KP_2: return 0x0004; /* 2 */
		case SDLK_KP_3: return 0x0008; /* 3 */
		case SDLK_KP_4: return 0x0010; /* 4 */
		case SDLK_KP_5: return 0x0020; /* 5 */
		case SDLK_KP_6: return 0x0040; /* 6 */
		case SDLK_KP_7: return 0x0080; /* 7 */
		case SDLK_KP_8: return 0x0100; /* 8 */
		case SDLK_KP_9: return 0x0200; /* 9 */
		case SDLK_KP_PERIOD: return 0x0400; /* A */
		case SDLK_KP_DIVIDE: return 0x0800; /* B */
		case SDLK_KP_MULTIPLY: return 0x1000; /* C */
		case SDLK_KP_MINUS: return 0x2000; /* D */
		case SDLK_KP_PLUS: return 0x4000; /* E */
		case SDLK_KP_ENTER: return 0x8000; /* F */
	}
}

int main(int argc, char *argv[]) {
	
	if (argc != 2) {
		std::cout << "/chip8 <program_name>" << std::endl;
		return 1;
	}
	
	// CPU
	Chip8 chip8;
	chip8.loadProgram(argv[1]);
	
	// Crear la pantalla donde van los graficos
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("Chip8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
										  SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, W,H);
	
	bool end = false;
	
	while (!end) {
		
		// Ejecutar instrucciones hasta que haya que:
		// - Actualizar la pantalla
		// - Leer de teclado
		// - Se hayan ejecutado 60 instrucciones seguidas
		int i = 0;
		while (i < CPU_FREQ && !chip8.displayUpdated && !chip8.waitingForKey) {
			chip8.step();
			i++;
			SDL_Delay(1000/CPU_FREQ);
		}
		
		// Actualizar pantalla
		if (chip8.displayUpdated) {
			uint32_t pixels[W*H];
			chip8.renderTo(pixels);
			SDL_UpdateTexture(texture, nullptr, pixels, 4*W);
			SDL_RenderCopy(renderer, texture, nullptr, nullptr);
			SDL_RenderPresent(renderer);
			chip8.displayUpdated = false;
		}
		
		// Leer todos los eventos de teclado (da igual si esta esperando tecla o no)
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				end = true;
			} else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					end = true;
				} else {
					chip8.keys ^= getMappedKey(event.key.keysym.sym);
				}
			}
		}
		
	}
	
	SDL_Quit();
}