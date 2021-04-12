#include <SDL.h>
#include <SDL_ttf.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

#include "SDL_Context.h"
#include "Text.h"

#define DEBUG 0

const int screen_w = 640;
const int screen_h = 320;

void LOG(std::string where_, std::string message) {
#if DEBUG
	std::cout << "[" << where_ << "]" << " " << message << "\n";
#endif // DEBUG

}

bool debug = false;

//helpers
int random_between(int mn, int mx) {
	if (mn == mx) return mn;
	int n = rand() % (mx - mn) + mn;
	return n;
}

float random_betweenf(float min, float max) {
	float scale = rand() / (float)RAND_MAX; // [0, 1.0] 
	return min + scale * (max - min);      // [min, max]
}
namespace C8 {
	unsigned char hx(unsigned char c);
}

struct Screen {
	Screen() : pixels{0}{
		texture = SDL_CreateTexture(SDL::Context::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screen_w, screen_h);
	}

	void draw() {
		SDL_UpdateTexture(texture, nullptr, pixels, 4 * screen_w);
		SDL_RenderCopy(SDL::Context::renderer, texture, NULL, NULL);
	}
	
	void sprite_normal(int x, int y, int n, int I, unsigned char* mem) {
		// drw         v0, v1, 1
		if (debug) printf("--------------draw---------------\n");
		if (debug) printf("drawing at: x:%d,y:%d,n:%d,i_%d\n", x, y, n, I);
		Uint32* p = &pixels[x + (screen_w * y)];
		if (debug) printf("pos %d\n", x + (screen_w * y));

		unsigned char* val = &mem[I];
		printf("I: %d setted to\n", I);
		printf("mem[i]: %d setted to\n", *val);

		for (int h = 0; h < n; h++) {
			unsigned char nn = *val;
			
			int ind = 7;
			for (Uint32 i = 0; i < 8; i++) {
				//printf("pixel%d: %d\n", i, (nn >> i) & 1);
				printf("%d", (nn >> i) & 1);
				if ((nn >> i) & 1) {
					*(p + ind) ^= 0xffffffff;
				}
				ind--;
			}
			printf("\n");
			p += screen_w;
			
			val++;
		}
		if (debug) printf("--------------draw---------------\n");
	}

	void clear() {
		for (int i = 0; i < screen_w * screen_h; i++) {
			pixels[i] = 0;
		}
	}

	void sprite_zoomed(int x, int y, int n, int I, unsigned char* mem, int scale_factor = 10) {
		if (debug) printf("--------------draw---------------\n");
		if (debug) printf("drawing at: x:%d,y:%d,n:%d,i_%d\n", x, y, n, I);
		int scaled_x = x * scale_factor;
		int scaled_y = y * scale_factor;
		
		if (debug) printf("drawing at scaled: x:%d,y:%d,n:%d,i_%d\n", scaled_x, scaled_y, n, I);
		int pos = scaled_x + (screen_w * scaled_y);
		if (debug) printf("pos %d\n", pos);

		//Uint32* p = &pixels[];
		unsigned char* val = &mem[I];
		if (debug) printf("I: %d setted to\n", I);
		if (debug) printf("mem[i]: %d setted to\n", *val);

		
		auto draw_10x10 = [&](int p) {
			for (int i = 0; i < scale_factor; i++) {
				for (int j = 0; j < scale_factor; j++) {
					pixels[p] ^= 0xFFFFFFFF;
					p += 1;
				}
				p += screen_w;
				p -= scale_factor;
			}
		};

		for (int h = 0; h < n; h++) {
			unsigned char nn = *val;
			//printf("will draw %d\n", nn);
			int ind = 7*scale_factor;
			
			for (Uint32 i = 0; i < 8; i++) {
				if (debug) printf("%d", (nn >> i) & 1);
				if ((nn >> i) & 1) {
					draw_10x10(pos+ind);
				}
				pos += scale_factor;
				ind -= scale_factor*2;
			}
			
			if (debug) printf("\n");

			pos += (screen_w * scale_factor) - scale_factor * 8;
			val++;
		}
		if (debug) printf("--------------draw---------------\n");
	}

	SDL_Texture* texture = nullptr;
	Uint32 pixels[screen_w*screen_h];

};

namespace C8 {
	unsigned char hx(unsigned char c) {
		if (c == '0') return 0x0;
		if (c == '1') return 0x1;
		if (c == '2') return 0x2;
		if (c == '3') return 0x3;
		if (c == '4') return 0x4;
		if (c == '5') return 0x5;
		if (c == '6') return 0x6;
		if (c == '7') return 0x7;
		if (c == '8') return 0x8;
		if (c == '9') return 0x9;
		if (c == 'a') return 0xA;
		if (c == 'b') return 0xB;
		if (c == 'c') return 0xC;
		if (c == 'd') return 0xD;
		if (c == 'e') return 0xE;
		if (c == 'f') return 0xF;
		return 'x';
	}

	unsigned char xh(unsigned char c) {
		if (c == 0x0) return '0';
		if (c == 0x1) return '1';
		if (c == 0x2) return '2';
		if (c == 0x3) return '3';
		if (c == 0x4) return '4';
		if (c == 0x5) return '5';
		if (c == 0x6) return '6';
		if (c == 0x7) return '7';
		if (c == 0x8) return '8';
		if (c == 0x9) return '9';
		if (c == 0xA) return 'a';
		if (c == 0xB) return 'b';
		if (c == 0xC) return 'c';
		if (c == 0xD) return 'd';
		if (c == 0xE) return 'e';
		if (c == 0xF) return 'f'; 
		return 'x';
	}

	unsigned char random_uchar() {
		unsigned char n = (unsigned char)rand() % (255);
		return n;
	}
	
	struct Chip8
	{
		Chip8() : pc_text(" ",screen_w-60,0,30),
			registers_text(" ", 0, screen_h-20, 20),
			current_instruction(" ", screen_w - 100, 40, 30)
		{
			unsigned char chars[] = {
				0xF0, 0x90, 0x90, 0x90, 0xF0, 0x20, 0x60, 0x20, 0x20, 0x70, 
				0xF0, 0x10, 0xF0, 0x80, 0xF0, 0xF0, 0x10, 0xF0, 0x10, 0xF0, 
				0x90, 0x90, 0xF0, 0x10, 0x10, 0xF0, 0x80, 0xF0, 0x10, 0xF0, 
				0xF0, 0x80, 0xF0, 0x90, 0xF0, 0xF0, 0x10, 0x20, 0x40, 0x40, 
				0xF0, 0x90, 0xF0, 0x90, 0xF0, 0xF0, 0x90, 0xF0, 0x10, 0xF0, 
				0xF0, 0x90, 0xF0, 0x90, 0x90, 0xE0, 0x90, 0xE0, 0x90, 0xE0, 
				0xF0, 0x80, 0x80, 0x80, 0xF0, 0xE0, 0x90, 0x90, 0x90, 0xE0, 
				0xF0, 0x80, 0xF0, 0x80, 0xF0, 0xF0, 0x80, 0xF0, 0x80, 0x80
			};

			for (int i = 0; i < sizeof(chars); i++) {
				memory[i+1] = chars[i];
			}
		}

		void load_rom(std::string rom_path) {
			std::ifstream input(rom_path, std::ios::binary);
			std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});
			
			int pos = 0x200;
			
			std::vector<unsigned char> b2;
			for (auto c : buffer) {
				if (c != ' ' && c != 13 && c != 10) {
					b2.push_back(c);
				}
			}
			
			for(int i = 1; i <= b2.size(); i+=2){
				unsigned char c1 = b2[i-1];
				unsigned char c2 = b2[i];
				unsigned char nn = (C8::hx(c1) << 4) + C8::hx(c2);
				memory[pos] = nn;
				pos++;
			}

			if (debug) {
				printf("buffer:\n");
				for (auto c : buffer) {
					printf("%c", c);
				}
				printf("\nMemory:\n");
				for (int i = 0x200; i < 0x200+buffer.size(); i++) {
					//printf("%c", memory[i]);
				}
				printf("\n");
			}
		}

		void print_registers() {
			int i = 0;
			for (auto r : V) {
				std::cout << "V" << i << ": " << (unsigned int)r << "\n";
				i++;
			}
			std::cout << "I" << I << "\n";

		}

		void print_memory(int from = 0, int to = 0x1000, bool print_zeroes = false) {
			for (int i = from; i < to; i++) {
				if (print_zeroes) {
					printf("[0x%d]%c%c\n", i, xh(memory[i]>>4), xh((memory[i]&0xf)));
				}
				else {
					if (memory[i]) {
						printf("[0x%d]%d\n", i, memory[i]);
					}
				}
			}
		}

		unsigned char current_ins[2] = { 0 };
		unsigned char current_decoded_ins[4] = {0};
		void fetch() {
			current_ins[0] = memory[PC];
			current_ins[1] = memory[PC+1];
			PC+=2;
		}
		void decode() {
			current_decoded_ins[0] = current_ins[0] >> 4;
			current_decoded_ins[1] = current_ins[0] & 0xf;
			current_decoded_ins[2] = current_ins[1] >> 4;
			current_decoded_ins[3] = current_ins[1] & 0xf;
		}

		void execute() {
			unsigned cod = current_decoded_ins[0];
			
			unsigned char x = current_decoded_ins[1];
			unsigned char y = current_decoded_ins[2];
			unsigned char n = current_decoded_ins[3];
			unsigned char nn = current_decoded_ins[2] * 16 + current_decoded_ins[3];
			unsigned short nnn = current_decoded_ins[1] * 256 + current_decoded_ins[2] * 16 + current_decoded_ins[3];

			if (debug) {
				printf("ins: %c%c%c%c\n", xh(current_decoded_ins[0]), xh(current_decoded_ins[1]), xh(current_decoded_ins[2]), xh(current_decoded_ins[3]));
				printf("cod: %d\n", cod);
				printf("x: %d\n", x);
				printf("y : %d\n", y);
				printf("n : %d\n", n);
				printf("nn: %d\n", nn);
				printf("nnn: %d\n", nnn);
			}

			auto unimplemented = [&]() {
				printf("UNIMPLEMENTED INSTRUCTION %c%c%c%c\n", xh(current_decoded_ins[0]), xh(current_decoded_ins[1]), xh(current_decoded_ins[2]), xh(current_decoded_ins[3]));
			};

			if (cod == 0x0) {
				if (nn == 0xe0) {
					screen.clear();
				}
				if (nn == 0xee) {
					PC = stack.back();
					stack.pop_back();
				}
				else {
					unimplemented();
				}
			}
			if (cod == 0x1) {
				PC = nnn;
			}
			if (cod == 0x2) {
				stack.push_back(PC);
				PC = nnn;
			}
			else if(cod == 0x3){
				if (V[x] == nn) {
					PC += 2;
				}
			}
			else if (cod == 0x4) {
				if (V[x] != nn) {
					PC += 2;
				}
			}
			else if (cod == 0x5) {
				if (V[x] == V[y]) {
					PC += 2;
				}
			}
			else if(cod == 0x6){
				if(debug) printf("v[%d] setted to %d\n", x, nn);
				V[x] = nn;
			}
			else if(cod == 0x7){
				if (debug) printf("v[%d] += %d == ", x, nn);
				V[x] += nn;
				if (debug) printf("%d\n", V[x]);
			}
			else if (cod == 0x8) {
				int signed_x = V[x];
				int signed_y = V[y];
				if (n == 0x0) {
					V[x] = V[y];
				}
				if (n == 0x1) {
					V[x] |= V[y];
				}
				if (n == 0x2) {
					V[x] &= V[y];
				}
				if (n == 0x3) {
					V[x] ^= V[y];
				}
				if (n == 0x4) {
					if (signed_x + signed_y > 255) {
						V[0xf] = 1;
					}
					else {
						V[0xf] = 0;
					}

					V[x] += V[y];
					//V[0xf] = 0;
					//V[0xf] = 1;
				}
				if (n == 0x5) {
					if (signed_x - signed_y < 0) {
						V[0xf] = 1;
					}
					else {
						V[0xf] = 0;
					}

					V[x] -= V[y];
				}
				if (n == 0x6) {
					V[0xf] = V[x]&1;
					V[x] >>= 1;
				}
				if (n == 0x7) {
					if (signed_y - signed_x < 0) {
						V[0xf] = 1;
					}
					else {
						V[0xf] = 0;
					}

					V[x] = V[y] - V[x];
				}
				if (n == 0xe) {
					V[x] <<= 1;
				}
			}
			else if(cod == 0xa){
				I = nnn;
			}
			else if(cod == 0xc){
				V[x] = random_uchar() & nn;
				
			}
			else if(cod == 0xd){
				set_pixel(V[x], V[y], n);
			}
			else if (cod == 0xe) {
				if (nn == 0x9e) {
					if (keys[V[x]]) {
						PC += 2;
					}
				}
				else if (nn == 0xa1) {
					if (!keys[V[x]]) {
						PC += 2;
					}
				}
				else {
					unimplemented();
				}
			}
			else if (cod == 0xf) {
				if (nn == 0x7) {
					V[x] = DelayTimer;
				}
				else if (nn == 0x15) {
					DelayTimer = V[x];
					//printf("Delay: %d", DelayTimer);
				}

				else if (nn == 0xa) {
					PC-=2;
					if (keys[V[x]]) {
						PC += 2;
					}
				}
				else if (nn == 0x29) {
					I = (V[x]*5)+1;
				}
				else if (nn == 0x33) {
					unsigned char u = V[x];
					unsigned char c = 0;
					unsigned char d = 0;
					if (V[x] > 10) {
						u = V[x] % (V[x] / 10);
						c = (V[x] - u) / 100;
						d = (V[x] - u - (c * 100)) / 10;
					}

					memory[I] = c;
					memory[I+1] = d;
					memory[I+2] = u;
				}
				else if (nn == 0x55) {
					unsigned short i_bu = I;
					printf("reg dump(%d,&%d)\n", x, I);
					for (unsigned char i = 0; i <= x; i++) {
						memory[i_bu] = V[i];
						i_bu++;
					}
				}

				else if (nn == 0x65) {
					unsigned short i_bu = I;
					printf("reg load(%d,&%d)\n", x, I);
					for (unsigned char i = 0; i <= x; i++) {
						V[i] = memory[i_bu];
						i_bu++;
					}
				}
				else {
					printf("UNIMPLEMENTED INSTRUCTION %c%c%c%c\n", xh(current_decoded_ins[0]), xh(current_decoded_ins[1]), xh(current_decoded_ins[2]), xh(current_decoded_ins[3]));				
				}
			}
		}

		void handle_input() {
			kbstate = SDL_GetKeyboardState(NULL);
			
			auto mirror_pressed = [&](Uint8 key, int c8_key){
				if (kbstate[key]) {
					keys[c8_key] = 1;
				}
				else {
					keys[c8_key] = 0;
				}
			};
			
			//1 	2 	3 	C
			//4 	5 	6 	D
			//7 	8 	9 	E
			//A 	0 	B 	F

			mirror_pressed(SDL_SCANCODE_1, 1);
			mirror_pressed(SDL_SCANCODE_2, 2);
			mirror_pressed(SDL_SCANCODE_3, 3);
			mirror_pressed(SDL_SCANCODE_4, 0xc);

			mirror_pressed(SDL_SCANCODE_Q, 4);
			mirror_pressed(SDL_SCANCODE_W, 5);
			mirror_pressed(SDL_SCANCODE_E, 6);
			mirror_pressed(SDL_SCANCODE_R, 0xd);

			mirror_pressed(SDL_SCANCODE_A, 7);
			mirror_pressed(SDL_SCANCODE_S, 8);
			mirror_pressed(SDL_SCANCODE_D, 9);
			mirror_pressed(SDL_SCANCODE_F, 0xe);

			mirror_pressed(SDL_SCANCODE_Z, 0xa);
			mirror_pressed(SDL_SCANCODE_X, 0xb);
			mirror_pressed(SDL_SCANCODE_C, 0xc);
			mirror_pressed(SDL_SCANCODE_V, 0xf);
		}

		void cycle() {
			handle_input();

			fetch();
			decode();
			execute();
			if (DelayTimer > 0) {
				DelayTimer--;
			}
		}

		void run() {
			//start reading instructions in memory from 0x100
			PC = 0x200;

			//while (PC != 4096) {
			//	
			//	if (PC > 4096) break;
			//
			//
			//}

		}

		void print_and_draw() {
			std::stringstream program_counter;
			program_counter << "PC: " << PC;
			pc_text.update_w(program_counter.str());

			std::stringstream registers;
			for (int i = 0; i < 16; i++) {
				registers << "V" << i << ": " << (int)V[i] << " ";
			}
			registers_text.update_w(registers.str());

			std::stringstream inst;
			inst << "CInst: " << xh(current_decoded_ins[0]) << xh(current_decoded_ins[1]) << xh(current_decoded_ins[2]) << xh(current_decoded_ins[3]);
			current_instruction.update_w(inst.str());

			pc_text.render();
			registers_text.render();
			current_instruction.render();
		}

		void set_pixel(unsigned char x, unsigned char y, unsigned char n) {
			screen.sprite_zoomed(x, y, n, I, memory);
		}

		unsigned char V[16]{ 0 }, DelayTimer{ 0 }, SoundTimer{0}, SP, keys[16];
		unsigned char memory[4096] = { 0 };
		unsigned short PC{ 0 }, I{ 0 };
		
		std::vector<unsigned short> stack;
		Screen screen;
		const Uint8* kbstate = nullptr;
	
		Text pc_text;
		Text registers_text;
		Text current_instruction;

		bool rom_2b = false;

	};
}

int main(int argc, char* args[])
{
	(void)argc;
	(void)args;
	//using namespace SDL;
	SDL::Context sdl_context("Test SDL");

	bool running = true;


	Uint32 startFTime = SDL_GetTicks();
	float fps = (1.0f / 55.0f) * 1000.0f;

	C8::Chip8 c8;
	c8.rom_2b = true;
	c8.load_rom("pong.bin");
	c8.run();
	c8.print_memory(0x1, 0x100, true);
	c8.print_registers();
	
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


	bool auto_cycling = false;
	int mhz = 1;

	while (running) {

		SDL_RenderClear(SDL::Context::renderer);

		const Uint8* kbstate = SDL_GetKeyboardState(NULL);
		//if (kbstate[SDL_SCANCODE_Q]) {
		//	running = false;
		//}

		if (kbstate[SDL_SCANCODE_RIGHT]) {}
		if (kbstate[SDL_SCANCODE_SPACE]) {
			//rand_pixels();
		}

		if (kbstate[SDL_SCANCODE_A]) {
			auto_cycling = true;
		}
		if (kbstate[SDL_SCANCODE_N]) {
			auto_cycling = false;
		}

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
			if (SDL::Context::event.type == SDL_MOUSEMOTION) {

			}
			if (SDL::Context::event.type == SDL_KEYDOWN) {
				if (SDL::Context::event.key.keysym.sym == SDLK_RIGHT) {

				}
			}
			if (SDL::Context::event.type == SDL_KEYUP) {
				switch (SDL::Context::event.key.keysym.sym) {
				case SDLK_UP:
				case SDLK_DOWN:
					break;
				}
			}
		}

		if (auto_cycling) {
			for (int i = 0; i < mhz; i++) {
				c8.cycle();
			}
		}
		c8.screen.draw();
		c8.print_and_draw();

		SDL_SetRenderDrawColor(SDL::Context::renderer, 255, 255, 255, 255);
		SDL_RenderPresent(SDL::Context::renderer);

		int delay = (int)fps - (SDL_GetTicks() - startFTime);
		
		if (delay > 0) SDL_Delay(delay);

		startFTime = SDL_GetTicks();
	}

	return 0;

}