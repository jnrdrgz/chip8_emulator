#include "Chip8.h"

unsigned char C8::hx(unsigned char c) {
	if (c > 58) c -= 39;
	return c - 48;

}

unsigned char C8::xh(unsigned char c) {
	if (c > 9) c += 39;
	return c + 48;
}

unsigned char C8::random_uchar() {
	unsigned char n = (unsigned char)rand() % (255);
	return n;
}

C8::Chip8::Chip8(int scale = 10) : pc_text(" ", screen_w - 90, 0, 30),
	registers_text1(" ", 0, screen_h - 50, 20),
	registers_text2(" ", 0, screen_h - 20, 20),
	current_instruction(" ", screen_w - 110, 40, 30),
	screen{ scale }
{
	load_chars_in_mem();
}

void C8::Chip8::load_chars_in_mem() {
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
		memory[i + 1] = chars[i];
	}
}

void C8::Chip8::load_rom(std::string rom_path) {
	std::ifstream input(rom_path, std::ios::binary);
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

	PC = 0x200;
	int pos = 0x200;

	std::vector<unsigned char> b2;
	for (auto c : buffer) {
		if (c != ' ' && c != 13 && c != 10) {
			b2.push_back(c);
		}
	}

	for (int i = 1; i <= b2.size(); i += 2) {
		unsigned char c1 = b2[i - 1];
		unsigned char c2 = b2[i];
		unsigned char nn = (C8::hx(c1) << 4) + C8::hx(c2);
		memory[pos] = nn;
		pos++;
	}

	int debug = 0;
	if (debug) {
		printf("buffer:\n");
		for (auto c : buffer) {
			printf("%c", c);
		}
		printf("\nMemory:\n");
		for (int i = 0x200; i < 0x200 + buffer.size(); i++) {
			//printf("%c", memory[i]);
		}
		printf("\n");
	}
}

void C8::Chip8::print_registers() {
	int i = 0;
	for (auto r : V) {
		std::cout << "V" << i << ": " << (unsigned int)r << "\n";
		i++;
	}
	std::cout << "I" << I << "\n";

}

void C8::Chip8::print_memory(int from = 0, int to = 0x1000, bool print_zeroes = false) {
	for (int i = from; i < to; i++) {
		if (print_zeroes) {
			printf("[0x%d]%c%c\n", i, xh(memory[i] >> 4), xh((memory[i] & 0xf)));
		}
		else {
			if (memory[i]) {
				printf("[0x%d]%d\n", i, memory[i]);
			}
		}
	}
}

void C8::Chip8::fetch() {
	current_ins[0] = memory[PC];
	current_ins[1] = memory[PC + 1];
	PC += 2;
}
void C8::Chip8::decode() {
	current_decoded_ins[0] = current_ins[0] >> 4;
	current_decoded_ins[1] = current_ins[0] & 0xf;
	current_decoded_ins[2] = current_ins[1] >> 4;
	current_decoded_ins[3] = current_ins[1] & 0xf;
}

void C8::Chip8::execute() {
	unsigned cod = current_decoded_ins[0];

	unsigned char x = current_decoded_ins[1];
	unsigned char y = current_decoded_ins[2];
	unsigned char n = current_decoded_ins[3];
	unsigned char nn = current_decoded_ins[2] * 16 + current_decoded_ins[3];
	unsigned short nnn = current_decoded_ins[1] * 256 + current_decoded_ins[2] * 16 + current_decoded_ins[3];

	int debug = 0;
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
		printf("excecuting COD::::%d\n", cod);
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
	else if (cod == 0x1) {
		PC = nnn;
	}
	else if (cod == 0x2) {
		stack.push_back(PC);
		PC = nnn;
	}
	else if (cod == 0x3) {
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
	else if (cod == 0x6) {
		if (debug) printf("v[%d] setted to %d\n", x, nn);
		V[x] = nn;
	}
	else if (cod == 0x7) {
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
			V[0xf] = V[x] & 1;
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
	else if (cod == 0xa) {
		I = nnn;
	}
	else if (cod == 0xc) {
		V[x] = random_uchar() & nn;
	}
	else if (cod == 0xd) {
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
			PC -= 2;
			if (keys[V[x]]) {
				PC += 2;
			}
		}
		else if (nn == 0x29) {
			I = (V[x] * 5) + 1;
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
			memory[I + 1] = d;
			memory[I + 2] = u;
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
	}
	else {
		printf("UNIMPLEMENTED INSTRUCTION %c%c%c%c\n", xh(current_decoded_ins[0]), xh(current_decoded_ins[1]), xh(current_decoded_ins[2]), xh(current_decoded_ins[3]));
	}
}

void C8::Chip8::handle_input() {
	kbstate = SDL_GetKeyboardState(NULL);

	auto mirror_pressed = [&](Uint8 key, int c8_key) {
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

void C8::Chip8::cycle() {
	handle_input();

	fetch();
	decode();
	execute();

	if (DelayTimer > 0) {
		DelayTimer--;
	}
}

void C8::Chip8::print_and_draw() {
	std::stringstream program_counter;
	program_counter << "PC: " << PC;
	pc_text.update_w(program_counter.str());

	std::stringstream registers;
	for (int i = 0; i < 9; i++) {
		registers << "V" << i << ": " << (int)V[i] << " ";
	}
	registers_text1.update_w(registers.str());

	registers.str(std::string());
	for (int i = 9; i < 16; i++) {
		registers << "V" << i << ": " << (int)V[i] << " ";
	}
	registers_text2.update_w(registers.str());


	std::stringstream inst;
	inst << "Inst: " << xh(current_decoded_ins[0]) << xh(current_decoded_ins[1]) << xh(current_decoded_ins[2]) << xh(current_decoded_ins[3]);
	current_instruction.update_w(inst.str());

	pc_text.render();
	registers_text1.render();
	registers_text2.render();
	current_instruction.render();
}

void C8::Chip8::reset() {
	for (int i = 0; i < 4096; i++) {
		memory[i] = 0;
	}

	load_chars_in_mem();
	screen.clear();
	PC = 0;
}

void C8::Chip8::set_pixel(unsigned char x, unsigned char y, unsigned char n) {
	if (screen.sprite(x, y, n, I, memory)) {
		V[0xf] = 1;
	}
	else {
		V[0xf] = 0;
	}
}