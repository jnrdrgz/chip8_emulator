#include "SDL_context.h"

SDL_Renderer* SDL::Context::renderer = NULL;
SDL_Event SDL::Context::event;
TTF_Font* SDL::Context::font = NULL;
SDL_Window* SDL::Context::window = NULL;
const int SDL::Context::screen_w = 640+120;
const int SDL::Context::screen_h = 320+30*2;
