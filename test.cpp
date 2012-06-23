#include <iostream>
#include <cstdio>

#include "game.h"

int lfb = 0;
int fps = 30;

int main(int argc, char* argv[]){
	bool running = true;
	SDL_Event e;
	
	game_init("data\\cfg\\db.cfg", "data\\cfg\\settings.cfg");
	activeMap = *get(&mapDb, "tutorial_island");
	
	unit *hero = activeMap.createUnit("slime", "buch", 15, 15, 0);
	hero->giveWeapon_primary("bash");
	controller c;
	c.u = hero;
	
	int turn = 0;
			
	while (running){
		lfb = SDL_GetTicks();
		
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) running = false;
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
		}
		
		SDL_FillRect(window, &window->clip_rect, 0);
		activeMap.print(window, window->w / 2, window->h / 2, hero);
		SDL_Flip(window);
		
		activeMap.nextFrame();
		if (c.ready()) c.getInput();

		if (SDL_GetTicks() - lfb < 1000 / fps) SDL_Delay(1000 / fps + lfb - SDL_GetTicks());
	}
	
	return 0;
}
	