#include <iostream>
#include <cstdio>

#include "game.h"

int lfb = 0;
int fps = 40;

int main(int argc, char* argv[]){
	bool input = true;
	SDL_Event e;
	
	game_init("data\\cfg\\db.cfg", "data\\cfg\\settings.cfg", "data\\cfg\\theme.cfg");
	
	while (running && gamePhase == MAIN_MENU) {
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) running = false;
			
			btn_newGame.checkEvents(e);
			btn_loadGame.checkEvents(e);
			btn_quitMenu.checkEvents(e);
		}
		
		SDL_FillRect(window, &window->clip_rect, 0);
		
		btn_newGame.print(window);
		btn_loadGame.print(window);
		btn_quitMenu.print(window);
		
		SDL_Flip(window);
	}
	
	setCampaign("tutorial");

	int curFps = 1;
	
	while (running && gamePhase == GAME_PHASE){
		lfb = SDL_GetTicks();
		
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) running = false;
			else if (e.type == SDL_KEYDOWN){
				switch (e.key.keysym.sym){
					case SDLK_RETURN:
					if (current.view == GAME) current.nextDialog();
					break;
					
					case SDLK_ESCAPE:
					if (current.view == GAME) running = false;
					else if (current.view != LEVELUP_1 && current.view != LEVELUP_2) current.view = GAME;
					break;
					
					case SDLK_i:
					if (current.view != LEVELUP_1 && current.view != LEVELUP_2) current.view = INVENTORY;
					break;
					
					case SDLK_u:
					if (current.view != LEVELUP_1 && current.view != LEVELUP_2) current.view = PLAYER;
					break;
				}
			}
			
			current.events(e);
		}
		
		SDL_FillRect(window, &window->clip_rect, 0);
		current.print(window, window->w / 2, window->h / 2);
		
		SDL_Surface* txt = TTF_RenderText_Solid(globalFont, ("fps: " + toString(curFps)).c_str(), SDL_Color{255,255,255});
		SDL_Rect o = {window->w - 80, 5};
		SDL_BlitSurface(txt, NULL, window, &o);
		SDL_FreeSurface(txt);
		
		SDL_Flip(window);
		
		if (input) current.turnMoves();
		current.nextFrame();
		input = !input;
		
		curFps = 1000 / (SDL_GetTicks() - lfb);
		if (SDL_GetTicks() - lfb < 1000 / fps) SDL_Delay(1000 / fps + lfb - SDL_GetTicks());
	}
	
	return 0;
}
	