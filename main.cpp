#include <SDL.h>
#include <SDL_ttf.h>

#include "SDL_Context.h"
#include "Text.h"

#include "Chip8.h"

#include <string>
#include <iostream>
#include <filesystem>

#define DEBUG 0

const int screen_w = SDL::Context::screen_w;
const int screen_h = SDL::Context::screen_h;

bool debug = false;

int main(int argc, char* args[])
{
	(void)argc;
	(void)args;

	SDL::Context sdl_context("CHIP-8");

	Uint32 startFTime = SDL_GetTicks();
	float fps = (1.0f / 55.0f) * 1000.0f;
	bool running = true;

	C8::Chip8 c8(10);
	c8.load_rom("roms/logo.bin");
	c8.print_memory(0x1, 0x100, true);
	c8.print_registers();
	
	bool auto_cycling = true;
	int mhz = 1;

	Uint8 pressed_keys[255] = { 0 };
	auto if_key_pressed = [&](Uint8 key, const Uint8* kbstate, auto&& f) {
		if (kbstate[key]) {
			if (!pressed_keys[key]) {
				pressed_keys[key] = true;
				f();
			}
		}
		else {
			pressed_keys[key] = false;
		}

	};

	while (running) {

		SDL_RenderClear(SDL::Context::renderer);

		const Uint8* kbstate = SDL_GetKeyboardState(NULL);
		if (kbstate[SDL_SCANCODE_ESCAPE]) {
			running = false;
		}

		if (kbstate[SDL_SCANCODE_A]) {
			auto_cycling = true;
		}
		if (kbstate[SDL_SCANCODE_N]) {
			auto_cycling = false;
		}

		if_key_pressed(SDL_SCANCODE_R, kbstate, [&]() {
			c8.reset();
			auto_cycling = false;
		});

		if_key_pressed(SDL_SCANCODE_LEFT, kbstate, [&]() {
			});

		if_key_pressed(SDL_SCANCODE_LEFT, kbstate, [&]() {
			});


		if_key_pressed(SDL_SCANCODE_UP, kbstate, [&]() {
			if (auto_cycling) {
				mhz++;
			}
			else {
				c8.cycle();
			}
			});

		if_key_pressed(SDL_SCANCODE_DOWN, kbstate, [&]() {
			if (auto_cycling) {
				mhz--;
			}
			});


		if_key_pressed(SDL_SCANCODE_P, kbstate, [&]() {
			c8.print_memory(0x100, 0x200, true);
		});


		while (SDL_PollEvent(&SDL::Context::event)) {
			if (SDL::Context::event.type == SDL_QUIT) {
				running = false;
			}
			if (SDL::Context::event.type == SDL_KEYDOWN) {
				if (SDL::Context::event.key.keysym.sym == SDLK_RIGHT) {}
			}
			if (SDL::Context::event.type == SDL_KEYUP) {
				switch (SDL::Context::event.key.keysym.sym) {
				case SDLK_UP:
				case SDLK_DOWN:
					break;
				}
			}
			if (SDL::Context::event.type == SDL_DROPFILE){
				const char* dropped_filedir = SDL::Context::event.drop.file;
				c8.reset();
				c8.load_rom(dropped_filedir);
				auto_cycling = true;
			}
		}

		if (auto_cycling) {
			for (int i = 0; i < mhz; i++) {
				c8.cycle();
			}
		}
		c8.screen.draw();
		c8.print_and_draw();

		SDL_SetRenderDrawColor(SDL::Context::renderer, 0x69, 0x77, 0x9b, 255);

		SDL_RenderPresent(SDL::Context::renderer);

		int delay = (int)fps - (SDL_GetTicks() - startFTime);
		
		if (delay > 0) SDL_Delay(delay);

		startFTime = SDL_GetTicks();
	}

	return 0;

}