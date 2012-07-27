#include <iostream>
#include <cstdio>

#include "game.h"

int lfb = 0;
int fps = 30;

int main(int argc, char* argv[]){
	bool input = true;
	SDL_Event e;
	
	game_init("data/cfg/db.cfg", "data/cfg/settings.cfg", "data/cfg/theme.cfg");
	
	/*while (running && gamePhase == MAIN_MENU) {
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) running = false;
			
			btn_newGame.checkEvents(e);
			btn_loadGame.checkEvents(e);
			btn_quitMenu.checkEvents(e);
		}
		
		SDL_FillRect(window, &window->clip_rect, 0);
		
		SDL_Rect titleOffset = {(window->w - title_image->w) / 2, 150};
		SDL_BlitSurface(title_image, NULL, window, &titleOffset);
		
		btn_newGame.print(window);
		btn_loadGame.print(window);
		btn_quitMenu.print(window);
		
		SDL_Flip(window);
	}*/
	
	startBattle("c_river_arena");
	
	int lfb;
	
	currentBattle.type = {CON_HUMAN, CON_AI};
	
	while (running){
		lfb = SDL_GetTicks();
		
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) running = false;
		}
		
		SDL_FillRect(window, &window->clip_rect, 0);
		currentBattle.print(window, window->w / 2, window->h / 2);
		SDL_Flip(window);
		
		currentBattle.nextFrame();
		currentBattle.turn();
		
		if (SDL_GetTicks() - lfb < 1000 / fps) SDL_Delay(1000 / fps + lfb - SDL_GetTicks());
	}
	
	return 0;
}
	