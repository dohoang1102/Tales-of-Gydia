#include <iostream>
#include <cstdio>

#include "game.h"

int main(int argc, char* argv[]){
	bool running = true;
	SDL_Event e;
	
	game_init("data\\cfg\\db.cfg", "testMap");
	unit* u = activeMap.createUnit("baseMale", "Buch", 160, 160);
		
	while (running){
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) running = false;
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
		}
		
		SDL_FillRect(window, &window->clip_rect, 0);
		u->execOrder(CMD_MOVE_S, "");
		u->anims.next();
		activeMap.print(window, window->w / 2, window->h / 2);
		SDL_Flip(window);
		
		SDL_Delay(50);
	}
	
	return 0;
}