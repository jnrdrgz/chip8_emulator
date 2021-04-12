#include "Screen.h"

Screen::Screen(int scale = 10) : scale{ scale } {
	sw = scale * 64;
	sh = scale * 32;
	rct.x = 0;
	rct.y = 0;
	rct.w = sw;
	rct.h = sh;
	pixels = (Uint32*)malloc(sizeof(Uint32) * sw * sh);
	texture = SDL_CreateTexture(SDL::Context::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, sw, sh);
	clear();
}

Screen::~Screen() {
	free(pixels);
	SDL_DestroyTexture(texture);
}

void Screen::draw() {
	SDL_UpdateTexture(texture, nullptr, pixels, 4 * sw);
	SDL_RenderCopy(SDL::Context::renderer, texture, NULL, &rct);
}


void Screen::clear() {
	for (int i = 0; i < sw * sh; i++) {
		pixels[i] = 0;
	}
}

bool Screen::sprite(int x, int y, int n, int I, unsigned char* mem) {
	int debug = 0;
	if (debug) printf("--------------draw---------------\n");
	if (debug) printf("drawing at: x:%d,y:%d,n:%d,i_%d\n", x, y, n, I);
	int scaled_x = x * scale;
	int scaled_y = y * scale;

	if (debug) printf("drawing at scaled: x:%d,y:%d,n:%d,i_%d\n", scaled_x, scaled_y, n, I);
	int pos = scaled_x + (sw * scaled_y);
	if (debug) printf("pos %d\n", pos);

	//Uint32* p = &pixels[];
	unsigned char* val = &mem[I];
	if (debug) printf("I: %d setted to\n", I);
	if (debug) printf("mem[i]: %d setted to\n", *val);


	auto draw_10x10 = [&](int p) {
		bool collision = true;
		for (int i = 0; i < scale; i++) {
			for (int j = 0; j < scale; j++) {
				Uint32 v = pixels[p];
				pixels[p] ^= 0xFFFFFFFF;
				collision = v && !pixels[p] && collision;
				p += 1;
			}
			p += sw;
			p -= scale;

		}
		return collision;
	};

	bool collision = false;
	for (int h = 0; h < n; h++) {
		unsigned char nn = *val;
		//printf("will draw %d\n", nn);
		int ind = 7 * scale;


		for (Uint32 i = 0; i < 8; i++) {
			if (debug) printf("%d", (nn >> i) & 1);
			if ((nn >> i) & 1) {
				if (draw_10x10(pos + ind)) {
					collision = true;
				}
			}
			pos += scale;
			ind -= scale * 2;
		}

		if (debug) printf("\n");

		pos += (sw * scale) - scale * 8;
		val++;
	}
	if (debug) printf("--------------draw---------------\n");

	return collision;
}
