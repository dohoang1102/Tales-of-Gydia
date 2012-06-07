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
#define OBJTYPE_MAP			"map"//Object type map

#define SELCOLOR			0xFFFFFF7A

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
	Uint32 mmapColor;//Color in minimap
	
	//Constructor
	terrain(){
		id = "";
		author = "";
		description = "";
		layer = 0;
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
				
				var* mmapColor = o.getVar("mmapColor");//Minimap color variable
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
	int x, y;//Unit position
	animSet anims;//Unit animations
	
	//Constructor
	unit(){
		x = 0;
		y = 0;
	}
	
	//Function to print unit
	void print(SDL_Surface* target, int x, int y){
		
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
	
	unsigned int selX, selY;//Selected tile (-1 to deselect)
	
	//Constructor
	map(){
		id = "";
		author = "";
		description = "";
		
		tiles = NULL;
		pict = NULL;
		mmap = NULL;
		
		resize(1,1);
				
		selX = -1;
		selY = -1;
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
	}
	
	//Function to get map width
	int width() {return w;}
	
	int height() {return h;}
	
	//Function to print the map
	void _print(SDL_Surface* target, int x, int y){
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
	}
	
	//Function to make picture of the map
	void makePict(){
		_print(pict, pict->w / 2, pict->h / 2);//Prints map on picture
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
	
	//Function to print picture of the map
	void print(SDL_Surface* target, int x, int y){
		SDL_Rect offset = {x - pict->w / 2, y - pict->h / 2};//Offset rectangle
		SDL_BlitSurface(pict, NULL, target, &offset);//Prints picture
		
		if (selX != -1 && selY != -1 && selX < w && selY < h){//If there's valid selected tile
			rectangleColor(target, selX * tilesSide, selY * tilesSide, (selX + 1) * tilesSide, (selY + 1) * tilesSide, SELCOLOR);
			rectangleColor(target, selX * tilesSide + 1, selY * tilesSide + 1, (selX + 1) * tilesSide - 1, (selY + 1) * tilesSide - 1, SELCOLOR);
			boxColor(target, selX * tilesSide, selY * tilesSide, (selX + 1) * tilesSide, (selY + 1) * tilesSide, (SELCOLOR & 0xFFFFFF00) + (SELCOLOR & 0xFF) / 2);
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
			}
			
			#ifdef _ERROR
				else if (errFunc) errFunc(SCRIPT_WARNING_INVALIDOBJTYPE, "[map.h] No map could be loaded from " + o.type + " object");
			#endif
		}
	#endif
} activeMap;

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
	}
}

//Initialization function
void game_init(string dbFile, string mapId){
	uiInit();
	image_init();
	
	initWindow(800, 600, "Tales of Solgonya");
	
	fileData f (dbFile);//File data for database
	loadDatabase(f.objGen("db"));//Loads database
	
	activeMap = *get(&mapDb, mapId);//Loads map
}

#endif