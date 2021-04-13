#pragma once

#include "SDL_Context.h"


struct Screen {
	Screen(int scale);
	~Screen();
	void draw();
	void clear();
	bool sprite(int x, int y, int n, int I, unsigned char* mem);

	SDL_Texture* texture = nullptr;
	int sw, sh, scale, max_p;
	Uint32* pixels;
	SDL_Rect rct;
};