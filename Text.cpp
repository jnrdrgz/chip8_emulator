#include "Text.h"

Text::Text(std::string text, int x, int y, int w, int h) : text{ text }, rct{ x,y,w,h }, color{ 255,0,0,255 } {
	load_text();
}

Text::Text(std::string text, int x, int y, int w, int h, SDL_Color color) : text{ text }, rct{ x,y,w,h }, color{ color } {
	load_text();
}

Text::Text(std::string text, int x, int y, int h) : text{ text }, color{ 255,0,0,255 }{
	load_text_and_rct(x, y, h);
}

Text::Text(std::string text, int x, int y, int h, SDL_Color color) : text{ text }, color{ color }{
	load_text_and_rct(x, y, h);
}

Text::~Text() {
	//std::cout << "Destrcutor called\n";
	SDL_DestroyTexture(texture);
}

void Text::load_text() {
	SDL_Surface* tmp_sur = TTF_RenderUTF8_Solid(SDL::Context::font, text.c_str(), color);
	texture = SDL_CreateTextureFromSurface(SDL::Context::renderer, tmp_sur);
	SDL_FreeSurface(tmp_sur);
}

void Text::load_text_and_rct(int x, int y, int h) {
	SDL_Surface* tmp_sur = TTF_RenderUTF8_Solid(SDL::Context::font, text.c_str(), color);
	if (!tmp_sur) printf("error in text surf: %s\n", TTF_GetError());
	texture = SDL_CreateTextureFromSurface(SDL::Context::renderer, tmp_sur);

	rct.w = tmp_sur->w;

	rct.x = x;
	rct.y = y;
	rct.h = h;

	SDL_FreeSurface(tmp_sur);
}

void Text::update(std::string text) {
	SDL_DestroyTexture(texture);
	this->text = text;
	load_text();
}

void Text::update_w(std::string text) {
	SDL_DestroyTexture(texture);
	this->text = text;
	load_text_and_rct(rct.x, rct.y, rct.h);
}


void Text::render() {
	SDL_SetRenderDrawColor(SDL::Context::renderer, 122, 122, 122, 255);
	SDL_RenderDrawRect(SDL::Context::renderer, &rct);
	SDL_RenderCopy(SDL::Context::renderer, texture, nullptr, &rct);
}