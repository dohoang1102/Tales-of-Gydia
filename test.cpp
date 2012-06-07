#include <iostream>
#include <cstdio>

#include "game.h"

int main(int argc, char* argv[]){
	bool running = true;
	SDL_Event e;
	
	game_init("data\\db.cfg", "testMap");
	
	while (running){
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) running = false;
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
		}
		
		SDL_FillRect(window, &window->clip_rect, 0);
		activeMap.print(window, window->w / 2, window->h / 2);
		SDL_Flip(window);
	}
	
	return 0;
}