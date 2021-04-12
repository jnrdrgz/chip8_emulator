#include "SDL_context.h"

SDL_Renderer* SDL::Context::renderer = NULL;
SDL_Event SDL::Context::event;
TTF_Font* SDL::Context::font = NULL;
const int SDL::Context::screen_w = 640;
const int SDL::Context::screen_h = 320;
