//Game handling header

#ifndef _MAP
#define _MAP

int tilesSide = 32;//Side of each tile of the map

#include "error.h"
#include "cache.h"
#include "script.h"
#include "locale.h"

#include "image.h"
#include "ui.h"

#include <string>
#include <deque>
#include <list>

using namespace std;

int CONTENT_INVALIDIDERROR = getErrorCode();//Code for invalid content request

int MAP_ERROR				= getErrorCode();//Generic map error
int MAP_INVALIDCOORDSERROR	= getErrorCode();//Invalid coordinates error
int MAP_WARNING 			= getErrorCode();//Generic map warning

int DB_VARIABLEWARNING		= getErrorCode();//Non-lethal variable error in database object

#define OBJTYPE_TERRAIN		"terrain"//Object type terrain
#define OBJTYPE_UNIT		"unit"//Object type unit
#define OBJTYPE_MAP			"map"//Object type map

#define CONTROLLER_HUMAN	0//Controller mode human (keyboard + mouse)
#define CONTROLLER_AI		1//Controller mode AI

#define NORTH				0//North direction
#define EAST				1//East direction
#define SOUTH				2//South direction
#define WEST				3//West direction

#define CMD_IDLE			0//Idle command

#define CMD_MOVE			0x10//Generic move command
#define CMD_MOVE_N			0x10//Move north command
#define CMD_MOVE_E			0x11//Move east command
#define CMD_MOVE_S			0x12//Move south command
#define CMD_MOVE_W			0x13//Move west command

#define CMD_SAY				0x20//Say command

#define SPEAKTIME			2500//Speaking time

class map;//Map class prototype

typedef struct {int code; string args;} order;//Order structure

TTF_Font* globalFont = NULL;//Global font

//Content base class
class content{
	public:
	string id;
	string author;
	string description;
	
	//Constructor
	content(){
		id = "";
		author = "";
		description = "";
	}
	
	#ifdef _SCRIPT//If script handling header has been included
		//Function to load from script object
		void fromScriptObj(object o){
			id = o.name;
			
			var* author = o.getVar("author");
			var* description = o.getVar("description");
			
			if (author) this->author = author->value;
			if (description) this->description = description->value;
		}
	#endif
};

//Function to get a content with a specific id from a content deque
template <class contentType> contentType* get(list<contentType> *source, string id){
	typename list<contentType>::iterator i;//Iterator
	
	for (i = source->begin(); i != source->end(); i++)//For each element in source deque
		if (i->id == id) return &*i;//Returns element if matching
		
	if (errFunc) errFunc(CONTENT_INVALIDIDERROR, "Content " + id + " not found in content list");//Error message	
	return NULL;//Returns a null pointer if no element was found
}

//Terrain class
class terrain: public content{
	public:
	list<image> transitions;
	image baseImage;//Base image of the terrain
	int layer;//Layer of the terrain (transitions are printed only above lower layers)
	bool free;//Flag indicating if player can walk on terrain
	Uint32 mmapColor;//Color in minimap
	
	//Constructor
	terrain(){
		id = "";
		author = "";
		description = "";
		layer = 0;
		free = true;
		mmapColor = 0;
	}
	
	//Function to get transition image
	image* getTransition(string id){
		return get(&transitions, id);//Returns image
	}
	
	//Function to print terrain
	void print(SDL_Surface* target, int x, int y, string transitionId = ""){
		if (transitionId != ""){//If there's a transition to use
			image* i = getTransition(transitionId);//Gets the transition
			
			if (i){//If transition is valid
				i->print(target, x, y);//Prints it
				return;//Exits function
			}
		}
		
		baseImage.print(target, x, y);//Prints image
	}
	
	#ifdef _SCRIPT
		//Function to load from script object
		void fromScriptObj(object o){
			if (o.type == OBJTYPE_TERRAIN){//If type is matching
				content::fromScriptObj(o);//Loads base content information
				
				object* baseImage = o.getObj("baseImage");//Base image sub-object
				
				if (baseImage) this->baseImage.fromScriptObj(*baseImage);//Loads base image
				
				deque<object>::iterator i;//Iterator
				for (i = o.o.begin(); i != o.o.end(); i++){//For each sub object
					if (i->type == OBJTYPE_IMAGE && i->name != "baseImage"){//If object is an image
						image m;//New image
						m.fromScriptObj(*i);//Loads the image
						transitions.push_back(m);//Adds the image to the transitions
					}
				}
				
				var* free = o.getVar("free");//Free variable
				var* mmapColor = o.getVar("mmapColor");//Minimap color variable
				
				if (free) this->free = free->intValue();//Gets free flag
				if (mmapColor) this->mmapColor = strtol(mmapColor->value.c_str(), NULL, 0);//Gets minimap color
			}
			
			#ifdef _ERROR
				else if (errFunc) errFunc(SCRIPT_WARNING_INVALIDOBJTYPE, "[map.h] No terrain could be loaded from " + o.type + " object");
			#endif
		}
	#endif
};

list <terrain> terrainDb;//Terrains database

//Unit class
class unit: public content{
	public:
	string name;//Unit name
	int x, y;//Unit position
	int speed;//Unit walking speed
	int margin;//Unit margin (minimum distance from limits)
	int direction;//Current direction
	animSet anims;//Unit animations
	
	map *parent;//Parent map
	
	string saying;//Words said by the unit
	unsigned int sayStart;//Time of speaking command
	
	//Constructor
	unit(){
		x = 0;
		y = 0;
		speed = 5;
		direction = SOUTH;
		parent = NULL;
	}
	
	//Function to print unit
	void print(SDL_Surface* target, int x, int y){
		anims.print(target, x, y);//Prints animation
		
		if (saying != "" && globalFont){//If unit is saying something and there's a valid font
			SDL_Surface* text = TTF_RenderText_Solid(globalFont, saying.c_str(), SDL_Color {30,30,30});//Text surface
			
			roundedBoxColor(target, x - text->w / 2 - 5, y - 1.5 * tilesSide - text->h - 5, x + text->w / 2, y - 1.5 * tilesSide, 5, 0xFFFFFFD0);//Fills balloon
			
			Sint16 px [] = {x - 3, x + 3, x};
			Sint16 py [] = {y - 1.5 * tilesSide + 1, y - 1.5 * tilesSide + 1, y - 1.3 * tilesSide};
			filledPolygonColor(target, px, py, 3, 0xFFFFFFD0);
			
			SDL_Rect offset = {x - text->w / 2, y - 1.5 * tilesSide - text->h};//Offset rectangle
			SDL_BlitSurface(text, NULL, target, &offset);//Prints text
			
			SDL_FreeSurface(text);//Frees text
		}
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_UNIT){//If object type is matching
			content::fromScriptObj(o);//Loads base content data
			
			object* anims = o.getObj("anims");//Animations
			var* speed = o.getVar("speed");//Speed variable
			var* margin = o.getVar("margin");//Margin variable
			
			if (anims) this->anims.fromScriptObj(*anims);//Loads animations
			if (speed) this->speed = speed->intValue();//Gets speed
			if (margin) this->margin = margin->intValue();//Gets margin
			
			this->anims.setAnim("idle_s");//Sets animation
		}
	}
	
	//Function to move unit
	void walk(int direction);
	
	//Function to stop unit (sets idle animation)
	void stop(){
		switch (direction){
			case NORTH: anims.setAnim("idle_n"); break;
			case EAST: anims.setAnim("idle_e"); break;
			case SOUTH: anims.setAnim("idle_s"); break;
			case WEST: anims.setAnim("idle_w"); break;
		}
	}
	
	//Function to speak
	void say (string text){
		sayStart = SDL_GetTicks();//Saves time
		saying = text;//Sets text
	}
	
	//Function to receive and execute orders
	void execOrder(order o){
		if (o.code == CMD_IDLE) stop();//Idle command - stops unit
		else if ((o.code & 0xF0) == CMD_MOVE) walk(o.code & 0xF);//Move command - moves unit
		else if (o.code == CMD_SAY) say(o.args);//Say command
		else stop();//Stops unit on any other command
	}
	
	//Function executing everything connected to timelapse
	void _time(){
		if (SDL_GetTicks() - sayStart >= SPEAKTIME) saying = "";//Clears words said by unit
	}
};

list <unit> unitDb;//Units database

//Class controller
class controller{
	public:
	int mode;//Controller mode (KEYBOARD, JOYSTICK, AI)
	list<unit*> u;//Units controlled by this controller
	
	//Constructor
	controller(){
		mode = CONTROLLER_HUMAN;
	}
	
	//Function to add unit to controller
	void addUnit(unit* un){
		u.push_back(un);//Adds unit to controlled units
	}
	
	//Function that gives order to all units linked to this controller
	void giveOrder(order o){
		list<unit*>::iterator i;//Iterator
		
		for (i = u.begin(); i != u.end(); i++)//For each unit
			(*i)->execOrder(o);//Gives order to unit
	}
	
	//Function that gets input from keyboard-mouse
	void getInput(){
		if (mode != CONTROLLER_HUMAN) return;//Exits function if controller is not human
		
		Uint8* keys = SDL_GetKeyState(NULL);//Gets keys
		
		//Walking orders
		if (keys[SDLK_UP]) giveOrder({CMD_MOVE_N, ""});
		else if (keys[SDLK_RIGHT]) giveOrder({CMD_MOVE_E, ""});
		else if (keys[SDLK_DOWN]) giveOrder({CMD_MOVE_S, ""});
		else if (keys[SDLK_LEFT]) giveOrder({CMD_MOVE_W, ""});
		else giveOrder({CMD_IDLE, ""});
	}
};

//Map class
class map: public content{
	private:
	int w, h;//Size of the map
	
	public:
	terrain* tiles;//Tiles of the map
	
	SDL_Surface* pict;//Picture of the map
	SDL_Surface* mmap;//Picture of the minimap
	SDL_Surface* tmap;//Picture of the transitions mask
		
	list <unit> units;//Units of the map
	
	//Constructor
	map(){
		id = "";
		author = "";
		description = "";
		
		tiles = NULL;
		pict = NULL;
		mmap = NULL;
		
		resize(1,1);
	}
	
	//Function to resize the map
	//Old tiles are cleared
	void resize(unsigned int newW, unsigned int newH){
		if (tiles) delete tiles;//Deletes tiles
		
		//Adjust arguments to be at least 1
		if (newW < 1) newW = 1;
		if (newH < 1) newH = 1;
		
		//Sets new sizes
		w = newW;
		h = newH;
		
		tiles = new terrain [w * h];//Creates new tiles array
		
		if (pict) SDL_FreeSurface(pict);//Frees picture
		if (mmap) SDL_FreeSurface(mmap);//Frees minimap
		
		pict = SDL_CreateRGBSurface(SDL_SWSURFACE, w * tilesSide, h * tilesSide, 32, 0, 0, 0, 0);//Creates picture
		mmap = SDL_CreateRGBSurface(SDL_SWSURFACE, w * 4, h * 4, 32, 0, 0, 0, 0);//Creates minimap
		tmap = SDL_CreateRGBSurface(SDL_SWSURFACE, w * tilesSide, h * tilesSide, 32, 0, 0, 0, 0);//Creates transitions map
	}
	
	//Function to get map width
	int width() {return w;}
	
	int height() {return h;}
	
	//Function to print the map
	void _print(SDL_Surface* target, int x, int y, bool grid = false, int gridCol = 0x00000045){
		//Offset position
		int x0 = x - (w - 1) * tilesSide / 2;
		int y0 = y - (h - 1) * tilesSide / 2;
		
		int i, j;//Counters for tiles
		
		//Base print
		for (i = 0; i < h; i++)//For each row
			for (j = 0; j < w; j++)//For each column
				tiles[i * w + j].print(target, x0 + j * tilesSide, y0 + i * tilesSide);//Prints tile
		
		int curLayer = 0;//Layer currently being printed
		int printedTiles = 0;//Tiles totally printed
		
		while (printedTiles < w * h){//Until all the tile transitions have been printed
			//Prints transitions
			for (i = 0; i < h; i++){//For each row
				for (j = 0; j < w; j++){//For each column
					if (tiles[i * w + j].layer != curLayer) continue;//Next tile if layer is not matching
					else printedTiles++;//Else tile has been printed
					
					if (j > 0){//If not in the first column
						//Checks if left transition is needed
						if ((i == 0 || tiles[(i - 1) * w + j - 1].id != tiles[i * w + j].id) &&
							tiles[i * w + j - 1].id != tiles[i * w + j].id &&
							(i == h - 1 || tiles[(i + 1) * w + j - 1].id != tiles[i * w + j].id) &&
							tiles[i * w + j].layer > tiles[i * w + j - 1].layer &&
							tiles[i * w + j].getTransition("t_0101"))
							tiles[i * w + j].print(target, x0 + (j - 1) * tilesSide, y0 + i * tilesSide, "t_0101");//Prints right to left transition
					}
					
					if (j > 0 && i > 0){//If not in the first row or column
						//Checks if up-left transition is needed
						if (tiles[(i - 1) * w + j - 1].id != tiles[i * w + j].id &&
							tiles[(i - 1) * w + j].id != tiles[i * w + j].id &&
							tiles[i * w + j - 1].id != tiles[i * w + j].id &&
							tiles[i * w + j].layer > tiles[(i - 1) * w + j - 1].layer &&
							tiles[i * w + j].getTransition("t_0001"))
							tiles[i * w + j].print(target, x0 + (j - 1) * tilesSide, y0 + (i - 1) * tilesSide, "t_0001");//Prints up-left transition
							
						//Checks if up-left concave transition is needed
						if (tiles[(i - 1) * w + j - 1].id != tiles[i * w + j].id &&
							tiles[(i - 1) * w + j].id == tiles[i * w + j].id &&
							tiles[i * w + j - 1].id == tiles[i * w + j].id &&
							tiles[i * w + j].layer > tiles[(i - 1) * w + j - 1].layer &&
							tiles[i * w + j].getTransition("t_0111"))
							tiles[i * w + j].print(target, x0 + (j - 1) * tilesSide, y0 + (i - 1) * tilesSide, "t_0111");//Prints up-left transition
					}
					
					if (i > 0){//If not in the first row
						//Checks if up transition is needed
						if ((j == 0 || tiles[(i - 1) * w + (j - 1)].id != tiles[i * w + j].id) &&
							tiles[(i - 1) * w + j].id != tiles[i * w + j].id &&
							(j == w - 1 || tiles[(i - 1) * w + (j + 1)].id != tiles[i * w + j].id) &&
							tiles[i * w + j].layer > tiles[(i - 1) * w + j].layer &&
							tiles[i * w + j].getTransition("t_0011"))
							tiles[i * w + j].print(target, x0 + j * tilesSide, y0 + (i - 1) * tilesSide, "t_0011");//Prints up transition
					}
					
					if (i > 0 && j < w - 1){//If not in the first row or last column
						//Checks if up-right transition is needed
						if (tiles[(i - 1) * w + j + 1].id != tiles[i * w + j].id &&
							tiles[(i - 1) * w + j].id != tiles[i * w + j].id &&
							tiles[i * w + j + 1].id != tiles[i * w + j].id &&
							tiles[i * w + j].layer > tiles[(i - 1) * w + j + 1].layer &&
							tiles[i * w + j].getTransition("t_0010"))
							tiles[i * w + j].print(target, x0 + (j + 1) * tilesSide, y0 + (i - 1) * tilesSide, "t_0010");//Prints up-right transition
							
						//Checks if up-right concave transition is needed
						if (tiles[(i - 1) * w + j + 1].id != tiles[i * w + j].id &&
							tiles[(i - 1) * w + j].id == tiles[i * w + j].id &&
							tiles[i * w + j + 1].id == tiles[i * w + j].id &&
							tiles[i * w + j].layer > tiles[(i - 1) * w + j + 1].layer &&
							tiles[i * w + j].getTransition("t_1011"))
							tiles[i * w + j].print(target, x0 + (j + 1) * tilesSide, y0 + (i - 1) * tilesSide, "t_1011");//Prints up-right transition
					}
					
					if (j < w - 1){//If not in the last column
						//Checks if right transition is needed
						if ((i == 0 || tiles[(i - 1) * w + j + 1].id != tiles[i * w + j].id) &&
							tiles[i * w + j + 1].id != tiles[i * w + j].id &&
							(i == h - 1 || tiles[(i + 1) * w + j + 1].id != tiles[i * w + j].id) &&
							tiles[i * w + j].layer > tiles[i * w + j + 1].layer &&
							tiles[i * w + j].getTransition("t_1010"))
							tiles[i * w + j].print(target, x0 + (j + 1) * tilesSide, y0 + i * tilesSide, "t_1010");//Prints right to left transition
					}
					
					if (i < h - 1 && j < w - 1){//If not in the last row or column
						//Checks if down-right transition is needed
						if (tiles[(i + 1) * w + j + 1].id != tiles[i * w + j].id &&
							tiles[(i + 1) * w + j].id != tiles[i * w + j].id &&
							tiles[i * w + j + 1].id != tiles[i * w + j].id &&
							tiles[i * w + j].layer > tiles[(i + 1) * w + j + 1].layer &&
							tiles[i * w + j].getTransition("t_1000"))
							tiles[i * w + j].print(target, x0 + (j + 1) * tilesSide, y0 + (i + 1) * tilesSide, "t_1000");//Prints down-right transition
						
						//Checks if down-right concave transition is needed
						if (tiles[(i + 1) * w + j + 1].id != tiles[i * w + j].id &&
							tiles[(i + 1) * w + j].id == tiles[i * w + j].id &&
							tiles[i * w + j + 1].id == tiles[i * w + j].id &&
							tiles[i * w + j].layer > tiles[(i + 1) * w + j + 1].layer &&
							tiles[i * w + j].getTransition("t_1110"))
							tiles[i * w + j].print(target, x0 + (j + 1) * tilesSide, y0 + (i + 1) * tilesSide, "t_1110");//Prints down-right transition
					}
					
					if (i < h - 1){//If not in the last row
						//Checks if down transition is needed
						if ((j == 0 || tiles[(i + 1) * w + (j - 1)].id != tiles[i * w + j].id) &&
							tiles[(i + 1) * w + j].id != tiles[i * w + j].id &&
							(j == w - 1 || tiles[(i + 1) * w + (j + 1)].id != tiles[i * w + j].id) &&
							tiles[i * w + j].layer > tiles[(i + 1) * w + j].layer &&
							tiles[i * w + j].getTransition("t_1100"))
							tiles[i * w + j].print(target, x0 + j * tilesSide, y0 + (i + 1) * tilesSide, "t_1100");//Prints down transition
					}
					
					if (i < h - 1 && j > 0){//If not in the last row or first column
						//Checks if down-left transition is needed
						if (tiles[(i + 1) * w + j - 1].id != tiles[i * w + j].id &&
							tiles[(i + 1) * w + j].id != tiles[i * w + j].id &&
							tiles[i * w + j - 1].id != tiles[i * w + j].id &&
							tiles[i * w + j].layer > tiles[(i + 1) * w + j - 1].layer &&
							tiles[i * w + j].getTransition("t_0100"))
							tiles[i * w + j].print(target, x0 + (j - 1) * tilesSide, y0 + (i + 1) * tilesSide, "t_0100");//Prints down-left transition
							
						//Checks if down-left concave transition is needed
						if (tiles[(i + 1) * w + j - 1].id != tiles[i * w + j].id &&
							tiles[(i + 1) * w + j].id == tiles[i * w + j].id &&
							tiles[i * w + j - 1].id == tiles[i * w + j].id &&
							tiles[i * w + j].layer > tiles[(i + 1) * w + j - 1].layer &&
							tiles[i * w + j].getTransition("t_1101"))
							tiles[i * w + j].print(target, x0 + (j - 1) * tilesSide, y0 + (i + 1) * tilesSide, "t_1101");//Prints down-left transition
					}
				}
			}
			
			curLayer++;//Next layer
		}
		
		//If grid is required
		if (grid){
			int i;//Counter
			
			for (i = 0; i < w; i++) lineColor(target, x0 + i * tilesSide - tilesSide / 2, y0 - tilesSide / 2, x0 + i * tilesSide - tilesSide / 2, y0 + h * tilesSide - tilesSide / 2, gridCol);//Prints vertical lines
			for (i = 0; i < h; i++) lineColor(target, x0 - tilesSide / 2, y0 + i * tilesSide - tilesSide / 2, x0 + w * tilesSide - tilesSide / 2, y0 + i * tilesSide - tilesSide / 2, gridCol);//Prints horizontal lines
		}
	}
	
	//Function to print transition mask
	void _printT(SDL_Surface* target, int x, int y, int col = 0x0000007A, int back = 0xFFFFFFFF){
		SDL_FillRect(target, &target->clip_rect, back);//Prints background
		
		//Offset position
		int x0 = x - (w) * tilesSide / 2;
		int y0 = y - (h) * tilesSide / 2;
		
		int i, j;//Counters
		
		int curLayer = 0;//Layer currently being printed
		int printedTiles = 0;//Tiles totally printed
		
		for (i = 0; i < h; i++){//For each row
			for (j = 0; j < w; j++){//For each column
				if (!tiles[i * w + j].free) boxColor(target, j * tilesSide, i * tilesSide, (j + 1) * tilesSide, (i + 1) * tilesSide, col);
			}
		}
		
		while (printedTiles < w * h){//Until all the tile transitions have been printed
			//Prints transitions
			for (i = 0; i < h; i++){//For each row
				for (j = 0; j < w; j++){//For each column
					if (tiles[i * w + j].layer != curLayer) continue;//Next tile if layer is not matching
					else printedTiles++;//Else tile has been printed
					
					if (tiles[i * w + j].free) continue;//Next tile if layer is free
					
					if (j > 0){//If not in the first column
						//Checks if left transition is needed
						if ((i == 0 || tiles[(i - 1) * w + j - 1].id != tiles[i * w + j].id) &&
							tiles[i * w + j - 1].id != tiles[i * w + j].id &&
							(i == h - 1 || tiles[(i + 1) * w + j - 1].id != tiles[i * w + j].id) &&
							tiles[i * w + j].layer > tiles[i * w + j - 1].layer &&
							tiles[i * w + j].getTransition("t_0101")){
							Sint16 px [] = {x0 + (j - 1) * tilesSide + tilesSide / 2, x0 + (j - 1) * tilesSide + tilesSide / 2, x0 + (j) * tilesSide, x0 + (j) * tilesSide};
							Sint16 py [] = {y0 + i * tilesSide, y0 + (i + 1) * tilesSide, y0 + (i + 1) * tilesSide, y0 + i * tilesSide};
							filledPolygonColor(target, px, py, 4, col);
						}	
					}
					
					if (j > 0 && i > 0){//If not in the first row or column
						//Checks if up-left transition is needed
						if (tiles[(i - 1) * w + j - 1].id != tiles[i * w + j].id &&
							tiles[(i - 1) * w + j].id != tiles[i * w + j].id &&
							tiles[i * w + j - 1].id != tiles[i * w + j].id &&
							tiles[i * w + j].layer > tiles[(i - 1) * w + j - 1].layer &&
							tiles[i * w + j].getTransition("t_0001")){
							Sint16 px [] = {x0 + (j - 1) * tilesSide + tilesSide / 2, x0 + j * tilesSide, x0 + j * tilesSide};
							Sint16 py [] = {y0 + i * tilesSide, y0 + (i - 1) * tilesSide + tilesSide / 2, y0 + i * tilesSide};
							filledPolygonColor(target, px, py, 3, col);
							}
							
						//Checks if up-left concave transition is needed
						if (tiles[(i - 1) * w + j - 1].id != tiles[i * w + j].id &&
							tiles[(i - 1) * w + j].id == tiles[i * w + j].id &&
							tiles[i * w + j - 1].id == tiles[i * w + j].id &&
							tiles[i * w + j].layer > tiles[(i - 1) * w + j - 1].layer &&
							tiles[i * w + j].getTransition("t_0111")){
							Sint16 px [] = {x0 + (j - 1) * tilesSide, x0 + (j - 1) * tilesSide + tilesSide / 2, x0 + j * tilesSide, x0 + j * tilesSide, x0 + (j - 1) * tilesSide};
							Sint16 py [] = {y0 + (i - 1) * tilesSide + tilesSide / 2, y0 + (i - 1) * tilesSide, y0 + (i - 1) * tilesSide, y0 + i * tilesSide, y0 + i * tilesSide};
							filledPolygonColor(target, px, py, 5, col);
						}
					}
					
					if (i > 0){//If not in the first row
						//Checks if up transition is needed
						if ((j == 0 || tiles[(i - 1) * w + (j - 1)].id != tiles[i * w + j].id) &&
							tiles[(i - 1) * w + j].id != tiles[i * w + j].id &&
							(j == w - 1 || tiles[(i - 1) * w + (j + 1)].id != tiles[i * w + j].id) &&
							tiles[i * w + j].layer > tiles[(i - 1) * w + j].layer &&
							tiles[i * w + j].getTransition("t_0011")){
							Sint16 px [] = {x0 + j * tilesSide, x0 + (j + 1) * tilesSide, x0 + (j + 1) * tilesSide, x0 + j * tilesSide};
							Sint16 py [] = {y0 + (i - 1) * tilesSide + tilesSide / 2, y0 + (i - 1) * tilesSide + tilesSide / 2, y0 + i * tilesSide, y0 + i * tilesSide};
							filledPolygonColor(target, px, py, 4, col);
						}
					}
					
					if (i > 0 && j < w - 1){//If not in the first row or last column
						//Checks if up-right transition is needed
						if (tiles[(i - 1) * w + j + 1].id != tiles[i * w + j].id &&
							tiles[(i - 1) * w + j].id != tiles[i * w + j].id &&
							tiles[i * w + j + 1].id != tiles[i * w + j].id &&
							tiles[i * w + j].layer > tiles[(i - 1) * w + j + 1].layer &&
							tiles[i * w + j].getTransition("t_0010")){
							Sint16 px [] = {x0 + (j + 1) * tilesSide, x0 + (j + 1) * tilesSide + tilesSide / 2, x0 + (j + 1) * tilesSide};
							Sint16 py [] = {y0 + (i - 1) * tilesSide + tilesSide / 2, y0 + i * tilesSide, y0 + i * tilesSide};
							filledPolygonColor(target, px, py, 3, col);
						}
							
						//Checks if up-right concave transition is needed
						if (tiles[(i - 1) * w + j + 1].id != tiles[i * w + j].id &&
							tiles[(i - 1) * w + j].id == tiles[i * w + j].id &&
							tiles[i * w + j + 1].id == tiles[i * w + j].id &&
							tiles[i * w + j].layer > tiles[(i - 1) * w + j + 1].layer &&
							tiles[i * w + j].getTransition("t_1011")){
							Sint16 px [] = {x0 + (j + 1) * tilesSide, x0 + (j + 1) * tilesSide + tilesSide / 2, x0 + (j + 2) * tilesSide, x0 + (j + 2) * tilesSide, x0 + (j + 1) * tilesSide};
							Sint16 py [] = {y0 + (i - 1) * tilesSide, y0 + (i - 1) * tilesSide, y0 + (i - 1) * tilesSide + tilesSide / 2, y0 + i * tilesSide, y0 + i * tilesSide};
							filledPolygonColor(target, px, py, 5, col);
						}
					}
					
					if (j < w - 1){//If not in the last column
						//Checks if right transition is needed
						if ((i == 0 || tiles[(i - 1) * w + j + 1].id != tiles[i * w + j].id) &&
							tiles[i * w + j + 1].id != tiles[i * w + j].id &&
							(i == h - 1 || tiles[(i + 1) * w + j + 1].id != tiles[i * w + j].id) &&
							tiles[i * w + j].layer > tiles[i * w + j + 1].layer &&
							tiles[i * w + j].getTransition("t_1010")){
							Sint16 px [] = {x0 + (j + 1) * tilesSide, x0 + (j + 1) * tilesSide + tilesSide / 2, x0 + (j + 1) * tilesSide + tilesSide / 2, x0 + (j + 1) * tilesSide};
							Sint16 py [] = {y0 + i * tilesSide, y0 + i * tilesSide, y0 + (i + 1) * tilesSide, y0 + (i + 1) * tilesSide};
							filledPolygonColor(target, px, py, 4, col);
						}
					}
					
					if (i < h - 1 && j < w - 1){//If not in the last row or column
						//Checks if down-right transition is needed
						if (tiles[(i + 1) * w + j + 1].id != tiles[i * w + j].id &&
							tiles[(i + 1) * w + j].id != tiles[i * w + j].id &&
							tiles[i * w + j + 1].id != tiles[i * w + j].id &&
							tiles[i * w + j].layer > tiles[(i + 1) * w + j + 1].layer &&
							tiles[i * w + j].getTransition("t_1000")){
							Sint16 px [] = {x0 + (j + 1) * tilesSide, x0 + (j + 1) * tilesSide + tilesSide / 2, x0 + (j + 1) * tilesSide};
							Sint16 py [] = {y0 + (i + 1) * tilesSide, y0 + (i + 1) * tilesSide, y0 + (i + 1) * tilesSide + tilesSide / 2};
							filledPolygonColor(target, px, py, 3, col);
						}
						
						//Checks if down-right concave transition is needed
						if (tiles[(i + 1) * w + j + 1].id != tiles[i * w + j].id &&
							tiles[(i + 1) * w + j].id == tiles[i * w + j].id &&
							tiles[i * w + j + 1].id == tiles[i * w + j].id &&
							tiles[i * w + j].layer > tiles[(i + 1) * w + j + 1].layer &&
							tiles[i * w + j].getTransition("t_1110")){
							Sint16 px [] = {x0 + (j + 1) * tilesSide, x0 + (j + 2) * tilesSide, x0 + (j + 2) * tilesSide, x0 + (j + 1) * tilesSide + tilesSide / 2, x0 + (j + 1) * tilesSide};
							Sint16 py [] = {y0 + (i + 1) * tilesSide, y0 + (i + 1) * tilesSide, y0 + (i + 1) * tilesSide + tilesSide / 2, y0 + (i + 2) * tilesSide, y0 + (i + 2) * tilesSide};
							filledPolygonColor(target, px, py, 5, col);
						}
					}
					
					if (i < h - 1){//If not in the last row
						//Checks if down transition is needed
						if ((j == 0 || tiles[(i + 1) * w + (j - 1)].id != tiles[i * w + j].id) &&
							tiles[(i + 1) * w + j].id != tiles[i * w + j].id &&
							(j == w - 1 || tiles[(i + 1) * w + (j + 1)].id != tiles[i * w + j].id) &&
							tiles[i * w + j].layer > tiles[(i + 1) * w + j].layer &&
							tiles[i * w + j].getTransition("t_1100")){
							Sint16 px [] = {x0 + j * tilesSide, x0 + (j + 1) * tilesSide, x0 + (j + 1) * tilesSide, x0 + j * tilesSide};
							Sint16 py [] = {y0 + (i + 1) * tilesSide, y0 + (i + 1) * tilesSide, y0 + (i + 1) * tilesSide + tilesSide / 2, y0 + (i + 1) * tilesSide + tilesSide / 2};
							filledPolygonColor(target, px, py, 4, col);
						}
					}
					
					if (i < h - 1 && j > 0){//If not in the last row or first column
						//Checks if down-left transition is needed
						if (tiles[(i + 1) * w + j - 1].id != tiles[i * w + j].id &&
							tiles[(i + 1) * w + j].id != tiles[i * w + j].id &&
							tiles[i * w + j - 1].id != tiles[i * w + j].id &&
							tiles[i * w + j].layer > tiles[(i + 1) * w + j - 1].layer &&
							tiles[i * w + j].getTransition("t_0100")){
							Sint16 px [] = {x0 + (j - 1) * tilesSide + tilesSide / 2, x0 + j * tilesSide, x0 + j * tilesSide};
							Sint16 py [] = {y0 + (i + 1) * tilesSide, y0 + (i + 1) * tilesSide, y0 + (i + 1) * tilesSide + tilesSide / 2};
							filledPolygonColor(target, px, py, 3, col);
						}
							
						//Checks if down-left concave transition is needed
						if (tiles[(i + 1) * w + j - 1].id != tiles[i * w + j].id &&
							tiles[(i + 1) * w + j].id == tiles[i * w + j].id &&
							tiles[i * w + j - 1].id == tiles[i * w + j].id &&
							tiles[i * w + j].layer > tiles[(i + 1) * w + j - 1].layer &&
							tiles[i * w + j].getTransition("t_1101")){
							Sint16 px [] = {x0 + (j - 1) * tilesSide, x0 + j * tilesSide, x0 + j * tilesSide, x0 + (j - 1) * tilesSide + tilesSide / 2, x0 + (j - 1) * tilesSide};
							Sint16 py [] = {y0 + (i + 1) * tilesSide, y0 + (i + 1) * tilesSide, y0 + (i + 2) * tilesSide, y0 + (i + 2) * tilesSide, y0 + (i + 1) * tilesSide + tilesSide / 2};
							filledPolygonColor(target, px, py, 5, col);
						}
					}
				}
			}
			
			curLayer++;//Next layer
		}
	};
	
	//Function to make picture of the map
	void makePict(){
		_print(pict, pict->w / 2, pict->h / 2, true);//Prints map on picture
	}
	
	//Function to make minimap
	void makeMmap(){
		int x, y;//Counters
		
		//For each tile
		for (y = 0; y < h; y++){
			for (x = 0; x < w; x++){
				boxColor(mmap, x * 4, y * 4, (x + 1) * 4, (y + 1) * 4, (tiles[y * w + x].mmapColor << 8) + 255);//Prints tile
			}
		}
	}
	
	//Function to make transitions mask
	void makeTmap(){
		_printT(tmap, tmap->w / 2, tmap->h / 2, 0x000000FF);//Prints transitions on transitions mask
	}
	
	//Function to print picture of the map
	void print(SDL_Surface* target, int x, int y){
		//Prints base
		SDL_Rect offset = {x - pict->w / 2, y - pict->h / 2};//Offset rectangle
		SDL_BlitSurface(pict, NULL, target, &offset);//Prints picture
			
		//Prints units
		list<unit>::iterator u;//Unit iterator
		for (u = units.begin(); u != units.end(); u++){//For each unit in map
			u->print(target, offset.x + u->x, offset.y + u->y);//Prints unit
		}
	}
	
	//Function to print minimap
	void printMmap(SDL_Surface* target, int x, int y){
		SDL_Rect offset = {x - mmap->w / 2, y - mmap->h / 2};//Offset rectangle
		SDL_BlitSurface(mmap, NULL, target, &offset);//Prints minimap
	}
	
	//Function to set a tile according to given terrain id
	void setTile(unsigned int x, unsigned int y, string id, int layer = 0){
		if (x > w || y > h){//If one of the coords is invalid
			#ifdef _ERROR//If error handling file has been included
				if (errFunc) errFunc(MAP_INVALIDCOORDSERROR, "Invalid coordinates " + toString(x) + "," + toString(y) + " in map " + id);//Error function
			#endif
			
			return;//Exits function
		}
		
		terrain * t = get(&terrainDb, id);//Pointer to the requested terrain
		
		if (t){//If terrain exists
			tiles[y * w + x] = *t;//Sets terrain
			tiles[y * w + x].layer = layer;//Sets layer
		}
		
		//No need to call error function here, it is called by the get function (see gameHeaders.h)
	}
	
	//Function to get the tile under given coordinates (pixel-level, referring to top left corner)
	terrain* getTile(unsigned int x, unsigned int y){
		int tX = floor(x / tilesSide);//X coord of the tile
		int tY = floor(y / tilesSide);//Y coord of the tile
		
		if (tX < w && tY < h) return &tiles[tY * w + tX];//Returns pointer to tile
		else return NULL;//Returns null if coordinates are invalid
	}
		
	//Function to create new unit
	unit* createUnit(string id, string name, int x, int y){
		unit* u = get(&unitDb, id);//Gets unit
		
		if (u){//If unit exists
			unit newUnit = *u;//New unit
			
			newUnit.parent = this;
			newUnit.name = name;
			newUnit.x = x;
			newUnit.y = y;
			
			units.push_back(newUnit);//Adds new unit
			
			return &units.back();//Returns pointer to last unit
		}
		else if (errFunc) errFunc(DB_VARIABLEWARNING, "Unit " + id + " not found");
		
		return NULL;//Returns null if couldn't add unit
	}
	
	#ifdef _SCRIPT
		//Function to load from script object
		void fromScriptObj(object o){
			if (o.type == OBJTYPE_MAP){//If type is matching
				content::fromScriptObj(o);//Base content loading function
				
				var* w = o.getVar("w");//Width variable
				var* h = o.getVar("h");//Height variable
				var* tiles = o.getVar("tiles");//Tiles variable
				
				if (w && h) resize(atoi(w->value.c_str()), atoi(h->value.c_str()));//Sets map size
				
				char* c = (char*) tiles->value.c_str();//C-string version of tiles
				char* tok = strtok(c, ",:\t ");//First token
				int i = 0;//Counter
				
				while (tok){//While there's a valid token
					int x = i % this->w;//X coord of the tile
					int y = floor (i / this->w);//Y coord of the tile
					string tileId = tok;//Tile id
					
					tok = strtok(NULL, ",:\t ");//Next token
					int tileLayer = atoi(tok);//Tile layer
					
					setTile(x, y, tileId, tileLayer);//Sets tile
					
					tok = strtok(NULL, ",:\t ");//Next token
					i++;//Next tile
					
					if (i > this->w * this->h){//If gone past the map size
						#ifdef _ERROR
							if (errFunc) errFunc(DB_VARIABLEWARNING, "Too many terrains defined in tiles variable of " + id);
						#endif
						
						break;//Exits loop
					}
				}
				
				if (i < this->w * this->h){//If there are still some terrains to define
					#ifdef _ERROR
						if (errFunc) errFunc(DB_VARIABLEWARNING, "Not enough terrains defined in tiles variable of " + id);
					#endif
				}
				
				makePict();//Makes picture
				makeMmap();//Makes minimap
				makeTmap();//Makes transitions map
			}
			
			#ifdef _ERROR
				else if (errFunc) errFunc(SCRIPT_WARNING_INVALIDOBJTYPE, "[map.h] No map could be loaded from " + o.type + " object");
			#endif
		}
	#endif
	
	//Function to exec everything connected to timelapse
	void _time(){
		list<unit>::iterator i;//Iterator
		for (i = units.begin(); i != units.end(); i++) i->_time();//Execs time function for each unit
	}
	
	//Function to animate map
	void animate(){
		list<unit>::iterator i;//Iterator
		for (i = units.begin(); i != units.end(); i++) i->anims.next();//Next anim function for each unit
	}
} activeMap;

//Function to move unit
void unit::walk(int direction){
	this->direction = direction;//Sets direction
	
	switch (direction){
		case NORTH:
		anims.setAnim("walk_n");//Sets animation
		if (parent && y - speed <= margin) y = margin;//Handles out of map
		
		else if (parent){//Handles tiles
			Uint32 px = ((Uint32*) parent->tmap->pixels)[(y - speed - margin) * parent->tmap->w + x];//Pixel in next position
			if (px != 0x000000) y -= speed;
		}
		
		else y -= speed;
		break;
		
		case EAST:
		anims.setAnim("walk_e");//Sets animation
		if (parent && x + speed >= parent->width() * tilesSide - margin) x = parent->width() * tilesSide - margin;
		
		else if (parent){//Handles tiles
			Uint32 px = ((Uint32*) parent->tmap->pixels)[y * parent->tmap->w + x + speed + margin];//Pixel in next position
			if (px != 0x000000) x += speed;
		}
		
		else x += speed;
		break;
		
		case SOUTH:
		anims.setAnim("walk_s");//Sets animation
		
		if (parent && y + speed >= parent->height() * tilesSide - margin) y = parent->height() * tilesSide - margin;//Handles out of map
		
		else if (parent){//Handles tiles
			Uint32 px = ((Uint32*) parent->tmap->pixels)[(y + speed + margin) * parent->tmap->w + x];//Pixel in next position
			if (px != 0x000000) y += speed;
		}
		
		else y += speed;
		break;
		
		case WEST:
		anims.setAnim("walk_w");//Sets animation
		
		if (parent && x - speed <= margin) x = margin;//Handles out of map
		
		else if (parent){//Handles tiles
			Uint32 px = ((Uint32*) parent->tmap->pixels)[y * parent->tmap->w + x - speed - margin];//Pixel in next position
			if (px != 0x000000) x -= speed;
		}
		
		else x -= speed;
		break;
		
		default:
		anims.setAnim("idle_s");//Sets default animation if wrong direction was given
	}
}

list <map> mapDb;//Maps database

//Function to load database from object
void loadDatabase(object o){
	deque<object>::iterator i;//Iterator
	
	for (i = o.o.begin(); i != o.o.end(); i++){//For each sub-object
		if (i->type == OBJTYPE_TERRAIN){//If object is a terrain
			terrain newTerrain;//New terrain
			newTerrain.fromScriptObj(*i);//Loads terrain
			terrainDb.push_back(newTerrain);//Adds terrain to database
		}
		
		if (i->type == OBJTYPE_MAP){//If object is a map
			map newMap;//New map
			newMap.fromScriptObj(*i);//Loads map
			mapDb.push_back(newMap);//Adds map to database
		}
		
		if (i->type == OBJTYPE_UNIT){//If object is an unit
			unit newUnit;//New unit
			newUnit.fromScriptObj(*i);//Loads unit
			unitDb.push_back(newUnit);//Adds unit to database
		}
	}
}

//Initialization function
void game_init(string dbFile, string settingsFile, string mapId){
	uiInit();
	image_init();
		
	initWindow(800, 600, "Tales of Solgonya");
	
	fileData f (dbFile);//File data for database
	loadDatabase(f.objGen("db"));//Loads database
	
	fileData s (settingsFile);//Settings file
	object settings = s.objGen("settings");//Loads settings
	
	if (settings.getVar("font_global")) globalFont = TTF_OpenFont(settings.getVar("font_global")->value.c_str(), 12);//Opens global font
	
	activeMap = *get(&mapDb, mapId);//Loads map
}

#endif