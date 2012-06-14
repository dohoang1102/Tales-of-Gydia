#include <iostream>
#include <cstdio>

#include "game.h"

int main(int argc, char* argv[]){
	bool running = true;
	SDL_Event e;
	
	game_init("data\\cfg\\db.cfg", "data\\cfg\\settings.cfg", "testMap");
	unit* u = activeMap.createUnit("baseMale", "Buch", 40, 40);
	u->giveWeapon_primary("staff");
	controller c;
	c.addUnit(u);
	
	activeMap.createUnit("baseMale", "COM1", rand() % (activeMap.width() * tilesSide), rand() % (activeMap.height() * tilesSide));
	activeMap.createUnit("baseMale", "COM2", rand() % (activeMap.width() * tilesSide), rand() % (activeMap.height() * tilesSide));
	activeMap.createUnit("baseMale", "COM3", rand() % (activeMap.width() * tilesSide), rand() % (activeMap.height() * tilesSide));
		
	activeMap.makeTmap();
	while (running){
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) running = false;
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
		}
		
		SDL_FillRect(window, &window->clip_rect, 0);
		activeMap._time();
		
		c.getInput();
		activeMap.print(window, window->w / 2, window->h / 2);
		
		SDL_BlitSurface(activeMap.mmap, NULL, window, NULL);
		SDL_Flip(window);
		
		SDL_Delay(50);
	}
	
	return 0;
}