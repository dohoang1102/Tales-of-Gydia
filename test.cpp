#include <iostream>
#include <cstdio>

#include "game.h"

int main(int argc, char* argv[]){
	bool running = true;
	SDL_Event e;
	
	game_init("data\\cfg\\db.cfg", "data\\cfg\\settings.cfg", "testMap");
	unit* u = activeMap.createUnit("baseMale", "Buch", 40, 40);
	controller c;
	c.addUnit(u);
		
	activeMap.makeTmap();
	while (running){
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) running = false;
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
		}
		
		SDL_FillRect(window, &window->clip_rect, 0);
		c.getInput();
		activeMap.animate();
		activeMap._time();
		activeMap.print(window, window->w / 2, window->h / 2);
		SDL_BlitSurface(activeMap.mmap, NULL, window, NULL);
		SDL_Flip(window);
		
		SDL_Delay(40);
	}
	
	return 0;
}