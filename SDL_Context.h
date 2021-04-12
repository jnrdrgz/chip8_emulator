#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>

namespace SDL {
	struct Context
	{
	public:
		Context(const char* title) {
			SDL_Init(SDL_INIT_VIDEO);
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
			SDL_SetHint("SDL_HINT_ANDROID_SEPARATE_MOUSE_AND_TOUCH", "0");

			window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_w, screen_h, SDL_WINDOW_SHOWN);
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

			TTF_Init();
			
			font = TTF_OpenFont("cafeteria-black.ttf", 25);
			if (!font) {
				printf("error loading font\n");
			}
		}

		~Context() {
			SDL_DestroyWindow(window);
			SDL_DestroyRenderer(renderer);
			TTF_Quit();
			SDL_Quit();
		}

		static SDL_Renderer* renderer;
		static TTF_Font* font;
		static SDL_Event event;
		bool running = false; 
		static const int screen_w;
		static const int screen_h;

	private:
		SDL_Window* window = NULL;
	};
}