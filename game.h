//Game handling header

#ifndef _MAP
#define _MAP

#include "error.h"
#include "cache.h"
#include "script.h"
#include "locale.h"

#include "image.h"
#include "ui.h"

#include <string>
#include <deque>
#include <list>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

int CONTENT_INVALIDIDERROR = getErrorCode();//Code for invalid content request

int MAP_ERROR				= getErrorCode();//Generic map error
int MAP_INVALIDCOORDSERROR	= getErrorCode();//Invalid coordinates error
int MAP_WARNING 			= getErrorCode();//Generic map warning

int DB_VARIABLEWARNING		= getErrorCode();//Non-lethal variable error in database object

#define OBJTYPE_DECO		"deco"//Object type deco
#define OBJTYPE_TERRAIN		"terrain"//Object type terrain
#define OBJTYPE_WEAPON		"weapon"//Object type weapon
#define OBJTYPE_UNIT		"unit"//Object type unit
#define OBJTYPE_MAP			"map"//Object type map

#define MINIMAP_RES			4//Minimap resolution (square size)
#define MINIMAP_USIZE		6//Minimap unit size
#define MINIMAP_UCOLOR_0	0x0040FFF0//Minimap unit color for side 0
#define MINIMAP_UCOLOR_1	0xFF4000F0//Minimap unit color for side 1
#define MINIMAP_UCOLOR_2	0x40FF00F0//Minimap unit color for side 2
#define MINIMAP_UCOLOR_3	0xFF0040F0//Minimap unit color for side 3 (or higher)

#define MINIMAP_UCOLOR(SIDE) (SIDE == 0 ? MINIMAP_UCOLOR_0 : (SIDE == 1 ? MINIMAP_UCOLOR_1 : (SIDE == 2 ? MINIMAP_UCOLOR_2 : MINIMAP_UCOLOR_3)))

#define MOTIONSTEP			4//Pixels of motion for each frame when units walk

//Directions
#define NORTH				0//North
#define WEST				1//West
#define SOUTH				2//South
#define EAST				3//East

//Action codes
//Second digit is effective code, first digit is for direction
#define ACT_IDLE			0x00//Idle
#define ACT_WALK			0x10//Walking
#define ACT_STRIKE			0x20//Striking
#define ACT_DYING			0x30//Dying
#define ACT_DEAD			0x40//Dead

#define GETACODE(ACT,DIR)	((ACT) | (DIR))//Macro to get action and direction code out of separate action and direction
#define GETACT(CODE) 		((CODE) & 0xF0)//Macro to get action code out of code with action and direction
#define GETDIR(CODE)		((CODE) % 0x10)//Macro to get direction code out of code with action and direction

class map;//Map class prototype
class world;//World class prototype

typedef struct {int code; string args;} order;//Order structure

int tilesSide = 32;//Side of each tile of the map

TTF_Font* globalFont = NULL;//Global font

int turn = 0;

//Function to convert direction in string
string dirToString (int dir){
	switch (dir){
		case NORTH: return "n";
		case WEST:	return "w";
		case SOUTH:	return "s";
		case EAST:	return "e";
	}
}

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

//Function to get a content with a specific id from a content list
template <class contentType> contentType* get(list<contentType> *source, string id){
	typename list<contentType>::iterator i;//Iterator
	
	for (i = source->begin(); i != source->end(); i++)//For each element in source deque
		if (i->id == id) return &*i;//Returns element if matching
		
	if (errFunc) errFunc(CONTENT_INVALIDIDERROR, "Content " + id + " not found in content list");//Error message	
	return NULL;//Returns a null pointer if no element was found
}

//Function to get a content with a specific id from a content deque
template <class contentType> contentType* get(deque<contentType> *source, string id){
	typename deque<contentType>::iterator i;//Iterator
	
	for (i = source->begin(); i != source->end(); i++)//For each element in source deque
		if (i->id == id) return &*i;//Returns element if matching
		
	if (errFunc) errFunc(CONTENT_INVALIDIDERROR, "Content " + id + " not found in content list");//Error message	
	return NULL;//Returns a null pointer if no element was found
}

//Function to get a content with a specific id from a pointer-to-content list
template <class contentType> contentType* get_ptr(list<contentType*> *source, string id){
	typename list<contentType*>::iterator i;//Iterator
	
	for (i = source->begin(); i != source->end(); i++)//For each element in source deque
		if ((*i)->id == id) return *i;//Returns element if matching
		
	if (errFunc) errFunc(CONTENT_INVALIDIDERROR, "Content " + id + " not found in content list");//Error message	
	return NULL;//Returns a null pointer if no element was found
}

//Function to get a content with a specific id from a content deque
template <class contentType> contentType* get_ptr(deque<contentType*> *source, string id){
	typename deque<contentType*>::iterator i;//Iterator
	
	for (i = source->begin(); i != source->end(); i++)//For each element in source deque
		if ((*i)->id == id) return *i;//Returns element if matching
		
	if (errFunc) errFunc(CONTENT_INVALIDIDERROR, "Content " + id + " not found in content list");//Error message	
	return NULL;//Returns a null pointer if no element was found
}

//Decoration class
class deco: public content{
	public:
	image i;//Decoration image
	bool free;//Free flag: can walk on deco
	int x, y;//Coords of deco
	
	//Constructor
	deco(){
		free = true;
		x = 0;
		y = 0;
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_DECO){//If type matches
			content::fromScriptObj(o);//Loads content base data
			
			var* free = o.getVar("free");//Free flag
			object* i = o.getObj("image");//Image flag
			
			if (free) this->free = free->intValue();//Gets free flag
			if (i) this->i.fromScriptObj(*i);//Gets image
		}
	}
	
	//Print function
	void print(SDL_Surface* target, int x, int y){
		i.print(target, x, y);
	}
};

//Function determining if a deco comes before another one (lower y or equal y and lower x)
bool sortDecos_compare(deco* a, deco* b){
	return a->y < b->y || (a->y == b->y && a->x < b->x);
}

list <deco> decoDb;//Decos database

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
			}
		}
		
		else {//Else
			baseImage.print(target, x, y);//Prints image
		}
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

//Weapon class
class weapon: public content{
	public:
	//Stats
	int damageMin, damageMax;//Minimum and maximum damage (effective damage is a random number between these)
	bool image;//Flag indicating if image was given
	
	//Graphics
	animSet overlay;//Weapon overlay (shown when striking)
	string strikeAnim;//Player strike animation
	
	//Constructor
	weapon(){
		id = "";
		author = "";
		description = "";
		
		damageMin = 1;
		damageMax = 1;
		
		strikeAnim = "";
		image = false;
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_WEAPON){//If types are matching
			content::fromScriptObj(o);//Loads base content data
			
			var* damageMin = o.getVar("damageMin");//Minimum damage variable
			var* damageMax = o.getVar("damageMax");//Maximum damage variable
			object* overlay = o.getObj("overlay");//Weapon overlay object
			var* strikeAnim = o.getVar("strikeAnim");//Strike animation variable
			
			if (damageMin) this->damageMin = damageMin->intValue();//Gets minimum damage
			if (damageMax) this->damageMax = damageMax->intValue();//Gets maximum damage
			if (overlay){ this->overlay.fromScriptObj(*overlay); image = true;}//Loads overlay
			if (strikeAnim) this->strikeAnim = strikeAnim->value;//Gets strike animation
		}
	}
	
	//Function to get effective damage
	int damage(){
		if (damageMin != damageMax) return damageMin + (rand() % (damageMax - damageMin));
		else return damageMax;
	}
	
	//Function to print weapon
	void print(SDL_Surface* target, int x, int y){
		if (image) overlay.print(target,x,y);//Prints image if given
	}
};

list <weapon> weaponDb;//Weapons database

//Unit class
class unit: public content{
	public:
	//Unit position
	int x, y;//Unit position
	int dX, dY;//X and Y variations (for walking animations)
	map* parent;//Parent map
	
	//Action
	int action;//Action & direction code
	
	//Graphics
	animSet anims;//Unit animations
	
	//General information
	string name;//Name of unit
	int side;//Unit side
	
	//Status
	int hits, maxHits;//Unit hits and max hits
	
	//Fighting
	weapon *primary;//Primary melee weapon
	
	//Constructor
	unit(){
		id = "";
		author = "";
		description = "";
		
		dX = 0;
		dY = 0;
		
		x = 0;
		y = 0;
		parent = NULL;
		
		action = GETACODE(ACT_IDLE, SOUTH);
		
		name = "";
		side = 0;
		
		hits = 1;
		maxHits = 1;
		
		primary = NULL;
	}
	
	//Unit printing function
	void print(SDL_Surface* target, int x, int y){
		//Prints hitbar
		if (hits > 0 && maxHits > 0){
			SDL_Rect hitBar = {x + anims.current()->current()->cShiftX - tilesSide / 2 + dX + 2, y + anims.current()->current()->cShiftY + anims.current()->current()->rect.h / 2 + dY, hits * (tilesSide - 4) / maxHits, 4};
			boxColor(target, hitBar.x, hitBar.y, hitBar.x + hitBar.w, hitBar.y + hitBar.h, 0xA000007A);
		}
		
		anims.print(target, x + dX, y + dY);//Prints using animSet printing function (prints current frame of current animation)
		if (GETACT(action) == ACT_STRIKE && primary && primary->image) primary->overlay.print(target, x + dX, y + dY);//Prints weapon
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_UNIT){//If type is matching
			content::fromScriptObj(o);//Loads base content data
			
			object* anims = o.getObj("anims");//AnimSet object
			var* hits = o.getVar("hits");//Hits variable
			
			if (anims) this->anims.fromScriptObj(*anims);//Loads animations
			if (hits) this->maxHits = hits->intValue();//Gets max hits
			
			this->anims.setAnim("idle_s");//Turns south
			this->hits = this->maxHits;//Sets hits
		}
	}
	
	//Function to set idle
	void stop(){
		action = GETACODE(ACT_IDLE, GETDIR(action));//Sets action code
		anims.setAnim("idle_" + dirToString(GETDIR(action)));//Sets animation
	}
	
	//Walking function
	void walk(int);
	
	//Striking function
	void strike(){
		if (primary){//If weapon is valid
			action = GETACODE(ACT_STRIKE, GETDIR(action));//Sets action code
			anims.setAnim(primary->strikeAnim + "_" + dirToString(GETDIR(action)));//Sets striking animation
			
			if (primary->image){
				primary->overlay.setAnim("strike_" + dirToString(GETDIR(action)));//Sets weapon animation
				primary->overlay.current()->curFrame = 0;//Restarts animation
			}
		}
		
		else stop();
	}
	
	//Function to kill unit
	void kill(){
		action = ACT_DYING;//Sets action code
		anims.setAnim("dying");//Sets animation
	}
	
	//Function to go to next frame
	void nextFrame();
	
	//Function returning if unit is ready
	bool ready(){
		return GETACT(action) == ACT_IDLE;
	}
	
	//Function to exec order
	void execOrder(order o){
		if (GETACT(o.code) == ACT_WALK) walk(GETDIR(o.code));//Checks for walking orders
		else if (GETACT(o.code) == ACT_STRIKE) strike();//Checks for striking orders
		else stop();//Stops on any other code
	}
	
	//Function to damage unit
	void damage(int amount){
		hits -= amount;
		if (hits <= 0) kill();
	}
	
	//Function to give primary melee weapon to unit
	void giveWeapon_primary(string id){
		primary = get(&weaponDb, id);//Sets weapon
	}
};

//Function determining if an unit comes before another one (lower y or equal y and lower x)
bool sortUnits_compare(unit* a, unit* b){
	return a->y < b->y || (a->y == b->y && a->x < b->x);
}

list<unit> unitDb;//Units database

//Function calculating distance between units
double uDist(unit* a, unit* b){
	return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}

//Controller class
class controller{
	public:
	unit* u;//Controlled unit
	
	//Function to give order to all controlled units
	void giveOrder(order o){
		u->execOrder(o);
	}
	
	//Function to get input from keyboard
	bool getInput(){
		Uint8* keys = SDL_GetKeyState(NULL);//Gets key states
		
		//Walking orders
		if (keys[SDLK_UP]){ giveOrder({GETACODE(ACT_WALK, NORTH), ""}); return true;}
		else if (keys[SDLK_LEFT]){ giveOrder({GETACODE(ACT_WALK, WEST), ""}); return true;}
		else if (keys[SDLK_DOWN]){ giveOrder({GETACODE(ACT_WALK, SOUTH), ""}); return true;}
		else if (keys[SDLK_RIGHT]){ giveOrder({GETACODE(ACT_WALK, EAST), ""}); return true;}
		
		//Striking orders
		else if (keys[SDLK_SPACE]){ giveOrder({ACT_STRIKE, ""}); return true;}
		
		return false;
	}
	
	//Function to get if all controlled units are ready
	bool ready(){
		return u->ready();
	}
};

//Function to compare unit and deco according to position
bool compare_unitDeco(unit* a, deco* b){
	return a->y < b->y || (a->y == b->y && a->x < b->x);
}

//Map class
class map: public content{
	private:
	int w, h;//Size of the map
	
	public:
	terrain* tiles;//Tiles of the map
	
	SDL_Surface* pict;//Picture of the map
	SDL_Surface* mmap;//Picture of the minimap
	SDL_Surface* tmap;//Picture of the transitions mask
	
	deque<unit*> units;//Units on map
	deque<deco*> decos;//Decorations on map
	
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
		mmap = SDL_CreateRGBSurface(SDL_SWSURFACE, w * MINIMAP_RES, h * MINIMAP_RES, 32, 0, 0, 0, 0);//Creates minimap
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
	
	//Function to make picture of the map
	void makePict(bool grid = false){
		_print(pict, pict->w / 2, pict->h / 2, grid);//Prints map on picture
	}
	
	//Function to make minimap
	void makeMmap(int res = MINIMAP_RES){
		int x, y;//Counters
		
		//For each tile
		for (y = 0; y < h; y++){
			for (x = 0; x < w; x++){
				boxColor(mmap, x * res, y * res, (x + 1) * res, (y + 1) * res, (tiles[y * w + x].mmapColor << 8) + 255);//Prints tile
			}
		}
	}
	
	//Function to sort units according to position
	void sortUnits(){
		sort(units.begin(), units.end(), sortUnits_compare);//Sorts units
	}
	
	//Function to sort decos according to position
	void sortDecos(){
		sort(decos.begin(), decos.end(), sortDecos_compare);//Sorts decos
	}
	
	//Function to create an unit on map
	unit* createUnit(string id, string name, int x, int y, int side){
		unit* u = get(&unitDb, id);//Required unit
		
		if (u && isFree(x, y)){//If unit exists and tile is free
			unit* newUnit = new unit;//Unit to add
			*newUnit = *u;//Copies unit
			
			//Sets unit data
			newUnit->name = name;
			newUnit->x = x;
			newUnit->y = y;
			newUnit->side = side;
			newUnit->parent = this;
			
			units.push_back(newUnit);//Adds unit to map
			sortUnits();//Sorts map units
			return newUnit;//Returns new unit
		}
		
		return NULL;//Returns null if failed
	}
	
	//Function to create a deco on map
	deco* createDeco(string id, int x, int y){
		deco* d = get(&decoDb, id);//Required deco
		
		if (d && isFree(x, y)){//If deco exists and tile is free
			deco* newDeco = new deco;//Deco to add
			*newDeco = *d;//Copies deco
			
			//Sets deco data
			newDeco->x = x;
			newDeco->y = y;
			
			decos.push_back(newDeco);//Adds deco to map
			sortDecos();//Sorts map decos
			return newDeco;//Returns new deco
		}
		
		return NULL;//Returns null if failed
	}
	
	//Function to remove a deco from map
	void removeDeco(int x, int y){
		deque<deco*>::iterator i;//Iterator
		
		for (i = decos.begin(); i != decos.end(); i++){//For each deco
			if ((*i)->x == x && (*i)->y == y){//If position matches
				decos.erase(i);//Erases deco
				return;//Exits function
			}
		}
	}
	
	//Function to print picture of the map
	void print(SDL_Surface* target, int x, int y, unit* c = NULL){
		SDL_Rect offset;//Offset rectangle
		
		//Prints base		
		if (!c){//If no centre coords were given
			offset = {x - pict->w / 2, y - pict->h / 2};//Offset rectangle
		}
		
		else {//Else
			offset = {x - tilesSide * c->x - c->dX - tilesSide / 2, y - tilesSide * c->y - c->dY - tilesSide / 2};
		}
		SDL_Rect o = offset;//Copies offset rectangle (it gets modified by SDL_Blit, but will be still needed in unit and deco prints)
		SDL_BlitSurface(pict, NULL, target, &o);//Prints picture
		
		deque<unit*>::iterator u = units.begin();//Unit iterator
		deque<deco*>::iterator d = decos.begin();//Deco iterator
		
		while (u != units.end() || d != decos.end()){//While there's still something to print
			if (u != units.end() && compare_unitDeco(*u, *d)){//If unit is before deco
				(*u)->print(target, offset.x + tilesSide * (*u)->x + tilesSide / 2, offset.y + tilesSide * (*u)->y + tilesSide / 2);//Prints unit
				u++;//Next unit
			}
			
			else if (d != decos.end()){//Else
				(*d)->print(target, offset.x + tilesSide * (*d)->x + tilesSide / 2, offset.y + tilesSide * (*d)->y + tilesSide / 2);//Prints deco
				d++;//Next deco
			}
		}
	}
	
	//Function to print minimap
	void printMmap(SDL_Surface* target, int x, int y){
		SDL_Rect offset = {x - mmap->w / 2, y - mmap->h / 2};//Offset rectangle
		SDL_BlitSurface(mmap, NULL, target, &offset);//Prints minimap
		
		deque <unit*>::iterator u;//Unit iterator
		for (u = units.begin(); u != units.end(); u++)//For each unit
			boxColor(target, (*u)->dX * MINIMAP_RES / tilesSide + offset.x + (*u)->x * MINIMAP_RES + (MINIMAP_RES - MINIMAP_USIZE) / 2, (*u)->dY * MINIMAP_RES / tilesSide + offset.y + (*u)->y * MINIMAP_RES + (MINIMAP_RES - MINIMAP_USIZE) / 2, (*u)->dX * MINIMAP_RES / tilesSide + offset.x + (*u)->x * MINIMAP_RES + (MINIMAP_RES + MINIMAP_USIZE) / 2, (*u)->dY * MINIMAP_RES / tilesSide + offset.y + (*u)->y * MINIMAP_RES + (MINIMAP_RES + MINIMAP_USIZE) / 2, MINIMAP_UCOLOR((*u)->side));
	}
	
	//Function to set a tile according to given terrain id
	void setTile(unsigned int x, unsigned int y, string id, int layer = 0){
		if (x >= w || y >= h){//If one of the coords is invalid
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
	terrain* getTile(int x, int y){
		if (x >= 0 && y >= 0 && x < w && y < h) return &tiles[y * w + x];//Returns pointer to tile
		else return NULL;//Returns null if coordinates are invalid
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_MAP){//If type is matching
			content::fromScriptObj(o);//Base content loading function
			
			var* w = o.getVar("w");//Width variable
			var* h = o.getVar("h");//Height variable
			var* tiles = o.getVar("tiles");//Tiles variable
			var* decos = o.getVar("decos");//Decos variable
			
			if (w && h) resize(atoi(w->value.c_str()), atoi(h->value.c_str()));//Sets map size
			
			if (tiles){//If tiles variable was given
				char* c = (char*) tiles->value.c_str();//C-string version of tiles
				char* tok = strtok(c, ",\t ");//First token
				int i = 0;//Counter
				
				while (tok){//While there's a valid token
					string tInfo = tok;//Tile info string
					
					int dots = tInfo.find(":");//Dots position
					string tId = tInfo.substr(0, dots);//Tile id
					int tLayer = atoi(tInfo.substr(dots + 1).c_str());//Tile layer
					
					//Tile coords
					int tX = i % this->w;
					int tY = floor(i / this->w);
					
					setTile(tX, tY, tId, tLayer);
					
					tok = strtok(NULL, ",\t ");//Next token
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
			}
			
			if (decos){//If decos variable was given
				char* c = (char*) decos->value.c_str();//C-string version of decos
				char* tok = strtok(c, ",\t ");//First token
				
				while (tok){//While there's a valid token
					string dInfo = tok;//Deco info string
					int firstSep = dInfo.find(":");//First dots
					int lastSep = dInfo.rfind(":");//Last dots
					
					string dId = dInfo.substr(0, firstSep);//Deco id
					int dX = atoi(dInfo.substr(firstSep + 1, lastSep - firstSep - 1).c_str());//Deco x
					int dY = atoi(dInfo.substr(lastSep + 1).c_str());//Deco y
					
					createDeco(dId, dX, dY);//Creates deco
					
					tok = strtok(NULL, ",\t ");//Next token
				}
			}
			
			makePict();//Makes picture
			makeMmap();//Makes minimap
		}
		
		#ifdef _ERROR
			else if (errFunc) errFunc(SCRIPT_WARNING_INVALIDOBJTYPE, "[map.h] No map could be loaded from " + o.type + " object");
		#endif
	}
	
	//Function returning if tile is free
	bool isFree(int x, int y){
		if (getTile(x, y)){//If terrain exists
			terrain* tile = getTile(x, y);//Gets terrain
			
			//Checks if terrain is not free or there's a transition from a non-free terrain
			if (!tile->free) return false;
			if (getTile(x, y - 1) && !getTile(x, y - 1)->free && getTile(x, y - 1)->layer > tile->layer) return false;
			if (getTile(x + 1, y - 1) && !getTile(x + 1, y - 1)->free && getTile(x + 1, y - 1)->layer > tile->layer) return false;
			if (getTile(x + 1, y) && !getTile(x + 1, y)->free && getTile(x + 1, y)->layer > tile->layer) return false;
			if (getTile(x + 1, y + 1) && !getTile(x + 1, y + 1)->free && getTile(x + 1, y + 1)->layer > tile->layer) return false;
			if (getTile(x, y + 1) && !getTile(x, y + 1)->free && getTile(x, y + 1)->layer > tile->layer) return false;
			if (getTile(x - 1, y + 1) && !getTile(x - 1, y + 1)->free && getTile(x - 1, y + 1)->layer > tile->layer) return false;
			if (getTile(x - 1, y) && !getTile(x - 1, y)->free && getTile(x - 1, y)->layer > tile->layer) return false;
			if (getTile(x - 1, y - 1) && !getTile(x - 1, y - 1)->free && getTile(x - 1, y - 1)->layer > tile->layer) return false;
			
			deque<unit*>::iterator u;//Unit iterator
			for (u = units.begin(); u != units.end(); u++)//For each unit
				if ((*u)->x == x && (*u)->y == y) return false;//If unit is in tile returns false
				
			deque<deco*>::iterator d;//Deco iterator
			for (d = decos.begin(); d != decos.end(); d++)//For each deco
				if ((*d)->x == x && (*d)->y == y) return false;//If deco is in tile returns false
				
			return true;//Returns true if no unit was found
		}
		
		return false;//Returns false if tile is not in map
	}
	
	//Function to go to next frame
	void nextFrame(){
		int i;//Unit counter
		
		for (i = 0; i < units.size(); i++){//For each unit
			int oldSize = units.size();//Unit amount before this loop
			
			units[i]->nextFrame();//Unit next frame
			
			if (units.size() != oldSize) i -= oldSize - units.size();//Corrects counter position if deque size has changed
		}
	}
	
	//Function to damage unit in given tile
	void damage(int x, int y, int amount){
		deque<unit*>::iterator i;//Iterator
		
		for(i = units.begin(); i != units.end(); i++){//For each unit
			if ((*i)->x == x && (*i)->y == y){//If unit is in given position
				(*i)->damage(amount);//Damages unit for given amount
				break;//Exits loop
			}
		}
	}
	
	//Function to fill map with given tile at given layer
	void fill(string tId, int layer){
		int x, y;//Tile counters
		
		for (y = 0; y < h; y++)
			for (x = 0; x < w; x++)
				setTile(x, y, tId, layer);
	}
	
	//Function to convert to script object
	object toScriptObj(){
		object result;//Result
		
		//Sets name and type
		result.name = id;
		result.type = OBJTYPE_MAP;
		
		//Sets metadata
		result.setVar("author", author);
		result.setVar("description", description);
		
		//Sets size
		result.setVar("w", w);
		result.setVar("h", h);
		
		string tilesVar = "";//Tiles variable content
		int i;//Counter
		for (i = 0; i < w * h; i++) tilesVar += "," + tiles[i].id + ":" + toString(tiles[i].layer);//Writes tile info
		tilesVar.erase(0,1);//Deletes first comma in tiles variable
		
		string decosVar = "";//Decos variable content
		for (i = 0; i < decos.size(); i++) decosVar += "," + decos[i]->id + ":" + toString(decos[i]->x) + ":" + toString(decos[i]->y);//Writes deco info
		decosVar.erase(0,1);//Deletes first comma in decos variable
		
		result.setVar("tiles", tilesVar);//Sets tiles variable
		result.setVar("decos", decosVar);//Sets decos variable
		
		return result;//Returns result
	}
} activeMap;

list <map> mapDb;//Maps database

//Walking function
void unit::walk(int direction){
	if (parent){//If there's a reference map
		//Checks free tiles
		switch (direction){//According to direction
			case NORTH:
			if (!parent->isFree(x, y - 1)){
				action = GETACODE(ACT_IDLE, direction);
				stop();
				return;
			}
			break;
			
			case WEST:
			if (!parent->isFree(x - 1, y)){
				action = GETACODE(ACT_IDLE, direction);
				stop();
				return;
			}
			break;
			
			case SOUTH:
			if (!parent->isFree(x, y + 1)){
				action = GETACODE(ACT_IDLE, direction);
				stop();
				return;
			}
			break;
			
			case EAST:
			if (!parent->isFree(x + 1, y)){
				action = GETACODE(ACT_IDLE, direction);
				stop();
				return;
			}
			break;
		}
	}

	action = GETACODE(ACT_WALK, direction);//Sets action codes
	anims.setAnim("walk_" + dirToString(direction));//Sets walking animation
}

//Function to go to next frame
void unit::nextFrame(){
	anims.next();//Next animation frame
	
	if (GETACT(action) == ACT_WALK){//If unit is walking
		//Moves unit
		switch (GETDIR(action)){//According to direction
			case NORTH:
			dY -= MOTIONSTEP;
			if (dY <= -tilesSide){
				y--;
				dY = 0;
				stop();
				
				if (parent) parent->sortUnits();
			}
			break;
			
			case WEST:
			dX -= MOTIONSTEP;
			if (dX <= -tilesSide){
				x--;
				dX = 0;
				stop();
				
				if (parent) parent->sortUnits();
			}
			break;
			
			case SOUTH:
			dY += MOTIONSTEP;
			if (dY >= tilesSide){
				y++;
				dY = 0;
				stop();
				
				if (parent) parent->sortUnits();
			}
			break;
			
			case EAST:
			dX += MOTIONSTEP;
			if (dX >= tilesSide){
				x++;
				dX = 0;
				stop();
				
				if (parent) parent->sortUnits();
			}
			break;
		}
	}
	
	else if (GETACT(action) == ACT_STRIKE){//If unit is striking
		if (primary && primary->image) primary->overlay.next();//Next frame for weapon animation
		
		if (anims.current()->curFrame == anims.current()->duration() - 1) stop();//Stops if reached end of animation
		
		if (anims.current()->curFrame == floor(anims.current()->duration() / 2) - 1 && parent){//At half striking animation
			switch (GETDIR(action)){//According to direction
				case NORTH: parent->damage(x, y - 1, primary->damage()); break;
				case WEST: parent->damage(x - 1, y, primary->damage()); break;
				case SOUTH: parent->damage(x, y + 1, primary->damage()); break;
				case EAST: parent->damage(x + 1, y, primary->damage()); break;
			}
		}
	}
	
	else if (GETACT(action) == ACT_DYING){//If unit is dying
		if (anims.current()->curFrame == anims.current()->duration() - 1 && parent){//If reached end of animation
			deque<unit*>::iterator i;//Iterator for units
			
			action = ACT_DEAD;
			
			for (i = parent->units.begin(); i != parent->units.end(); i++){//For each unit in parent map
				if (*i == this){//If unit is this
					i = parent->units.erase(i);//Removes unit from map
					break;//Exits loop
				}
			}
		}
	}
}

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
		
		if (i->type == OBJTYPE_WEAPON){//If object is a weapon
			weapon newWeapon;//New weapon
			newWeapon.fromScriptObj(*i);//Loads weapon
			weaponDb.push_back(newWeapon);//Adds weapon to database
		}
		
		if (i->type == OBJTYPE_DECO){//If object is a decoration
			deco newDeco;//New decoration
			newDeco.fromScriptObj(*i);//Loads deco
			decoDb.push_back(newDeco);//Adds deco to database
		}
	}
}

//Initialization function
void game_init(string dbFile, string settingsFile){
	srand(time(NULL));//Randomize
		
	uiInit();//Initializes UI
	image_init();//Initializes image library
		
	initWindow(800, 600, "Tales of Gydia");//Window setup
	
	fileData f (dbFile);//File data for database
	loadDatabase(f.objGen("db"));//Loads database
		
	fileData s (settingsFile);//Settings file
	object settings = s.objGen("settings");//Loads settings
	
	if (settings.getVar("font_global")) globalFont = TTF_OpenFont(settings.getVar("font_global")->value.c_str(), 12);//Opens global font
}

#endif