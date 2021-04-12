#pragma once

#include "Screen.h"
#include "Text.h"
#include <random>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <vector>

namespace C8 {
	const int screen_w = SDL::Context::screen_w;
	const int screen_h = SDL::Context::screen_h;

	unsigned char hx(unsigned char c);
	unsigned char xh(unsigned char c);
	unsigned char random_uchar();

	struct Chip8
	{
		Chip8(int scale);

		void load_rom(std::string rom_path);
		void print_registers();
		void print_memory(int from, int to, bool print_zeroes);

		void fetch();
		void decode();
		void execute();

		void handle_input();
		void cycle();
		void print_and_draw();

		void set_pixel(unsigned char x, unsigned char y, unsigned char n);

		unsigned char V[16]{ 0 }, DelayTimer{ 0 }, SoundTimer{ 0 }, SP, keys[16];
		unsigned char memory[4096] = { 0 };
		unsigned short PC{ 0 }, I{ 0 };

		unsigned char current_ins[2] = { 0 };
		unsigned char current_decoded_ins[4] = { 0 };

		std::vector<unsigned short> stack;
		Screen screen;
		const Uint8* kbstate = nullptr;

		Text pc_text;
		Text registers_text1;
		Text registers_text2;
		Text current_instruction;
	};
}
