#include <iostream>
#include <cstdio>

#include "game.h"

int lfb = 0;
int fps = 30;

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
		
		SDL_Rect titleOffset = {(window->w - title_image->w) / 2, 150};
		SDL_BlitSurface(title_image, NULL, window, &titleOffset);
		
		btn_newGame.print(window);
		btn_loadGame.print(window);
		btn_quitMenu.print(window);
		
		SDL_Flip(window);
	}

	int curFps = 1;
	
	while (running && gamePhase == GAME_PHASE){
		lfb = SDL_GetTicks();
		
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) running = false;
			else if (e.type == SDL_KEYDOWN){
				switch (e.key.keysym.sym){
					case SDLK_RETURN:
					if (current.view == GAME && current.ansCount() <= 1) current.nextDialog();
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
					
					case SDLK_o:
					if (current.view != LEVELUP_1 && current.view != LEVELUP_2) current.view = QUEST;
					break;
					
					case SDLK_p:
					if (current.view != LEVELUP_1 && current.view != LEVELUP_2){
						int dX = current.player.units[0]->x, dY = current.player.units[0]->y;
						
						switch (GETDIR(current.player.units[0]->action)){
							case NORTH: dY--; break;
							case WEST: dX--; break;
							case SOUTH: dY++; break;
							case EAST: dX++; break;
						}
						
						if (current.m->getDeco(dX, dY) && current.m->getDeco(dX, dY)->container > 0){
							int i;
							for (i = 0; i < 12; i++) current.exchanging[i] = current.m->getDeco(dX, dY)->inside[i];
							current.view = EXCHANGE;
						}
					}
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
		
		current.nextFrame();
		if (input) current.turnMoves();
		
		if (current.view == EXCHANGE){
			int dX = current.player.units[0]->x, dY = current.player.units[0]->y;
						
			switch (GETDIR(current.player.units[0]->action)){
				case NORTH: dY--; break;
				case WEST: dX--; break;
				case SOUTH: dY++; break;
				case EAST: dX++; break;
			}
			
			if (current.m->getDeco(dX, dY) && itemCount(current.m->getDeco(dX, dY)->inside) > 0){
				int i;
				for (i = 0; i < 12; i++) current.m->getDeco(dX, dY)->inside[i] = current.exchanging[i];
				current.view = EXCHANGE;
			}
		}
		
		curFps = 1000 / (SDL_GetTicks() - lfb);
		if (SDL_GetTicks() - lfb < 1000 / fps) SDL_Delay(1000 / fps + lfb - SDL_GetTicks());
	}
	
	if (gamePhase == GAME_PHASE){
		ofstream svf ("data\\cfg\\saves\\continue.cfg");
		svf << current.toScriptObj().toString();
		svf.close();
	}
	
	return 0;
}
	