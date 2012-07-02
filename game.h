//Game handling header

#ifndef _MAP
#define _MAP

#include "error.h"
#include "cache.h"
#include "script.h"
#include "expr.h"
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
#define OBJTYPE_EFFECT		"effect"//Object type effect
#define OBJTYPE_WEAPON		"weapon"//Object type weapon
#define OBJTYPE_CLOTH		"cloth"//Object type cloth
#define OBJTYPE_UNIT		"unit"//Object type unit
#define OBJTYPE_MAP			"map"//Object type map
#define OBJTYPE_EVENT		"event"//Object type event
#define OBJTYPE_SEQUENCE	"sequence"//Object type sequence
#define OBJTYPE_CAMPAIGN	"campaign"//Object type campaign

//Minimap graphics
#define MINIMAP_RES			4//Minimap resolution (square size)
#define MINIMAP_USIZE		3//Minimap unit size
#define MINIMAP_UCOLOR_0	0x0040FFF0//Minimap unit color for side 0
#define MINIMAP_UCOLOR_1	0xFF4000F0//Minimap unit color for side 1
#define MINIMAP_UCOLOR_2	0x40FF00F0//Minimap unit color for side 2
#define MINIMAP_UCOLOR_3	0xFF0040F0//Minimap unit color for side 3 (or higher)

#define MINIMAP_UCOLOR(SIDE) (SIDE == 0 ? MINIMAP_UCOLOR_0 : (SIDE == 1 ? MINIMAP_UCOLOR_1 : (SIDE == 2 ? MINIMAP_UCOLOR_2 : MINIMAP_UCOLOR_3)))

//Movement settings
#define MOTIONSTEP			4//Pixels of motion for each frame when units walk

//Event flags
#define EVENT_NORMAL		0b00000000//Normal event, triggered whenever control script is verified
#define EVENT_ONCE			0b00000001//Event triggered only the first time control script is verified

//Script results
#define VOID				-1
#define FALSE				0
#define TRUE				1

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
class campaign;//Campaign class prototype

typedef struct {int code; string args;} order;//Order structure

int tilesSide = 32;//Side of each tile of the map

TTF_Font* globalFont = NULL;//Global font
TTF_Font* labelsFont = NULL;//Floating labels font

int turn = 0;

deque<op> ops_bool;//Boolean script operators

//Boolean operators functions
string ops_bool_equal(string a, string b){ return toString(a == b); }
string ops_bool_greater(string a, string b){ return toString(atoi(a.c_str()) > atoi(b.c_str())); }
string ops_bool_less(string a, string b){ return toString(atoi(a.c_str()) < atoi(b.c_str())); }
string ops_bool_and(string a, string b){ return toString(atoi(a.c_str()) && atoi(b.c_str())); }
string ops_bool_or(string a, string b){ return toString(atoi(a.c_str()) || atoi(b.c_str())); }

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

//Effect class
class effect {
	public:
	int hits;//Hits variation
	int maxHits;//Max hits variation
	
	int strength;//Strength bonus
	int intelligence;//Intelligence bonus
	int constitution;//Constitution bonus
	int wisdom;//Wisdom bonus
	
	int duration;//Effect duration (0 instant/permanent, else duration in turns)
	
	//Constructor
	effect(){
		hits = 0;
		maxHits = 0;
		duration = 0;
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_EFFECT){//If type is matching
			var* hits = o.getVar("hits");//Hits variable
			var* maxHits = o.getVar("maxHits");//Max hits variable
			var* strength = o.getVar("strength");//Strength variable
			var* intelligence = o.getVar("intelligence");//Intelligence variable
			var* constitution = o.getVar("constitution");//Constitution variable
			var* wisdom = o.getVar("wisdom");//Wisdom variable
			var* duration = o.getVar("duration");//Duration variable
			
			//Gets values
			if (hits) this->hits = hits->intValue();
			if (maxHits) this->maxHits = maxHits->intValue();
			if (strength) this->strength = strength->intValue();
			if (intelligence) this->intelligence = intelligence->intValue();
			if (constitution) this->constitution = constitution->intValue();
			if (wisdom) this->wisdom = wisdom->intValue();
			if (duration) this->duration = duration->intValue();
		}
	}
};

//Cloth class
class cloth: public content{
	public:
	animSet overlay;//Cloth animations
	int layer;//Cloth layer (highest layer -> printed on top, layer = 0 -> printed below character)
	
	effect onEquip;//On-equip effect
	
	//Constructor
	cloth(){
		layer = 1;
	}
	
	//Printing function
	void print(SDL_Surface* target, int x, int y){
		overlay.print(target, x, y);
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_CLOTH){//If type is matching
			content::fromScriptObj(o);//Loads base content data
			
			object* overlay = o.getObj("overlay");//Gets overlay object
			var* layer = o.getVar("layer");//Gets layer variable
			
			object* onEquip = o.getObj("onEquip");//On-equip effect
			
			if (overlay) this->overlay.fromScriptObj(*overlay);//Loads overlay
			if (layer) this->layer = layer->intValue();//Gets layer
			if (onEquip) this->onEquip.fromScriptObj(*onEquip);//Gets on-equip effect
			
			this->overlay.setAnim("idle_s");
		}
	}
};

//Function to sort cloths according to layer
bool sortCloths_compare(cloth* a, cloth* b){
	if (a->layer < b->layer) return true;
	return false;
}

list <cloth> clothDb;//Cloths database

//Weapon class
class weapon: public content{
	public:
	//Graphics
	animSet overlay;//Weapon overlay (shown when striking)
	string strikeAnim;//Player strike animation
	int strikeFrame;//Frame of animation when unit strikes
	bool image;//Flag indicating if image was given
	
	//Effects
	effect onEquip;//On-equip effect
	effect onTarget;//On-target effect
	
	//Constructor
	weapon(){
		id = "";
		author = "";
		description = "";
				
		strikeAnim = "";
		strikeFrame = 0;
		image = false;
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_WEAPON){//If types are matching
			content::fromScriptObj(o);//Loads base content data
			
			object* overlay = o.getObj("overlay");//Weapon overlay object
			var* strikeAnim = o.getVar("strikeAnim");//Strike animation variable
			var* strikeFrame = o.getVar("strikeFrame");//Strike frame variable
			object* onEquip = o.getObj("onEquip");//On equip effect object
			object* onTarget = o.getObj("onTarget");//On target effct object
			
			if (overlay){ this->overlay.fromScriptObj(*overlay); image = true;}//Loads overlay
			if (strikeAnim) this->strikeAnim = strikeAnim->value;//Gets strike animation
			if (strikeFrame) this->strikeFrame = strikeFrame->intValue();//Gets strike frame
			if (onEquip) this->onEquip.fromScriptObj(*onEquip);//Loads on equip effect
			if (onTarget) this->onTarget.fromScriptObj(*onTarget);//Loads on target effect
		}
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
	
	//Stats
	int baseHits;//Unit's base hits (no effects, no stats applied)
	int baseMaxHits;//Unit's base max hits (no effects, no stats applied)
	int baseMana;//Unit's base mana
	int baseMaxMana;//Unit's base max mana
	
	int baseStrength;//Unit's strength
	int baseIntelligence;//Unit's intelligence
	int baseConstitution;//Unit's constitution
	int baseWisdom;//Unit's wisdom
	
	int sight;//Unit's sight
	
	list<effect> tempEffects;//Temporary effects
	
	//Fighting
	weapon *primary;//Primary melee weapon
	deque <cloth*> cloths;//Unit cloths
	
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
		
		baseHits = 1;
		baseMaxHits = 1;
		baseMana = 1;
		baseMaxMana = 1;
		baseStrength = 1;
		baseIntelligence = 1;
		baseConstitution = 1;
		baseWisdom = 1;
		sight = 5;
		
		primary = NULL;
	}
	
	//Unit printing function
	void print(SDL_Surface* target, int x, int y){
		deque<cloth*>::iterator i;//Iterator
		for (i = cloths.begin(); i != cloths.end(); i++)//For each cloth
			if ((*i)->layer == 0) (*i)->print(target, x + dX, y + dY);//Prints cloth if on layer 0
			else break;//Else exits loop
		
		//Prints hitbar
		if (hits() > 0 && maxHits() > 0){
			SDL_Rect hitBar = {x + anims.current()->current()->cShiftX - tilesSide / 2 + dX + 2, y + anims.current()->current()->cShiftY + anims.current()->current()->rect.h / 2 + dY, hits() * (tilesSide - 4) / maxHits(), 4};
			boxColor(target, hitBar.x, hitBar.y, hitBar.x + hitBar.w, hitBar.y + hitBar.h, 0xA000007A);
		}
		
		anims.print(target, x + dX, y + dY);//Prints using animSet printing function (prints current frame of current animation)
		if (GETACT(action) == ACT_STRIKE && primary && primary->image) primary->overlay.print(target, x + dX, y + dY);//Prints weapon
	
		for (;i != cloths.end(); i++) (*i)->print(target, x + dX, y + dY);//Prints remaining cloths
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_UNIT){//If type is matching
			content::fromScriptObj(o);//Loads base content data
			
			object* anims = o.getObj("anims");//AnimSet object
			var* hits = o.getVar("hits");//Hits variable
			var* sight = o.getVar("sight");//Sight variable
			
			if (anims) this->anims.fromScriptObj(*anims);//Loads animations
			if (hits) this->baseMaxHits = hits->intValue();//Gets max hits
			if (sight) this->sight = sight->intValue();//Gets sight
			
			this->anims.setAnim("idle_s");//Turns south
			this->baseHits = this->baseMaxHits;//Sets hits
		}
	}
	
	//Function to set idle
	void stop(){
		action = GETACODE(ACT_IDLE, GETDIR(action));//Sets action code
		anims.setAnim("idle_" + dirToString(GETDIR(action)));//Sets animation
		setClothsAnim("idle_" + dirToString(GETDIR(action)));//Sets cloths animation
	}
	
	//Walking function
	void walk(int);
	
	//Turning function (faces given direction)
	void turn(int direction){
		action = GETACODE(ACT_IDLE, direction);//Sets action code
		anims.setAnim("idle_" + dirToString(direction));//Sets animation
		setClothsAnim("idle_" + dirToString(direction));//Sets cloths animation
	}
	
	//Striking function
	void strike(){
		if (primary){//If weapon is valid
			action = GETACODE(ACT_STRIKE, GETDIR(action));//Sets action code
			anims.setAnim(primary->strikeAnim + "_" + dirToString(GETDIR(action)));//Sets striking animation
			setClothsAnim(primary->strikeAnim + "_" + dirToString(GETDIR(action)));//Sets cloth animation
			
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
		setClothsAnim("dying");//Sets cloths animation
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
		baseHits -= amount;
		if (hits() <= 0) kill();
	}
	
	//Function to heal unit
	void heal(int amount){
		baseHits += amount;
		if (hits() <= 0) kill();
	}
	
	//Function to give primary melee weapon to unit
	void giveWeapon_primary(string id){
		weapon *w = new weapon;
		if(get(&weaponDb, id)) *w = *get(&weaponDb, id);//Sets weapon
		
		primary = w;//Sets primary weapon
	}
	
	//Function to give cloth to unit
	void wear(string id){
		cloth *c = new cloth;//New cloth
		if (get(&clothDb, id)) *c = *get(&clothDb, id);//Gets cloth
		
		cloths.push_back(c);//Adds cloth
		cout << name << " now wears " << id << endl;
	}
	
	//Function to set animation for all cloths
	void setClothsAnim(string id){
		deque<cloth*>::iterator i;//Iterator
		for (i = cloths.begin(); i != cloths.end(); i++)//For each cloth
			(*i)->overlay.setAnim(id);//Sets cloth anim
	}
	
	//Function to move to next frame for all cloths
	void nextFrameCloths(){
		deque<cloth*>::iterator i;//Iterator
		for (i = cloths.begin(); i != cloths.end(); i++)//For each cloth
			(*i)->overlay.next();//Sets cloth anim
	}
	
	//Function to make AI move (prototype)
	void AI();
	
	//Function to get unit hits applying effects
	int hits(){
		return baseHits;//Return plain hits (no permanent-temporary effects can affect hits)
	}
	
	//Functio to get max unit hits applying effects
	int maxHits(){
		int result = baseMaxHits;//Base max hits
		
		int i;//Counter
		for (i = 0; i < cloths.size(); i++) result += cloths[i]->onEquip.maxHits;//Adds effect of each cloth
		
		return result;//Returns result
	}
	
	//Function to apply an effect to the unit
	void applyEffect(effect e){
		if (e.duration == 0){//Instant/permanente effect
			//Applies effect to stats
			heal(e.hits);
			baseMaxHits += e.maxHits;
			baseStrength += e.strength;
			baseConstitution += e.constitution;
			baseIntelligence += e.intelligence;
			baseWisdom += e.wisdom;
		}
		
		else {//Temporary effect
			tempEffects.push_back(e);//Adds effect to effects
		}
	}
	
	//Function to apply temporary effects and remove the finished ones
	void tempEff(){
		list<effect>::iterator e;//Iterator
		
		for (e = tempEffects.begin(); e != tempEffects.end(); e++){//For each temporary effect
			if (e->duration == 0) e = tempEffects.erase(e);//Removes effect if finished
			
			else {//Else
				e->duration--;//Reduces duration by 1
				
				//Applies time effects (basically hits, mana)
				heal(e->hits);
			}
		}
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
	deque<unit*> units;//Controlled units
	int AI_current;//Current AI unit
	
	//Constructor
	controller(){
		AI_current = 0;
	}
	
	//Function to give order to all controlled units
	void giveOrder(order o){
		deque<unit*>::iterator u;//Unit iterator
		for (u = units.begin(); u != units.end(); u++)//For each unit
			(*u)->execOrder(o);
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
		deque<unit*>::iterator u;//Unit iterator
		for (u = units.begin(); u != units.end(); u++)//For each unit
			if (!(*u)->ready()) return false;//Returns false if unit is not ready
		
		return true;//Returns true if units were all ready
	}
	
	//Function to get if all controlled units are ready or dead
	bool readyOrDead(){
		deque<unit*>::iterator u;//Unit iterator
		for (u = units.begin(); u != units.end(); u++)//For each unit
			if (!(*u)->ready() && (*u)->action != ACT_DEAD) return false;//Returns false if unit is neither ready nor dead
		
		return true;//Returns true if units were all ready
	}
	
	//Function to add controlled unit
	void addUnit(unit* u){
		units.push_back(u);//Adds unit to controlled
	}
	
	//Function to run units' AI functions
	bool AI(){
		if (units.size() == 0) return true;//Returns true if there are no units
		
		begin:
		if (AI_current >= 0 || units[AI_current - 1]->ready() || units[AI_current - 1]->action == ACT_DEAD){//If current unit is the first or the previous is ready or dead
			if (AI_current == units.size()){//If that was last unit
				AI_current = 0;//Goes back to first unit
				return true;//Returns true
			}
			
			else {//Else
				units[AI_current]->AI();//Execs current unit's AI function
				AI_current++;//Next unit
				
				if (units[AI_current - 1]->ready() || units[AI_current - 1]->action == ACT_DEAD) goto begin;//Restarts from beginning if unit did nothing
				return false;//Returns false
			}
		}
	}
	
	//Function returning if there are units to be controlled
	bool valid(){
		deque<unit*>::iterator i;//Iterator
		for (i = units.begin(); i != units.end(); i++)//For each unit
			if ((*i)->ready()) return true;//If unit is ready returns true
			
		return false;//Returns false
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
	SDL_Surface* sightMask;//Picture of sight mask
	
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
		sightMask = NULL;
		
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
	
	//Function to make hero sight mask
	void makeSightMask(unit* u, int w, int h){
		if (sightMask) SDL_FreeSurface(sightMask);//Frees mask if existing
		
		sightMask = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, w, h, 32, 0, 0, 0, 0);//Makes surface
		filledCircleColor(sightMask, w / 2, h / 2, u->sight * tilesSide, 0xFFFFFFFF);//Draws circle
		
		SDL_SetColorKey(sightMask, SDL_SRCCOLORKEY, 0xFFFFFF);
		SDL_SetAlpha(sightMask, SDL_SRCALPHA, 64);
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
			if (u != units.end() && (d == decos.end() || compare_unitDeco(*u, *d))){//If unit is before deco
				if (c && uDist(c, *u) < c->sight) (*u)->print(target, offset.x + tilesSide * (*u)->x + tilesSide / 2, offset.y + tilesSide * (*u)->y + tilesSide / 2);//Prints unit
				u++;//Next unit
			}
			
			else if (d != decos.end()){//Else
				(*d)->print(target, offset.x + tilesSide * (*d)->x + tilesSide / 2, offset.y + tilesSide * (*d)->y + tilesSide / 2);//Prints deco
				d++;//Next deco
			}
		}
		
		if (sightMask) SDL_BlitSurface(sightMask, NULL, target, NULL);//Prints sight mask
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
			
			makePict(true);//Makes picture
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
	
	//Function to apply effect to unit in given tile
	void applyEffect(int x, int y, effect e){
		deque<unit*>::iterator i;//Iterator
		
		for(i = units.begin(); i != units.end(); i++){//For each unit
			if ((*i)->x == x && (*i)->y == y){//If unit is in given position
				(*i)->applyEffect(e);//Applies effect to unit
				break;//Exits loop
			}
		}
	}
	
	//Function to apply temporary effects to all units
	void tempEffects(){
		deque<unit*>::iterator i;//Iterator
		
		for (i = units.begin(); i != units.end(); i++){//For each unit
			(*i)->tempEff();//Applies temp effects to unit
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
	
	//Function to get unit by name
	unit* getUnit_name(string name){
		deque<unit*>::iterator i;//Unit iterator
		
		for (i = units.begin(); i != units.end(); i++)//For each unit
			if ((*i)->name == name) return *i;//Returns unit if name is matching
			
		return NULL;//Returns null if no unit was found
	}
} activeMap;

list <map> mapDb;//Maps database

//Script class
class script{
	public:
	deque<string> cmds;//Script instructions in order of execution
	
	//Function to load from string
	void fromString(string source){
		char* cString = (char*) source.c_str();//C-string version of source
		char* tok = strtok(cString, ",");//First token
		
		while (tok){//While there are tokens left
			cmds.push_back(string(tok));//Adds token to instructions
			tok = strtok(NULL, ",");//Next token
		}
	}
	
	//Execution function
	int exec(campaign* c);
};

//Event class
class event {
	public:
	script control;//Event control script (must return true to trigger event)
	script action;//Action script, triggered when control script is verified
	
	int flags;//Event flags
	
	//Constructor
	event(){
		flags = 0b00000000;
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_EVENT){//If type is matching
			var* control = o.getVar("script_control");//Control script variable
			var* action = o.getVar("script_action");//Action script variable
			
			var* triggerOnce = o.getVar("triggerOnce");//Trigger once flag
			
			if (control) this->control.fromString(control->value);//Loads control script
			if (action) this->action.fromString(action->value);//Loads action script
			
			if (triggerOnce && triggerOnce->intValue()) flags |= EVENT_ONCE;//Trigger once flag
		}
	}
};

//Sequence class
class sequence: public content{
	public:
	script begin;//Begin script, executed when starting sequence
	script end;//End script, executed when finishing sequence
	
	list <event> events;//Sequence events
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_SEQUENCE){//If type is matching
			content::fromScriptObj(o);//Loads base content data
			
			var* begin = o.getVar("script_begin");//Begin script variable
			var* end = o.getVar("script_end");//End script variable
			
			if (begin) this->begin.fromString(begin->value);//Gets begin script
			if (end) this->end.fromString(end->value);//Gets end script
			
			deque<object>::iterator i;//Object iterator
			for (i = o.o.begin(); i != o.o.end(); i++){//For each sub object
				if (i->type == OBJTYPE_EVENT){//If object is an event
					event newEv;//New event
					newEv.fromScriptObj(*i);//Loads event
					events.push_back(newEv);//Adds event to list
				}
			}
		}
	}
	
	//Function to check all sequence events
	void checkEvents(campaign* parent){
		list<event>::iterator i;//Event iterator
		
		for (i = events.begin(); i != events.end(); i++){//For each event
			if (i->control.exec(parent) == TRUE){//If control script is verified
				i->action.exec(parent);//Triggers action event
				
				if (i->flags & EVENT_ONCE) i = events.erase(i);//Erases event if has to be triggered only once
			}
		}
	}
};

//Campaign class
class campaign: public content{
	public:
	map* m;//Campaign map
	
	controller player;//Player controller
	controller ai;//AI controller
	int turn;//Current turn
	
	deque<sequence> seq;//Sequences of campaign, in order of execution
	int curSequence;//Current sequence index
	
	//Constructor
	campaign(){
		m = NULL;
		curSequence = 0;
		turn = 0;
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_CAMPAIGN){//If type is matching
			content::fromScriptObj(o);//Loads base content data
			
			var* m = o.getVar("map");//Map variable
			
			if (m) this->m = get(&mapDb, m->value);//Gets map
			
			deque<object>::iterator i;//Sub-object iterator
			for (i = o.o.begin(); i != o.o.end(); i++){//For each sub-object in given object
				if (i->type == OBJTYPE_SEQUENCE){//If object is a sequence
					sequence newSeq;//New sequence
					newSeq.fromScriptObj(*i);//Loads sequence
					seq.push_back(newSeq);//Adds sequence to campaign sequences
				}
			}
		}
	}
	
	//Printing function
	void print(SDL_Surface* target){
		if (m){//If map is valid
			if (player.units.size() > 0)//If there's at least one controlled unit
				m->print(target, target->w / 2, target->h / 2, player.units[0]);//Prints map centered on first controlled unit
			
			else m->print (target, target->w / 2, target->h / 2);//Else prints map normally
		}
	}
	
	//Next sequence function
	void nextSeq(){
		seq[curSequence].end.exec(this);//Runs current sequence end script
		
		if (curSequence < seq.size() - 1){//If there's still a sequence to run
			curSequence++;//Next sequence
			seq[curSequence].begin.exec(this);//Runs begin script
		}
	}
	
	//Next frame function
	void nextFrame(){
		if (m) m->nextFrame();//Goes to map next frame if possible
	}
	
	//Turn moves function
	void turnMoves(){
		if (turn == 0 && player.ready()){//If player turn
			player.getInput();//Gets player input
			
			if (!player.ready() && ai.valid()) turn++;//Goes to AI turn if moved
		}
		
		if (turn == 1 && player.ready() && ai.readyOrDead() && ai.AI())//When all AI units have been moved
			endTurn();//Ends turn
	}
	
	//Campaign setup function
	void setup(){
		curSequence = 0;//Sets first sequence
		if (seq.size() > 0) seq.front().begin.exec(this);//Execs first sequence beginning script
		
		if (player.units.size() > 0) m->makeSightMask(player.units[0], window->w, window->h);//Makes sight mask
	}
	
	//End turn function
	void endTurn(){
		m->tempEffects();//Applies temporary effects
		seq[curSequence].checkEvents(this);//Checks sequence events
		
		turn = 0;//Restarts turn
	}
	
	//Function to get a deque of campaign variables
	deque<var> getVars(){
		deque<var> result;//Result
		
		if (player.units.size() > 0){
			var pId ("player.id", player.units[0]->id);
			var pName ("player.name", player.units[0]->name);
			var pX ("player.x", player.units[0]->x);
			var pY ("player.y", player.units[0]->y);
			
			result.push_back(pId);
			result.push_back(pName);
			result.push_back(pX);
			result.push_back(pY);
		}
		
		return result;//Returns result
	}
};

list<campaign> campaignDb;//Campaign database

//Execution function
int script::exec(campaign* c){
	if (!c || !c->m) return false;//Exits function if no target was given
	
	int i;//Counter
	int result = VOID;//Script result (false on errors)
	
	deque<var> vars = c->getVars();//Generates variables
		
	for (i = 0; i < cmds.size(); i++){//For each instruction		
		deque<string> tokens = tokenize(cmds[i], "\t ");
		
		if (tokens[0] == "createUnit" && tokens.size() >= 6){//Create unit instruction
			string uId = tokens[1];//Unit id
			string uName = tokens[2];//Unit name
			int uX = atoi(tokens[3].c_str());//Unit x
			int uY = atoi(tokens[4].c_str());//Unit y
			int uSide = atoi(tokens[5].c_str());//Unit side
			
			unit* u = c->m->createUnit(uId, uName, uX, uY, uSide);//Creates unit in map
			if (u && tokens.size() >= 7) u->giveWeapon_primary(tokens[6]);//Gives weapon if defined
			if (u && tokens.size() >= 8) u->turn(atoi(tokens[7].c_str()));//Picks direction if defined
		}
		
		else if (tokens[0] == "giveWeapon_primary" && tokens.size() >= 3){//Give primary weapon instruction
			unit* u = c->m->getUnit_name(tokens[1]);//Gets unit
			if (u) u->giveWeapon_primary(tokens[2]);//Gives weapon to unit
		}
		
		else if (tokens[0] == "wear" && tokens.size() >= 3){//Gives clothing to unit
			unit* u = c->m->getUnit_name(tokens[1]);//Gets unit
			if (u) u->wear(tokens[2]);//Gives cloth to unit
		}
		
		else if (tokens[0] == "giveControl" && tokens.size() >= 2){//Give control to player instruction
			unit* u = c->m->getUnit_name(tokens[1]);//Gets unit
			if (u) c->player.addUnit(u);//Gives unit to player
		}
		
		else if (tokens[0] == "createPlayer" && tokens.size() >= 6){//Create unit controlled by player instruction
			string uId = tokens[1];//Unit id
			string uName = tokens[2];//Unit name
			int uX = atoi(tokens[3].c_str());//Unit x
			int uY = atoi(tokens[4].c_str());//Unit y
			int uSide = atoi(tokens[5].c_str());//Unit side
			
			unit* u = c->m->createUnit(uId, uName, uX, uY, uSide);//Creates unit in map
			if (u){//If created successfully
				if (tokens.size() >= 7) u->giveWeapon_primary(tokens[6]);//Gives weapon if defined
				if (tokens.size() >= 8) u->turn(atoi(tokens[7].c_str()));//Picks direction if defined
				c->player.addUnit(u);//Gives control to player
			}
		}
		
		else if (tokens[0] == "createAI" && tokens.size() >= 6){//Create unit controlled by AI
			string uId = tokens[1];//Unit id
			string uName = tokens[2];//Unit name
			int uX = atoi(tokens[3].c_str());//Unit x
			int uY = atoi(tokens[4].c_str());//Unit y
			int uSide = atoi(tokens[5].c_str());//Unit side
			
			unit* u = c->m->createUnit(uId, uName, uX, uY, uSide);//Creates unit in map
			if (u){//If created successfully
				if (tokens.size() >= 7) u->giveWeapon_primary(tokens[6]);//Gives weapon if defined
				if (tokens.size() >= 8) u->turn(atoi(tokens[7].c_str()));//Picks direction if defined
				c->ai.addUnit(u);//Gives control to player
			}
		}
		
		else if (tokens[0] == "if" && tokens.size() >= 2){//If statement
			string s = "";//Expression
			int j;//Counter
			for (j = 1; j < tokens.size(); j++) s += tokens[j] + " ";//Adds each token to string
			
			string result = expr(s, &ops_bool, &vars);//Calculates expression
			if (!atoi(result.c_str())) i++;//Skips next instruction if statement is false
		}
		
		else if (tokens[0] == "return" && tokens.size() >= 2){//Return statement
			if (tokens[1] == "true") return TRUE;
			else if (tokens[1] == "false") return FALSE;
			else return VOID;
		}
		
		else if (tokens[0] == "endSequence"){//End sequence instruction
			c->nextSeq();//Moves to next sequence
		}
	}
	
	return result;//Returns result
}

//Walking function
void unit::walk(int direction){
	if (parent){//If there's a reference map
		//Checks free tiles
		switch (direction){//According to direction
			case NORTH:
			if (!parent->isFree(x, y - 1)){
				turn(NORTH);
				return;
			}
			break;
			
			case WEST:
			if (!parent->isFree(x - 1, y)){
				turn(WEST);
				return;
			}
			break;
			
			case SOUTH:
			if (!parent->isFree(x, y + 1)){
				turn(SOUTH);
				return;
			}
			break;
			
			case EAST:
			if (!parent->isFree(x + 1, y)){
				turn(EAST);
				return;
			}
			break;
		}
	}

	action = GETACODE(ACT_WALK, direction);//Sets action codes
	anims.setAnim("walk_" + dirToString(direction));//Sets walking animation
	setClothsAnim("walk_" + dirToString(direction));//Sets walking animation for cloths
}

//Function to go to next frame
void unit::nextFrame(){
	anims.next();//Next animation frame
	nextFrameCloths();//Next cloths frame
	
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
	
	else if (GETACT(action) == ACT_STRIKE && primary){//If unit is striking
		if (primary->image) primary->overlay.next();//Next frame for weapon animation
		
		if (anims.current()->curFrame == anims.current()->duration() - 1) stop();//Stops if reached end of animation
		
		if (anims.current()->curFrame == primary->strikeFrame && parent){//At half striking animation
			switch (GETDIR(action)){//According to direction
				case NORTH: parent->applyEffect(x, y - 1, primary->onTarget); break;
				case WEST: parent->applyEffect(x - 1, y, primary->onTarget); break;
				case SOUTH: parent->applyEffect(x, y + 1, primary->onTarget); break;
				case EAST: parent->applyEffect(x + 1, y, primary->onTarget); break;
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

//Function to make unit AI move
void unit::AI(){
	if (action == ACT_DEAD) return;//Exits if unit is dead
	
	if (parent){//If there's a parent scenario
		//Picks closest unit
		unit* closest = NULL;//Pointer to closest emey unit
		deque<unit*>::iterator u;//Unit iterator
		
		for (u = parent->units.begin(); u != parent->units.end(); u++)//For each unit in parent scenario
			if (*u != this && (*u)->side != side && (!closest || uDist(this, *u) < uDist(this, closest))) closest = *u;//Sets closest unit

		if (!closest || uDist(this, closest) > sight) return;//Exits if didn't found any appropriate unit or unit was too far to be seen
		
		if (uDist(closest, this) == 1){//If closest unit is in adjacent tile
			//Faces unit
			if (closest->y < y) turn(NORTH);
			else if (closest->x < x) turn(WEST);
			else if (closest->y > y) turn(SOUTH);
			else if (closest->x > x) turn(EAST);
			
			strike();//Strikes
		}
		
		//Gives walking orders
		if (closest->y < y && parent->isFree(x, y - 1)) walk(NORTH);
		else if (closest->x < x && parent->isFree(x - 1, y)) walk(WEST);
		else if (closest->y > y && parent->isFree(x, y + 1)) walk(SOUTH);
		else if (closest->x > x && parent->isFree(x + 1, y)) walk(EAST);
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
		
		if (i->type == OBJTYPE_CLOTH){//If object is a cloth
			cloth newCloth;//New cloth
			newCloth.fromScriptObj(*i);//Loads cloth
			clothDb.push_back(newCloth);//Adds cloth to database
		}
		
		if (i->type == OBJTYPE_DECO){//If object is a decoration
			deco newDeco;//New decoration
			newDeco.fromScriptObj(*i);//Loads deco
			decoDb.push_back(newDeco);//Adds deco to database
		}
		
		if (i->type == OBJTYPE_CAMPAIGN){//If object is a campaign
			campaign newCampaign;//New campaign
			newCampaign.fromScriptObj(*i);//Loads campaign
			campaignDb.push_back(newCampaign);//Adds campaign
		}
	}
	
	cout << "Loaded successfully!" << endl;
	cout << terrainDb.size() << " terrains\n" << mapDb.size() << " maps\n" << unitDb.size() << " units\n" << weaponDb.size() << " weapons\n" << clothDb.size() << " cloths\n" << decoDb.size() << " decos\n" << campaignDb.size() << " campaigns\n";
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
	if (settings.getVar("font_labels")) labelsFont = TTF_OpenFont(settings.getVar("font_labels")->value.c_str(), 12);//Opens global font
	
	//Sets operators
	op bool_equal ("=", 1, ops_bool_equal);
	op bool_greater (">", 1, ops_bool_greater);
	op bool_less ("<", 1, ops_bool_less);
	op bool_and ("&", 0, ops_bool_and);
	op bool_or ("|", 0, ops_bool_or);
	
	ops_bool.push_back(bool_equal);
	ops_bool.push_back(bool_greater);
	ops_bool.push_back(bool_less);
	ops_bool.push_back(bool_and);
	ops_bool.push_back(bool_or);
}

#endif