#include <iostream>
#include <cstdio>

#include "game.h"

int lfb = 0;
int fps = 30;

string sum (string a, string b){
	return toString(atoi(a.c_str()) + atoi(b.c_str()));
}

string mult (string a, string b){
	return toString(atoi(a.c_str()) * atoi(b.c_str()));
}

string gV(string id){
	if (id == "a") return toString(5);
	else return toString(0);
}

int main(int argc, char* argv[]){
	bool running = true;
	bool input = true;
	SDL_Event e;
	
	game_init("data\\cfg\\db.cfg", "data\\cfg\\settings.cfg", "data\\cfg\\theme.cfg");
	
	campaign current = *get(&campaignDb, "tutorial");
	current.setup();

	int curFps = 1;
	
	while (running){
		lfb = SDL_GetTicks();
		
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT) running = false;
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
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
		input = !input;
		
		curFps = 1000 / (SDL_GetTicks() - lfb);
		if (SDL_GetTicks() - lfb < 1000 / fps) SDL_Delay(1000 / fps + lfb - SDL_GetTicks());
	}
	
	return 0;
}
	