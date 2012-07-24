//World editor tool

#include "game.h"

SDL_Event e;

textBox prompt;//Command prompt
int mode = 0; string brush = "grass"; int brushLayer = 0;//Currently picked terrain and layer

//Bucket fill function
void bucketFill(map* m, int x, int y, string brushId, int brushLayer){
	string oldId = m->getTile(x, y)->id;
	int oldLayer = m->getTile(x, y)->layer;
	
	if (oldLayer == brushLayer && oldId == brushId) return;
	
	m->setTile(x, y, brushId, brushLayer);//Sets first tile
	
	if (x > 0 && m->getTile(x - 1, y)->id == oldId && m->getTile(x - 1, y)->layer == oldLayer) bucketFill(m, x - 1, y, brushId, brushLayer);
	if (y > 0 && m->getTile(x, y - 1)->id == oldId && m->getTile(x, y - 1)->layer == oldLayer) bucketFill(m, x, y - 1, brushId, brushLayer);
	if (x < m->width() - 1 && m->getTile(x + 1, y)->id == oldId && m->getTile(x + 1, y)->layer == oldLayer) bucketFill(m, x + 1, y, brushId, brushLayer);
	if (y < m->height() - 1 && m->getTile(x, y + 1)->id == oldId && m->getTile(x, y + 1)->layer == oldLayer) bucketFill(m, x, y + 1, brushId, brushLayer);
}

void edit(map* m){
	if (!m) return;//Exit if no map passed
		
	int oldX = -1, oldY = -1, oldLayer = -1; string oldTerrain = "";//Old data to undo
	
	bool mmap = false;//Flag show minimap
	bool edit = true;
	bool grid = true;
	
	int oX = 0, oY = 0;//Offset
	
	while (edit){
		while (SDL_PollEvent(&e)){
			prompt.checkEvents(e);
			
			if (e.type == SDL_QUIT){ running = false; edit = false; }
			else if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) && prompt.isFocused()){//If command was prompted
				char* cmd = (char*) prompt.text.c_str();//C-string text
				deque<string> tokens;//Command tokens
				
				char* curTok = strtok(cmd, " ");//Current token
				while (curTok) {//While there are tokens left
					tokens.push_back(string(curTok));//Adds token
					curTok = strtok(NULL, " ");//Next token
				}
				
				if (tokens[0] == "id" && tokens.size() >= 2) m->id = tokens[1];
				
				if (tokens[0] == "author" && tokens.size() >= 2) m->author = tokens[1];
				
				if (tokens[0] == "desc" && tokens.size() >= 2){
					int i;
					m->description = "";
					for (i = 1; i < tokens.size(); i++) m->description += " " + tokens[i];
					m->description.erase(0,1);
				}
				
				else if (tokens[0] == "new" && tokens.size() >= 4){
					m->resize(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()));
					m->fill(tokens[3], 0);
					m->makePict(grid);
					m->makeMmap();
				}
				
				else if (tokens[0] == "fill" && tokens.size() >= 3){
					m->fill(tokens[1], atoi(tokens[2].c_str()));
					m->makePict(grid);
					m->makeMmap();
				}
				
				else if (tokens[0] == "pick" && tokens.size() >= 3){
					mode = 0;
					brush = tokens[1];
					brushLayer = atoi(tokens[2].c_str());
				}
				
				else if (tokens[0] == "deco" && tokens.size() >= 2){
					mode = 1;
					brush = tokens[1];
				}
				
				else if (tokens[0] == "undo" && oldX != -1){
					m->setTile(oldX, oldY, oldTerrain, oldLayer);
					m->makePict(grid);
					m->makeMmap();
				}
				
				else if (tokens[0] == "grid"){
					grid = !grid;
					m->makePict(grid);
				}
				
				else if (tokens[0] == "mmap") mmap = !mmap;
				
				else if (tokens[0] == "save"){
					int i;
					string fPath = "";
					for (i = 1; i < tokens.size(); i++) fPath += " " + tokens[i];
					fPath.erase(0,1);
					
					ofstream f(fPath.c_str());
					f << OBJTYPE_MAP << " " << m->id << " {\n";
					f << m->toScriptObj().toString(1);
					f << "};";
					f.close();
				}
				
				else if (tokens[0] == "open" && tokens.size() >= 1){
					if (get(&mapDb, tokens[1])) m = get(&mapDb, tokens[1]);
					m->makePict(grid);
				}
								
				else if (tokens[0] == "quit") edit = false;
				
				else if (tokens[0] == "scshot"){
					int i;
					string fPath = "";
					for (i = 1; i < tokens.size(); i++) fPath += " " + tokens[i];
					fPath.erase(0,1);
					
					SDL_Surface* s = m->pict;
					m->print(s, s->w / 2, s->h / 2);
					SDL_SaveBMP(s, fPath.c_str());
					
					m->makePict(grid);
				}
				
				prompt.text = "";
				
				prompt.getFocus();
			}
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F1){
				int mX, mY;
				SDL_GetMouseState(&mX, &mY);
				
				int tX = floor((mX - (window->w / 2 - m->pict->w / 2 + oX)) / tilesSide);
				int tY = floor((mY - (window->h / 2 - m->pict->h / 2 + oY)) / tilesSide);
				
				prompt.text = toString(tX) + "," + toString(tY);
			}
			else if (e.type == SDL_MOUSEBUTTONUP){
				int tX = floor((e.button.x - (window->w / 2 - m->pict->w / 2 + oX)) / tilesSide);
				int tY = floor((e.button.y - (window->h / 2 - m->pict->h / 2 + oY)) / tilesSide);
				
				if (m->getTile(tX, tY)){
					if (e.button.button == SDL_BUTTON_LEFT && mode == 0){
						oldX = tX;
						oldY = tY;
						oldLayer = m->getTile(tX, tY)->layer;
						oldTerrain = m->getTile(tX, tY)->id;
						
						m->setTile(tX, tY, brush, brushLayer);
						m->makePict(grid);
						m->makeMmap();
					}
					
					else if (e.button.button == SDL_BUTTON_LEFT && mode == 1){
						m->createDeco(brush, tX, tY);
						m->makePict(grid);
					}
					
					else if (e.button.button == SDL_BUTTON_RIGHT && mode == 0){
						brush = m->getTile(tX, tY)->id;
						brushLayer = m->getTile(tX, tY)->layer;
					}
					
					else if (e.button.button == SDL_BUTTON_RIGHT && mode == 1){
						m->removeDeco(tX, tY);
						m->makePict(grid);
					}
					
					else if (e.button.button == SDL_BUTTON_MIDDLE && mode == 0){
						bucketFill(m, tX, tY, brush, brushLayer);
						m->makePict(grid);
						m->makeMmap();
					}
				}
				
				prompt.getFocus();
			}
			
			else if (e.type == SDL_KEYDOWN){
				if (e.key.keysym.sym == SDLK_UP) oY += tilesSide;
				if (e.key.keysym.sym == SDLK_DOWN) oY -= tilesSide;
				if (e.key.keysym.sym == SDLK_RIGHT) oX -= tilesSide;
				if (e.key.keysym.sym == SDLK_LEFT) oX += tilesSide;
			}
		}
		
		SDL_FillRect(window, &window->clip_rect, 0x202020);
		
		boxColor(window, 0, 0, window->w, window->h, 0x0000007A);
		
		m->print(window, window->w / 2 + oX, window->h / 2 + oY);
		if (mmap) m->printMmap(window, m->mmap->w / 2 + 5, m->mmap->h / 2 + 5);
		
		int i = 0;
		list<terrain>::iterator t;
		SDL_Rect o = {window->w - tilesSide / 2 - 5, tilesSide / 2 + 5};
		for (t = terrainDb.begin(); t != terrainDb.end(); t++, i++){
			t->print(window, o.x, o.y);
			
			if (brush == t->id && mode == 0){
				boxColor(window, o.x - tilesSide / 2 - 2, o.y - tilesSide / 2 - 2, o.x + tilesSide / 2 + 1, o.y + tilesSide / 2 + 1, 0xFFFFFF5A);
			}
			
			o.y += tilesSide + 5;
			
			if (o.y > window->h){
				o.y = tilesSide / 2 + 5;
				o.x -= tilesSide + 5;
			}
		}
		
		SDL_Surface* text = TTF_RenderText_Solid(globalFont, string("Id: " + m->id).c_str(), SDL_Color{255,255,255});
		SDL_Rect offset = {5, window->h - prompt.h - TTF_FontHeight(globalFont) * 3};
		SDL_BlitSurface(text, NULL, window, &offset);
		SDL_FreeSurface(text);
		
		text = TTF_RenderText_Solid(globalFont, string("Author: " + m->author).c_str(), SDL_Color{255,255,255});
		offset.y += TTF_FontHeight(globalFont);
		SDL_BlitSurface(text, NULL, window, &offset);
		SDL_FreeSurface(text);
		
		text = TTF_RenderText_Solid(globalFont, string("Description: " + m->description).c_str(), SDL_Color{255,255,255});
		offset.y += TTF_FontHeight(globalFont);
		SDL_BlitSurface(text, NULL, window, &offset);
		SDL_FreeSurface(text);
		
		prompt.print(window);
		SDL_Flip(window);
	}
}

int main(int argc, char* argv[]){
	string dbFile = "data\\cfg\\db.cfg", settingsFile = "data\\cfg\\settings.cfg", themeFile = "data\\cfg\\theme.cfg";//Tileset file and settings file
	
	int oldX = -1, oldY = -1, oldLayer = -1; string oldTerrain = "";//Old data to undo
	
	bool mmap = true;//Flag show minimap
	
	int i;//Counter for args
	for (i = 0; i < argc; i++){//For each argument
		string a = argv[i];//Arg converted to string
		
		if (a.substr(0,2) == "-d") dbFile = a.substr(2);
		if (a.substr(0,2) == "-s") settingsFile = a.substr(2);
		if (a.substr(0,3) == "-t") themeFile = a.substr(2);
	}
	
	
	
	game_init(dbFile, settingsFile, themeFile);
	
	
	
	prompt.x = 0;
	prompt.y = window->h - 20;
	prompt.w = window->w;
	prompt.h = 20;
	prompt.setClickArea();
	prompt.edit = true;
	prompt.setBackColor(0x202020);
	prompt.setAlpha(128);
	prompt.font = globalFont;
	prompt.foreColor = 0xFFFFFF;
	prompt.getFocus();
	
	
	current.m = new map;
	current.m->resize(5,5);
	current.m->fill("dirt",0);
	current.m->makePict(true);
	current.m->makeMmap();
	edit(current.m);
	
	
	return 0;
}