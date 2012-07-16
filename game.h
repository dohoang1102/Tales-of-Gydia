//Game handling header

#ifndef _GAME
#define _GAME

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
#define OBJTYPE_DAMTYPE		"damageType"//Object type damage type
#define OBJTYPE_EFFECT		"effect"//Object type effect
#define OBJTYPE_PROJECTILE	"projectile"//Object type projectile
#define OBJTYPE_ITEM		"item"//Object type item
#define OBJTYPE_WEAPON		"weapon"//Object type weapon
#define OBJTYPE_SPELL		"spell"//Object type spell
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
#define PROJSTEP			6//Pixels of motion for each frame of projectiles flying

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

//Clothing types
#define CLOTH_HEAD			0//Head
#define CLOTH_BODY			1//Body
#define CLOTH_LEGS			2//Pants

//Action codes
//Second digit is effective code, first digit is for direction
#define ACT_IDLE			0x00//Idle
#define ACT_WALK			0x10//Walking
#define ACT_STRIKE			0x20//Striking
#define ACT_DYING			0x30//Dying
#define ACT_DEAD			0x40//Dead
#define ACT_REST			0x50//Resting
#define ACT_TURN			0x60//Turning
#define ACT_SPELL			0x70//Casting spell

#define GETACODE(ACT,DIR)	((ACT) | (DIR))//Macro to get action and direction code out of separate action and direction
#define GETACT(CODE) 		((CODE) & 0xF0)//Macro to get action code out of code with action and direction
#define GETDIR(CODE)		((CODE) % 0x10)//Macro to get direction code out of code with action and direction

//Damage super types
#define DAM_PHYSICAL		0//Phisical damage
#define DAM_MAGICAL			1//Magical damage

//Item types
#define DISPOSABLE			0//Disposable item
#define WEAPON				1//Weapon
#define CLOTHING			2//Clothing
#define SPELL				3//Spell

//Game views
#define GAME				0//Game view
#define PLAYER				1//Player info view
#define INVENTORY			2//Inventory view
#define LEVELUP_1			3//Level up view
#define LEVELUP_2			4//Level up view
#define QUEST				5//Quest info view
#define EXCHANGE			6//Exchange view

//Game calculations: damage, mana costs, ...
#define MINDAMAGE							-1//Minimum damage that can be taken (if calculates lower damage, replaced with this value)
#define CALC_DAMAGEDEALT(BASE, STAT)		((BASE) * (1 + STAT / 100))//Dealt damage calculation: [base weapon damage] * [strength/intelligence] / 10 - this means: strength = 1 => 10% damage bonus on weapons
#define CALC_DAMAGETAKEN(BASE, STAT, MULT)	((BASE) * ((MULT) - (STAT) / 100) >= MINDAMAGE ? MINDAMAGE : ceil((BASE) * ((MULT) - (STAT) / 100)))//Damage taken calculation: [base damage] * [resistance multiplier reduced of stat/100]
#define CALC_RECOVERY(STAT1, STAT2)			((STAT1) * (STAT2) > 100 ? (STAT1) * (STAT2) / 100 : 1)//Recovery calculation
#define CALC_REQUIREDXP(LEVEL)				((LEVEL) * (LEVEL) * 8)//Required xp calculation

class unit;//Unit class prototype
class map;//Map class prototype
class campaign;//Campaign class prototype

typedef struct {int code; string args;} order;//Order structure

int tilesSide = 32;//Side of each tile of the map

deque<op> ops_bool;//Boolean script operators
deque<op> ops_int;//Intger script operators
deque<op> ops_str;//String script operators

//Game status
bool running = true;

//Game phases
#define MAIN_MENU			0
#define GAME_PHASE			1

int gamePhase = MAIN_MENU;//Game phase

//Settings
//Localization
list <loc> locales;//Installed locales
string curLoc = "";//Current locale
string getText(string);//Get text function prototype

//User interface
//Fonts
TTF_Font* globalFont = NULL;//Global generic font
TTF_Font* panelFont_major = NULL;//Panels major font
TTF_Font* panelFont_minor = NULL;//Panels minor font
TTF_Font* dialogFont = NULL;//Font for dialogs

//Global graphics: buttons
SDL_Surface* button_released = NULL;//Released button image
SDL_Surface* button_pressed = NULL;//Pressed button image
SDL_Surface* button_big_released = NULL;//Released button image
SDL_Surface* button_big_pressed = NULL;//Pressed button image
SDL_Surface* button_quit_released = NULL;//Released quit button image
SDL_Surface* button_quit_pressed = NULL;//Pressed quit button image

//User interface data
//Main menu
SDL_Surface* title_image = NULL;//Title image

button btn_newGame;//New game button
button btn_loadGame;//Load game button
button btn_quitMenu;//Quit button

//Game HUD
SDL_Surface* bar_frame = NULL;//Frame for hit and mana bars
SDL_Rect bar_frame_offset = {0, 0};//Frame offset

SDL_Surface* bar_hits = NULL;//Hitbar fill
SDL_Rect bar_hits_offset = {0, 0};//Hitbar offset

SDL_Surface* bar_mana = NULL;//Manabar fill
SDL_Rect bar_mana_offset = {0, 0};//Manabar offset

SDL_Surface* bar_xp_frame = NULL;//Frame for xp bar
SDL_Rect bar_xp_frame_offset = {0, 0};//Xp bar offset

SDL_Surface* bar_xp = NULL;//Xp bar fill
SDL_Rect bar_xp_offset = {0, 0};//Xp fill offset

//Dialogs
SDL_Surface* dialog_pict = NULL;//Dialog box picture
SDL_Surface* dialog_actor_pict = NULL;//Dialog actor box picture
int dialog_actor_x, dialog_actor_y, dialog_actor_nameY;//Coords of image and name in actor picture
textBox dialogBox;//Dialog box
imageBox dialogBox_actor;//Actor dialog box
button btn_answers[4];//Answer buttons

//Windows: global
button btn_quit;//Quit button

//Windows: inventory
SDL_Surface* bigPanel_pict = NULL;//Big panel picture
panel inventoryPanel;//Inventory panel
imageBox inv_slots [12];//Inventory slots boxes
imageBox headBox, bodyBox, legsBox, weaponBox, spellBox;//Equipped stuff boxes
textBox itemInfoBox;//Info about selected item

SDL_Surface* slot_pict = NULL;//Slot box picture
SDL_Surface* slot_pict_focus = NULL;//Focused slot box picture

int slot_selected = -1;

button btn_use, btn_drop;//Use and drop inventory buttons

//Windows: player info
panel infoPanel;//Player info panel
imageBox nameBox;//Player name box
textBox hpBox, mpBox;//Player hp and mp box
textBox xpBox;//Player xp box
textBox strBox, intBox, conBox, wisBox;//Statistics boxes
textBox effBox;//Effects box
list<textBox*> resBox;//Resistance boxes

//Windows: level up
SDL_Surface* midPanel_pict = NULL;//Middle-size panel picture

//Level up 1: HP/MP
textBox levelUp_caption;//Level up caption

panel hp_mp_levelUp;//First level up panel
button hpUp, mpUp;//Hits and mana buttons

//Level up 2: stats
panel stats_levelUp;//Stat level up panel
button strUp, conUp, intUp, wisUp;//Stats buttons

//Windows: quest info
panel questPanel;//Quest info panel
textBox questNameBox;//Quest name text box
textBox questInfoBox;//Quest info text box

//Windows: exchange
panel exchangePanel;//Exchange panel
imageBox exch_slots [12];//Exchange slot boxes
button btn_sell;//Sell button

//Colors
Uint32 infoPanel_col1 = 0x000000;//Color for main stuff in info panel
Uint32 infoPanel_col2 = 0x000000;//Secondary color in info panel
Uint32 infoPanel_col3 = 0x000000;//Color for bonuses in info panel
Uint32 infoPanel_col4 = 0x000000;//Color for maluses in info panel

Uint32 buttons_col = 0x000000;//Color for buttons
Uint32 buttons_big_col = 0x000000;//Color for big buttons

Uint32 dialog_col = 0xFFFFFF;//Color for dialog boxes

//Boolean operators functions
string ops_bool_equal(string a, string b){ return toString(a == b); }
string ops_bool_greater(string a, string b){ return toString(atoi(a.c_str()) > atoi(b.c_str())); }
string ops_bool_less(string a, string b){ return toString(atoi(a.c_str()) < atoi(b.c_str())); }
string ops_bool_and(string a, string b){ return toString(atoi(a.c_str()) && atoi(b.c_str())); }
string ops_bool_or(string a, string b){ return toString(atoi(a.c_str()) || atoi(b.c_str())); }

//Integer operators functions
string ops_int_sum(string a, string b){ return toString(atoi(a.c_str()) + atoi(b.c_str())); }
string ops_int_subt(string a, string b){ return toString(atoi(a.c_str()) - atoi(b.c_str())); }
string ops_int_mult(string a, string b){ return toString(atoi(a.c_str()) * atoi(b.c_str())); }
string ops_int_div(string a, string b){ return toString(atoi(a.c_str()) / atoi(b.c_str())); }

//String operators functions
string ops_str_concat(string a, string b){ return a + b; }

//Mid panel printing function
void midPanelPrint(SDL_Surface* target, control* p){
	SDL_Rect offset = {p->x, p->y};//Offset rectangle
	SDL_BlitSurface(midPanel_pict, NULL, target, &offset);//Prints panel picture
	
	panel* e = (panel*) p;//Converts to panel
	list<control*>::iterator i;//Iterator for controls
	for (i = e->controls.begin(); i != e->controls.end(); i++){//For each control
		//Applies offset
		(*i)->x += e->x;
		(*i)->y += e->y;
		
		(*i)->print(target);//Prints the control
		
		//Removes offset
		(*i)->x -= e->x;
		(*i)->y -= e->y;
	}
}

//Big panel printing function
void bigPanelPrint(SDL_Surface* target, control* p){
	SDL_Rect offset = {p->x, p->y};//Offset rectangle
	SDL_BlitSurface(bigPanel_pict, NULL, target, &offset);//Prints panel picture
	
	panel* e = (panel*) p;//Converts to panel
	list<control*>::iterator i;//Iterator for controls
	for (i = e->controls.begin(); i != e->controls.end(); i++){//For each control
		//Applies offset
		(*i)->x += e->x;
		(*i)->y += e->y;
		
		(*i)->print(target);//Prints the control
		
		//Removes offset
		(*i)->x -= e->x;
		(*i)->y -= e->y;
	}
}

//Dialog printing function
void dialogPrint(SDL_Surface* target, control* p){
	textBox* e = (textBox*) p;//Converts to text boex
	
	SDL_Rect offset = {e->x, e->y};//Offet rectangle
	SDL_BlitSurface(dialog_pict, NULL, target, &offset);//Prints dialog picture
	
	if (dialogBox.text != "" && dialogBox.font){//If there's a text to print
		string curLine = "";//Current line
		int w = 0;//Text width
		int i = 0;//Counter
		int curY = e->y + 25;//Current y coord
		deque<string> tokens = tokenize(e->text + (e->isFocused() && e->edit ? "|" : ""), " \t");//Text tokens
		
		
		while (i < tokens.size()){//While the line is not long enough
			TTF_SizeText(e->font, (curLine + tokens[i] + " ").c_str(), &w, NULL);//Calcs width
			
			if (w <= e->w - 10 && tokens[i] != "\\n") curLine += tokens[i] + " ";//Adds token if there's still space and it is not a newline
			else {//Else
				SDL_Surface* text = RENDERTEXT(e->font, curLine.c_str(), e->getForeColor());//Renders line
				SDL_Rect offset;//Offset rectangle
		
				if (e->textAlign == LEFT) offset = {e->x + 5, curY};
				else if (e->textAlign == RIGHT) offset = {e->x + e->w - text->w, curY};
				else if (e->textAlign == CENTER) offset = {e->x + (e->w - text->w) / 2, curY};
				
				SDL_BlitSurface(text, NULL, target, &offset);//Prints text
				SDL_FreeSurface(text);//Frees text
				
				curY += TTF_FontHeight(e->font) + 1;//Caret down
				curLine = (tokens[i] == "\\n" ? "" : tokens[i] + " ");//Resets line
			}
			
			i++;
		}
		
		//Prints last line
		SDL_Surface* text = RENDERTEXT(e->font, curLine.c_str(), e->getForeColor());//Renders line
		SDL_Rect offset;//Offset rectangle

		if (e->textAlign == LEFT) offset = {e->x + 5, curY};
		else if (e->textAlign == RIGHT) offset = {e->x + e->w - text->w, curY};
		else if (e->textAlign == CENTER) offset = {e->x + (e->w - text->w) / 2, curY};
		
		SDL_BlitSurface(text, NULL, target, &offset);//Prints text
		SDL_FreeSurface(text);//Frees text
	}
}

//Dialog portrait printing function
void dialogPortraitPrint(SDL_Surface* target, control* p){
	imageBox* e = (imageBox*) p;//Converts to image box
	
	SDL_Rect offset = {e->x, e->y};//Offet rectangle
	SDL_BlitSurface(dialog_actor_pict, NULL, target, &offset);//Prints rectangle

	if (e->i) e->i->print(target, e->x + dialog_actor_x, e->y + dialog_actor_y);//Prints slot image
	
	if (e->id != "" && panelFont_minor){//If there's an available
		SDL_Surface* text = TTF_RenderText_Blended(panelFont_minor, e->id.c_str(), dialogBox.getForeColor());//Renders text
		SDL_Rect offset {e->x + (e->w - text->w) / 2, e->y + dialog_actor_nameY - text->h / 2};//Offset rectangle
		SDL_BlitSurface(text, NULL, target, &offset);//Prints text
		SDL_FreeSurface(text);//Frees text
	}
}

//Slot printing function
void slotPrint(SDL_Surface* target, control* p){
	imageBox* e = (imageBox*) p;//Converts to image box
	
	SDL_Rect offset = {e->x, e->y};//Offet rectangle
	if (!e->isFocused()) SDL_BlitSurface(slot_pict, NULL, target, &offset);//Prints slot picture
	else SDL_BlitSurface(slot_pict_focus, NULL, target, &offset);//Prints focused slot picture

	if (e->i) e->i->print(target, e->x + e->w / 2, e->y + e->h / 2);//Prints slot image
}

//Quit button printing function
void quitPrint(SDL_Surface* target, control* p){
	SDL_Rect offset = {p->x, p->y};//Offset rectangle
	
	if (p->getStatus() == VISIBLE) SDL_BlitSurface(button_quit_released, NULL, target, &offset);//Prints normal picture
	if (p->getStatus() == PRESSED) SDL_BlitSurface(button_quit_pressed, NULL, target, &offset);//Prints pressed picture
}

//Function to handle slot selection
void slot_getFocus(control* p){
	slot_selected = atoi(p->id.c_str());
}

//Function to handle slot losing selection
void slot_loseFocus(control* p){
	control* sel = inventoryPanel.getFocused();

	if (!sel || sel->type != IMAGEBOX) slot_selected = -1;
}

//Button printing function
void buttonPrint(SDL_Surface* target, control* p){
	button* e = (button*) p;//Converts to button
	
	SDL_Rect offset = {e->x, e->y};//Offset rectangle
	
	if (e->getStatus() == VISIBLE)//Normal status
		SDL_BlitSurface(button_released, NULL, target, &offset);//Prints released button
		
	else if (e->getStatus() == PRESSED)//Button pressed
		SDL_BlitSurface(button_pressed, NULL, target, &offset);//Prints pressed button
	
	//Prints text
	if (e->text != "" && e->font){//If there are valid text and font
		SDL_Surface* txt = TTF_RenderText_Blended(e->font, e->text.c_str(), e->getForeColor());//Renders text
		SDL_Rect o {e->x + e->w / 2 - txt->w / 2, e->y + 5};//Offset rectangle
		SDL_BlitSurface(txt, NULL, target, &o);//Blits text
		SDL_FreeSurface(txt);//Frees text
	}
}

//Big button printing function
void bigButtonPrint(SDL_Surface* target, control* p){
	button* e = (button*) p;//Converts to button
	
	SDL_Rect offset = {e->x, e->y};//Offset rectangle
	
	if (e->getStatus() == VISIBLE)//Normal status
		SDL_BlitSurface(button_big_released, NULL, target, &offset);//Prints released button
		
	else if (e->getStatus() == PRESSED)//Button pressed
		SDL_BlitSurface(button_big_pressed, NULL, target, &offset);//Prints pressed button
	
	//Prints text
	if (e->text != "" && e->font){//If there are valid text and font
		SDL_Surface* txt = TTF_RenderText_Blended(e->font, e->text.c_str(), e->getForeColor());//Renders text
		SDL_Rect o {e->x + e->w / 2 - txt->w / 2, e->y + 5};//Offset rectangle
		SDL_BlitSurface(txt, NULL, target, &o);//Blits text
		SDL_FreeSurface(txt);//Frees text
	}
}

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
	string shownName;
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
			shownName = getText(id);
			
			var* author = o.getVar("author");
			var* description = o.getVar("description");
			
			if (author) this->author = author->value;
			if (description) this->description = getText(description->value);
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

//Get text function
string getText(string id){
	loc *main = get(&locales, curLoc);//Current locale
	
	if (main) return main->getText(id);//Returns text if found
	else return id;//Else returns required id
}

//Terrain class
class terrain: public content{
	public:
	list<image> transitions;
	image baseImage;//Base image of the terrain
	int layer;//Layer of the terrain (transitions are printed only above lower layers)
	bool free;//Flag indicating if player can walk on terrain
	bool free_air;//Flag indicating if things can fly over terrain
	Uint32 mmapColor;//Color in minimap
		
	//Constructor
	terrain(){
		id = "";
		author = "";
		description = "";
		layer = 0;
		free = true;
		free_air = true;
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
				var* free_air = o.getVar("free_air");//Air free variable
				var* mmapColor = o.getVar("mmapColor");//Minimap color variable
				
				if (free) this->free = free->intValue();//Gets free flag
				if (free_air) this->free_air = free_air->intValue();//Gets air free flag
				if (mmapColor) this->mmapColor = strtol(mmapColor->value.c_str(), NULL, 0);//Gets minimap color
			}
			
			#ifdef _ERROR
				else if (errFunc) errFunc(SCRIPT_WARNING_INVALIDOBJTYPE, "[map.h] No terrain could be loaded from " + o.type + " object");
			#endif
		}
	#endif
};

list <terrain> terrainDb;//Terrains database

//Damage type class
class damType: public content {
	public:
	int superType;//Damage super-type (physical/magic)
	
	double mult;//Damage multiplier (used in resistances)
	
	//Constructor
	damType(){
		id = "";
		superType = DAM_PHYSICAL;
		
		mult = 1;
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_DAMTYPE){//If type is matching
			content::fromScriptObj(o);//Loads base content data (only id and description are used)
			
			var* superType = o.getVar("type");//Super type variable
			var* mult = o.getVar("multiplier");//Multiplier variable
			
			//Sets super-type
			if (superType){
				if (superType->value == "physical") this->superType = DAM_PHYSICAL;
				else if (superType->value == "magical") this->superType = DAM_MAGICAL;
			}
			
			if (mult) this->mult = mult->doubleValue();//Gets multiplier
		}
	}
};

list <damType> damTypeDb;//Types database

//General item class
class item: public content {
	public:
	animSet overlay;//Item animations
	bool imgGiven;//Flag indicating if image was given
	image icon;//Icon image
	
	int itemType;//Item type
	
	//Constructor
	item(){
		imgGiven = false;
		itemType = DISPOSABLE;
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		content::fromScriptObj(o);//Base content data
		
		object* overlay = o.getObj("overlay");//Weapon overlay object
		object* icon = o.getObj("icon");//Icon object
		
		if (overlay){ this->overlay.fromScriptObj(*overlay); imgGiven = true;}//Loads overlay
		if (icon) this->icon.fromScriptObj(*icon);//Gets icon
	}
	
	//Printing function
	void print(SDL_Surface* target, int x, int y){
		if (imgGiven) overlay.print(target, x, y);//Prints if image was given
	}
};

//Effect class
class effect: public content {
	public:
	int hits;//Hits variation
	int maxHits;//Max hits variation
	int mana;//Mana variation
	int maxMana;//Max mana variation
	
	int strength;//Strength bonus
	int intelligence;//Intelligence bonus
	int constitution;//Constitution bonus
	int wisdom;//Wisdom bonus
	
	string shot;//Id of projectile shot by effect
	
	int duration;//Effect duration (0 instant/permanent, else duration in turns)
	
	damType type;//Effect damage type (needed only if effect damages someone)
	
	list<damType> resistances;//Per-type resistances bonuses
	
	bool img;//Flag indicating if image was given
	image icon;//Icon shown when effect is active
	unit* owner;//Effect owner
	
	//Constructor
	effect(){
		hits = 0;
		maxHits = 0;
		mana = 0;
		maxMana = 0;
		strength = 0;
		intelligence = 0;
		constitution = 0;
		wisdom = 0;
		shot = "";
		duration = 0;
		img = false;
		owner = NULL;
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_EFFECT){//If type is matching
			content::fromScriptObj(o);//Loads base content data (only description used)
			
			var* hits = o.getVar("hits");//Hits variable
			var* maxHits = o.getVar("maxHits");//Max hits variable
			var* mana = o.getVar("mana");//Mana variable
			var* maxMana = o.getVar("maxMana");//Max mana variable
			var* strength = o.getVar("strength");//Strength variable
			var* intelligence = o.getVar("intelligence");//Intelligence variable
			var* constitution = o.getVar("constitution");//Constitution variable
			var* wisdom = o.getVar("wisdom");//Wisdom variable
			var* shot = o.getVar("shot");//Shot variable
			var* duration = o.getVar("duration");//Duration variable
			var* type = o.getVar("type");//Type variable
			object* icon = o.getObj("icon");//Icon variable
				
			//Gets values
			if (hits) this->hits = hits->intValue();
			if (maxHits) this->maxHits = maxHits->intValue();
			if (mana) this->mana = mana->intValue();
			if (maxMana) this->maxMana = maxMana->intValue();
			if (strength) this->strength = strength->intValue();
			if (intelligence) this->intelligence = intelligence->intValue();
			if (constitution) this->constitution = constitution->intValue();
			if (wisdom) this->wisdom = wisdom->intValue();
			if (shot) this->shot = shot->value;
			if (duration) this->duration = duration->intValue();
			if (type && get(&damTypeDb, type->value)) this->type = *get(&damTypeDb, type->value);
			if (icon) {this->icon.fromScriptObj(*icon); img = true;}
			
			//Loads resistances
			deque<object>::iterator i;//Object iterator
			for (i = o.o.begin(); i != o.o.end(); i++){//For each sub-object
				if (i->type == OBJTYPE_DAMTYPE && get(&damTypeDb, i->name)){//If object is a damage type and that damage type exists
					damType newDT;//New damage type
					newDT.fromScriptObj(*i);//Loads damage type
					resistances.push_back(newDT);//Adds to resistances
				}
			}
		}
	}
};

//Class disposable object
class disposable: public item {
	public:
	effect use;//Effect when using
	
	//Constructor
	disposable(){
		imgGiven = false;
		itemType = DISPOSABLE;
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_ITEM){//if type is matching
			item::fromScriptObj(o);//Loads base data
			
			object* use = o.getObj("use");//Use effect
			
			if (use) this->use.fromScriptObj(*use);//Gets use effect
		}
	}
};

list<disposable> itemDb;//Items database

//Cloth class
class cloth: public item{
	public:	
	effect onEquip;//On-equip effect
	
	int type;//Cloth type (head, body, pants)
	
	//Constructor
	cloth(){
		type = CLOTH_HEAD;
		imgGiven = false;
		itemType = CLOTHING;
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_CLOTH){//If type is matching
			item::fromScriptObj(o);//Loads base content data
			
			var* type = o.getVar("type");//Gets type variable
			
			object* onEquip = o.getObj("onEquip");//On-equip effect
			
			if (onEquip) this->onEquip.fromScriptObj(*onEquip);//Gets on-equip effect
			if (type){//If there's a type variable
				if (type->value == "head") this->type = CLOTH_HEAD;
				else if (type->value == "body") this->type = CLOTH_BODY;
				else if (type->value == "legs") this->type = CLOTH_LEGS;
			}
			
			this->overlay.setAnim("idle_s");
		}
	}
};

list <cloth> clothDb;//Cloths database

//Projectile class
class projectile: public content{
	public:
	animSet anims;//Projectile animations
	effect onTarget;//Effect on target unit
	int range;//Projectile range (expires after getting out)
	
	int x, y;//Projectile position
	int dX, dY;//Position variation (for moving animation)
	int direction;//Projectile direction
	
	bool flying;//Flag indicating if projectile is flying
	
	int dist;//Distance made by projectile
	
	map *parent;//Projectile parent map
	unit *owner;//Projectile owner
	
	//Constructor
	projectile(){
		range = 1;
		x = 0;
		y = 0;
		dX = 0;
		dY = 0;
		direction = NORTH;
		flying = true;
		dist = 0;
		parent = NULL;
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_PROJECTILE){//If type is matching
			content::fromScriptObj(o);//Loads base content data
			
			object* anims = o.getObj("anims");//Animations object
			object* onTarget = o.getObj("onTarget");//Effect object
			var* range = o.getVar("range");//Range variable
			var* flying = o.getVar("flying");//Flying variable
			
			if (anims) this->anims.fromScriptObj(*anims);//Loads anims
			if (onTarget) this->onTarget.fromScriptObj(*onTarget);//Loads effect on target
			if (range) this->range = range->intValue();//Gets range
			if (flying) this->flying = flying->intValue();//Gets flying flag
		}
	}
	
	//Projectile printing function
	void print(SDL_Surface* target, int x, int y){
		anims.print(target, x + dX, y + dY);//Prints current animation
	}
	
	//Next frame function
	void nextFrame();
};

//Function to compare projectiles by position
bool sortProj_compare(projectile* a, projectile* b){
	return a->y < b->y || (a->y == b->y && a->x < b->x);
}

list<projectile> projectileDb;//Projectile database

//Weapon class
class weapon: public item{
	public:	
	//Graphics
	string strikeAnim;//Player strike animation
	int strikeFrame;//Frame of animation when unit strikes
	
	//Effects and stats
	effect onEquip;//On-equip effect
	effect onStrike;//On-strike effect
	effect onTarget;//On-target effect
	
	//Constructor
	weapon(){
		id = "";
		author = "";
		description = "";
		
		itemType = WEAPON;
	
		strikeAnim = "";
		strikeFrame = 0;
		imgGiven = false;
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_WEAPON || o.type == OBJTYPE_SPELL){//If types are matching
			item::fromScriptObj(o);//Loads base content data
			
			if (o.type == OBJTYPE_WEAPON) itemType = WEAPON;
			else itemType = SPELL;
			
			var* strikeAnim = o.getVar("strikeAnim");//Strike animation variable
			var* strikeFrame = o.getVar("strikeFrame");//Strike frame variable
			object* onEquip = o.getObj("onEquip");//On equip effect object
			object* onStrike = o.getObj("onStrike");//On strike effect object
			object* onTarget = o.getObj("onTarget");//On target effct object

			if (strikeAnim) this->strikeAnim = strikeAnim->value;//Gets strike animation
			if (strikeFrame) this->strikeFrame = strikeFrame->intValue();//Gets strike frame
			if (onEquip) this->onEquip.fromScriptObj(*onEquip);//Loads on equip effect
			if (onStrike) this->onStrike.fromScriptObj(*onStrike);//Loads on strike effect
			if (onTarget) this->onTarget.fromScriptObj(*onTarget);//Loads on target effect
		}
	}
	
	//Function to get weapon effect on target with bonuses related to owner stats (mainly damage bonuses)
	effect getEff(unit* u);
};

list <weapon> weaponDb;//Weapons database

//Function to get item from one of databases above
item* getItem(string id){
	if (get(&itemDb, id)) return (item*) get(&itemDb, id);
	else if (get(&clothDb, id)) return (item*) get(&clothDb, id);
	else if (get(&weaponDb, id)) return (item*) get(&weaponDb, id);
	else return NULL;
}

//Decoration class
class deco: public content{
	public:
	image i;//Decoration image
	bool free;//Free flag: can walk on deco
	bool free_air;//Air free flag
	int x, y;//Coords of deco
	
	item *inside [12];//Items inside decoration
	
	//Constructor
	deco(){
		free = true;
		free_air = true;
		x = 0;
		y = 0;
		
		int i;
		for (i = 0; i < 12; i++) inside[i] = NULL;
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_DECO){//If type matches
			content::fromScriptObj(o);//Loads content base data
			
			var* free = o.getVar("free");//Free flag
			var* free_air = o.getVar("free_air");//Air free flag
			object* i = o.getObj("image");//Image flag
			var* inside = o.getVar("content");//Content variable
			
			if (free) this->free = free->intValue();//Gets free flag
			if (free_air) this->free_air = free_air->intValue();//Gets air free flag
			if (i) this->i.fromScriptObj(*i);//Gets image
			
			if (inside){//If there's an inside variable
				deque<string> tokens = tokenize(inside->value, ", \t");//Splits content
				
				int n;//Counter
				for (n = 0; n < tokens.size() && n < 12; n++)//For each token
					this->inside[n] = getItem(tokens[n]);//Sets item
			}
			
		}
	}
	
	//Print function
	void print(SDL_Surface* target, int x, int y){
		i.print(target, x, y);
	}
	
	//Function to get the first free slot of inventory (or -1 if none)
	int freeSlot(){
		int i;
		for (i = 0; i < 12; i++) if (inside[i] == NULL) return i;
		return -1;
	}
};

//Function determining if a deco comes before another one (lower y or equal y and lower x)
bool sortDecos_compare(deco* a, deco* b){
	return a->y < b->y || (a->y == b->y && a->x < b->x);
}

list <deco> decoDb;//Decos database

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
	image portrait;//Unit portrait
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
	list<damType> resistances;//Per-type resistances
	
	//Experience
	int xp, level;//Current xp and level
	int xpReward;//Unit xp reward
	
	//Fighting
	weapon *primary;//Primary weapon
	weapon *spell;//Active spell
	
	//Clothes
	cloth* head;
	cloth* body;
	cloth* legs;
	
	//Items
	item *inv [12];//Inventory items
	
	//Various
	bool moved;//Flag indicating if unit has moved
	bool flying;//Flag indicating if unit can fly
	
	unit* lastEffectOwner;//Owner of the last effect applied to unit (for XP giving)
	
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
		
		xp = 0;
		level = 1;
		xpReward = 1;
		
		primary = NULL;
		spell = NULL;
		
		head = NULL;
		body = NULL;
		legs = NULL;
		
		int i;
		for (i = 0; i < 12; i++) inv[i] = NULL;
		
		moved = false;
		flying = false;
		
		lastEffectOwner = NULL;
	}
	
	//Unit printing function
	void print(SDL_Surface* target, int x, int y){
		//Prints hitbar
		if (hits() > 0 && maxHits() > 0){
			SDL_Rect hitBar = {x + anims.current()->current()->cShiftX - tilesSide / 2 + dX + 2, y + anims.current()->current()->cShiftY + anims.current()->current()->rect.h / 2 + dY, hits() * (tilesSide - 4) / maxHits(), 4};
			boxColor(target, hitBar.x, hitBar.y, hitBar.x + hitBar.w, hitBar.y + hitBar.h, 0xA000007A);
		}
		
		anims.print(target, x + dX, y + dY);//Prints using animSet printing function (prints current frame of current animation)
		
		//Prints clothes
		if (head) head->print(target, x + dX, y + dY);
		if (body) body->print(target, x + dX, y + dY);
		if (legs) legs->print(target, x + dX, y + dY);
		
		if (GETACT(action) == ACT_STRIKE && primary) primary->print(target, x + dX, y + dY);//Prints weapon
		if (GETACT(action) == ACT_SPELL && spell) spell->print(target, x + dX, y + dY);//Prints spell
		
		//Prints effect icons
		list<effect>::iterator e;//Effect iterator
		int eX = x - tilesSide / 2 + dX + 4;
		for (e = tempEffects.begin(); e != tempEffects.end(); e++){//For each effect
			if (e->img){//If effect icon is available
				e->icon.print(target, eX, y + anims.current()->current()->cShiftY + anims.current()->current()->rect.h / 2 + dY - e->icon.rect.h / 2 - 1);
				eX += e->icon.rect.w + 1;
			}
		}
	}
	
	//Function to load from script object
	void fromScriptObj(object o){
		if (o.type == OBJTYPE_UNIT){//If type is matching
			content::fromScriptObj(o);//Loads base content data
			
			object* anims = o.getObj("anims");//AnimSet object
			var* hits = o.getVar("hits");//Hits variable
			var* mana = o.getVar("mana");//Mana variable
			var* strength = o.getVar("strength");//Strength variable
			var* intelligence = o.getVar("intelligence");//Intelligence variable
			var* constitution = o.getVar("constitution");//Constitution variable
			var* wisdom = o.getVar("wisdom");//Wisdom variable
			var* sight = o.getVar("sight");//Sight variable
			var* flying = o.getVar("flying");//Flying variable
			var* xpReward = o.getVar("xpReward");//XP reward
			object* portrait = o.getObj("portrait");//Unit portrait
			
			if (anims) this->anims.fromScriptObj(*anims);//Loads animations
			if (hits) this->baseMaxHits = hits->intValue();//Gets max hits
			if (mana) this->baseMaxMana = mana->intValue();//Gets mana
			
			//Gets stats
			if (strength) this->baseStrength = strength->intValue();
			if (constitution) this->baseConstitution = constitution->intValue();
			if (intelligence) this->baseIntelligence = intelligence->intValue();
			if (wisdom) this->baseWisdom = wisdom->intValue();
			
			if (sight) this->sight = sight->intValue();//Gets sight
			if (flying) this->flying = flying->intValue();//Gets flying
			
			if (xpReward) this->xpReward = xpReward->intValue();//Gets xp reward
			
			if (portrait) this->portrait.fromScriptObj(*portrait);//Gets portrait
			
			this->anims.setAnim("idle_s");//Turns south
			this->baseHits = this->baseMaxHits;//Sets hits
			this->baseMana = this->baseMaxMana;//Sets mana
			
			deque<object>::iterator i;//Sub object iterator
			for (i = o.o.begin(); i != o.o.end(); i++){//For each sub-object
				if (i->type == OBJTYPE_DAMTYPE){//If object is a damage type
					damType newDT;//New damage type - resistance
					newDT.fromScriptObj(*i);//Loads resistance
					resistances.push_back(newDT);//Adds to resistances
				}
			}
		}
	}
	
	//Function to set idle
	void stop(){
		action = GETACODE(ACT_IDLE, GETDIR(action));//Sets action code
		anims.setAnim("idle_" + dirToString(GETDIR(action)));//Sets animation
		if (head) head->overlay.setAnim("idle_" + dirToString(GETDIR(action)));//Sets cloths animation
		if (body) body->overlay.setAnim("idle_" + dirToString(GETDIR(action)));//Sets cloths animation
		if (legs) legs->overlay.setAnim("idle_" + dirToString(GETDIR(action)));//Sets cloths animation
	}
	
	//Walking function
	void walk(int, bool = true);
	
	//Turning function (faces given direction)
	void turn(int direction){
		action = GETACODE(ACT_IDLE, direction);//Sets action code
		anims.setAnim("idle_" + dirToString(direction));//Sets animation
		if (head) head->overlay.setAnim("idle_" + dirToString(direction));//Sets cloths animation
		if (body) body->overlay.setAnim("idle_" + dirToString(direction));//Sets cloths animationi
		if (legs) legs->overlay.setAnim("idle_" + dirToString(direction));//Sets cloths animation
	}
	
	//Striking function
	void strike(){
		if (primary){//If weapon is valid
			action = GETACODE(ACT_STRIKE, GETDIR(action));//Sets action code
			anims.setAnim(primary->strikeAnim + "_" + dirToString(GETDIR(action)));//Sets striking animation
			if (head) head->overlay.setAnim(primary->strikeAnim + "_" + dirToString(GETDIR(action)));//Sets cloths animation
			if (body) body->overlay.setAnim(primary->strikeAnim + "_" + dirToString(GETDIR(action)));//Sets cloths animation
			if (legs) legs->overlay.setAnim(primary->strikeAnim + "_" + dirToString(GETDIR(action)));//Sets cloths animation
			
			if (primary->imgGiven){
				primary->overlay.setAnim("strike_" + dirToString(GETDIR(action)));//Sets weapon animation
				primary->overlay.current()->curFrame = 0;//Restarts animation
			}
			
			moved = true;
		}
		
		else stop();
	}
	
	//Spellcast function
	void spellcast(){
		if (spell && mana() >= -spell->onStrike.mana){//If weapon is valid and there's enough mana to cast spell
			action = GETACODE(ACT_SPELL, GETDIR(action));//Sets action code
			anims.setAnim(spell->strikeAnim + "_" + dirToString(GETDIR(action)));//Sets striking animation
			if (head) head->overlay.setAnim(spell->strikeAnim + "_" + dirToString(GETDIR(action)));//Sets cloths animation
			if (body) body->overlay.setAnim(spell->strikeAnim + "_" + dirToString(GETDIR(action)));//Sets cloths animation
			if (legs) legs->overlay.setAnim(spell->strikeAnim + "_" + dirToString(GETDIR(action)));//Sets cloths animation
			
			if (spell->imgGiven){
				spell->overlay.setAnim("cast_" + dirToString(GETDIR(action)));//Sets weapon animation
				spell->overlay.current()->curFrame = 0;//Restarts animation
			}
			
			moved = true;
		}
		
		else stop();
	}
	
	//Unit rests - skips turn, recovers HP - MP
	void rest(){
		if (hits() < maxHits() / 2)	varyHits (CALC_RECOVERY(strength(), constitution()));//Recovers health
		if (mana() < maxMana() / 2) varyMana (CALC_RECOVERY(intelligence(), wisdom()));//Recovers mana
		moved = true;
	}
	
	//Function to kill unit
	void kill(){
		action = ACT_DYING;//Sets action code
		anims.setAnim("dying");//Sets animation
		if (head) head->overlay.setAnim("dying");//Sets cloths animation
		if (body) body->overlay.setAnim("dying");//Sets cloths animation
		if (legs) legs->overlay.setAnim("dying");//Sets cloths animation
		
		if (lastEffectOwner) lastEffectOwner->xpGain(xpReward);//Effect owner earns xp
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
		else if (GETACT(o.code) == ACT_SPELL) spellcast();//Checks for spellcast orders
		else if (GETACT(o.code) == ACT_REST) rest();//Checks for resting orders
		else if (GETACT(o.code) == ACT_TURN) turn(GETDIR(o.code));//Checks for turning orders
		else stop();//Stops on any other code
	}
	
	//Function to vary hits
	void varyHits(int amount, damType *type = NULL){
		if (type && amount < 0){//If damage type was given and unit was damaged (amount < 0)
			damType* res = get(&resistances, type->id);//Gets resistance
			baseHits += CALC_DAMAGETAKEN(amount, (type->superType == DAM_PHYSICAL ? constitution() : wisdom()), (res ? res->mult : 1));//Damages unit calculating damage
		}
		
		else baseHits += amount;//Else just varies base hits
		
		if (hits() > maxHits()) baseHits -= hits() - maxHits();//Adjust to stay below maxHits
		if (hits() <= 0) kill();//Kills unit if no hits left
	}
	
	//Function to vary mana amount
	void varyMana(int amount){
		baseMana += amount;//Varies mana
		
		if (mana() > maxMana()) baseMana -= mana() - maxMana();//Adjust to stay below maxHits
	}
	
	//Function to give primary melee weapon to unit
	void giveWeapon_primary(string id){
		weapon *w = new weapon;
		if(get(&weaponDb, id)) *w = *get(&weaponDb, id);//Sets weapon
		
		primary = w;//Sets primary weapon
	}
	
	//Function to wear generic cloth
	void wear(string id){
		cloth* c = new cloth;//New cloth
		if (get(&clothDb, id)) *c = *get(&clothDb, id);//Gets cloth
		
		if (c){//If cloth exists
			if (c->type == CLOTH_HEAD) head = c;
			if (c->type == CLOTH_BODY) body = c;
			if (c->type == CLOTH_LEGS) legs = c;
		}
	}
	
	//Function to get the first free slot of inventory (or -1 if none)
	int freeSlot(){
		int i;
		for (i = 0; i < 12; i++) if (inv[i] == NULL) return i;
		return -1;
	}
	
	//Function to give an item (disposable, weapon, clothing) to the unit
	void giveItem(string id){
		item *a = (item*) get(&itemDb, id);//Gets item from item database
		if (!a) a = (item*) get(&weaponDb, id);//Gets item from weapon database if not found
		if (!a) a = (item*) get(&clothDb, id);//Gets item from clothing database if not found
		
		if (!a) return;//Exits function if nothing was found
		
		int slot = freeSlot();//Gets free slot
		if (slot != -1){//If there's a free slot
			if (a->itemType == DISPOSABLE){//If item is disposable
				disposable* d = new disposable;
				*d = *((disposable*) a);
				inv[slot] = (item*) d;
			}
			
			if (a->itemType == WEAPON || a->itemType == SPELL){//If item is weapon
				weapon* w = new weapon;
				*w = *((weapon*) a);
				inv[slot] = (item*) w;
			}
			
			if (a->itemType == CLOTHING){//If item is clothing
				cloth* c = new cloth;
				*c = *((cloth*) a);
				inv[slot] = (item*) c;
			}
		}
	}
	
	//Function to make AI move (prototype)
	void AI(bool = true);
	
	//Function to get unit hits applying effects
	int hits(){
		return baseHits;//Return plain hits (no permanent-temporary effects can affect hits)
	}
	
	//Functio to get max unit hits applying effects
	int maxHits(){
		int result = baseMaxHits + (primary ? primary->onEquip.maxHits : 0);//Base max hits + primary weapon bonus
		
		//Gets clothing bonus
		if (head) result += head->onEquip.maxHits;	
		if (body) result += body->onEquip.maxHits;
		if (legs) result += legs->onEquip.maxHits;
		
		list <effect>::iterator e;//Effect iterator
		for (e = tempEffects.begin(); e != tempEffects.end(); e++) result += e->maxHits;//Adds each temporary effect
		
		return (result > 1 ? result : 1);//Returns result
	}
	
	//Function to get unit mana applying effects
	int mana(){
		return baseMana;
	}
	
	//Function to get unit max mana applying effects
	int maxMana(){
		int result = baseMaxMana + (primary ? primary->onEquip.maxHits : 0);//Base max mana + primary weapon bonus
		
		//Gets clothing bonus
		if (head) result += head->onEquip.maxMana;	
		if (body) result += body->onEquip.maxMana;
		if (legs) result += legs->onEquip.maxMana;
		
		list <effect>::iterator e;//Effect iterator
		for (e = tempEffects.begin(); e != tempEffects.end(); e++) result += e->maxMana;//Adds each temporary effect
		
		return (result > 1 ? result : 1);//Returns result
	}
	
	//Function to get unit strength applying effects
	int strength(){
		int result = baseStrength + (primary ? primary->onEquip.strength : 0);//Base strength + primary weapon bonus
		
		//Gets clothing bonus
		if (head) result += head->onEquip.strength;	
		if (body) result += body->onEquip.strength;
		if (legs) result += legs->onEquip.strength;
		
		list <effect>::iterator e;//Effect iterator
		for (e = tempEffects.begin(); e != tempEffects.end(); e++) result += e->strength;//Adds each temporary effect
		
		return (result > 0 ? result : 0);//Returns result
	}
	
	//Function to get unit intelligence applying effects
	int intelligence(){
		int result = baseIntelligence + (primary ? primary->onEquip.intelligence : 0);//Base intelligence + primary weapon bonus
		
		//Gets clothing bonus
		if (head) result += head->onEquip.intelligence;	
		if (body) result += body->onEquip.intelligence;
		if (legs) result += legs->onEquip.intelligence;
		
		list <effect>::iterator e;//Effect iterator
		for (e = tempEffects.begin(); e != tempEffects.end(); e++) result += e->intelligence;//Adds each temporary effect
		
		return (result > 0 ? result : 0);//Returns result
	}
	
	//Function to get unit constitution applying effects
	int constitution(){
		int result = baseConstitution + (primary ? primary->onEquip.constitution : 0);//Base constitution + primary weapon bonus
		
		//Gets clothing bonus
		if (head) result += head->onEquip.constitution;
		if (body) result += body->onEquip.constitution;
		if (legs) result += legs->onEquip.constitution;
		
		list <effect>::iterator e;//Effect iterator
		for (e = tempEffects.begin(); e != tempEffects.end(); e++) result += e->constitution;//Adds each temporary effect
		
		return (result > 0 ? result : 0);//Returns result
	}
	
	//Function to get unit wisdom applying effects
	int wisdom(){
		int result = baseWisdom + (primary ? primary->onEquip.wisdom : 0);//Base wisdom + primary weapon bonus
		
		//Gets clothing bonus
		if (head) result += head->onEquip.wisdom;
		if (body) result += body->onEquip.wisdom;
		if (legs) result += legs->onEquip.wisdom;
		
		list <effect>::iterator e;//Effect iterator
		for (e = tempEffects.begin(); e != tempEffects.end(); e++) result += e->wisdom;//Adds each temporary effect
		
		return (result > 0 ? result : 0);//Returns result
	}
	
	//Function to get base damage multiplier for given type
	double baseMult(string tId){
		damType* base = get(&resistances, tId);//Base resistance
		
		if (base) return base->mult;//Returns result
		else return 1;
	}
	
	//Function to get damage multiplier for given damage type
	double typeMult(string tId){
		double result = baseMult(tId);//Result
		
		list<effect>::iterator i;//Effect iterator
		for (i = tempEffects.begin(); i != tempEffects.end(); i++){//For each temporary effect
			damType* res = get(&i->resistances, tId);//Effect resistance bonus
			if (res) result += res->mult;//Adds effect to result
		}
		
		if (head){//If there's a head clothing
			damType* res = get(&head->onEquip.resistances, tId);//Head resistance bonus
			if (res) result += res->mult;//Adds effect to result
		}
		
		if (body){//If there's a body clothing
			damType* res = get(&body->onEquip.resistances, tId);//Body resistance bonus
			if (res) result += res->mult;//Adds effect to result
		}
		
		if (legs){//If there's a legs clothing
			damType* res = get(&legs->onEquip.resistances, tId);//Legs resistance bonus
			if (res) result += res->mult;//Adds effect to result
		}
		
		if (primary){//If there's a primary weapon
			damType* res = get(&primary->onEquip.resistances, tId);//Primary weapon resistance bonus
			if (res) result += res->mult;//Adds effect to result
		}
		
		return result;
	}
	
	//Function to apply an effect to the unit
	void applyEffect(effect e){
		if (e.hits < 0) lastEffectOwner = e.owner;//If damage effect, saves owner
		
		if (e.duration == 0){//Instant/permanente effect
			//Applies effect to stats
			varyHits(e.hits, &e.type);
			varyMana(e.mana);
			baseMaxHits += e.maxHits;
			baseMaxMana += e.maxMana;
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
			if (e->duration <= 0){ e = tempEffects.erase(e); e--;}//Removes effect if finished
			
			else if (e->duration >= 0) {//Else
				e->duration--;//Reduces duration by 1
				
				//Applies time effects (basically hits, mana)
				varyHits(e->hits, &e->type);
				varyMana(e->mana);
			}
		}
	}
	
	//Function for xp gain
	void xpGain(int amount){
		xp += amount;//Adds xp
	}
};

//Function determining if an unit comes before another one (lower y or equal y and lower x)
bool sortUnits_compare(unit* a, unit* b){
	return a->y < b->y || (a->y == b->y && a->x < b->x);
}

list<unit> unitDb;//Units database

//Function to get weapon effect on target with bonuses related to owner stats (mainly damage bonuses)
effect weapon::getEff(unit* u){
	effect e = onTarget;//Base effect
	e.hits = CALC_DAMAGEDEALT(e.hits, (e.type.superType == DAM_PHYSICAL ? u->strength() : u->intelligence()));//Calculates damage
	e.owner = u;//Sets owner
	return e;//Returns effect
}

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
		if (keys[SDLK_w]){ giveOrder({GETACODE(ACT_WALK, NORTH), ""}); return true;}
		else if (keys[SDLK_a]){ giveOrder({GETACODE(ACT_WALK, WEST), ""}); return true;}
		else if (keys[SDLK_s]){ giveOrder({GETACODE(ACT_WALK, SOUTH), ""}); return true;}
		else if (keys[SDLK_d]){ giveOrder({GETACODE(ACT_WALK, EAST), ""}); return true;}
		
		//Striking orders
		else if (keys[SDLK_q]){ giveOrder({ACT_STRIKE, ""}); return true;}
		else if (keys[SDLK_e]){ giveOrder({ACT_SPELL, ""}); return true;}
		
		//Resting orders
		else if (keys[SDLK_RETURN]){ giveOrder({ACT_REST, ""}); return true;}
		
		return false;
	}
	
	//Function to get if all controlled units are ready
	bool ready(){
		deque<unit*>::iterator u;//Unit iterator
		for (u = units.begin(); u != units.end(); u++)//For each unit
			if (!(*u)->ready()) return false;//Returns false if unit is not ready
		
		return true;//Returns true if units were all ready
	}
	
	//Function to get if all units have been moved
	bool moved(){
		deque<unit*>::iterator u;//Unit iterator
		for (u = units.begin(); u != units.end(); u++)//For each unit
			if (!(*u)->moved) return false;//Returns false if unit hasn't been moved
		
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
	bool AI();
	
	//Function to reset moved flag for all units
	void resetMoved(){
		deque<unit*>::iterator u;//Unit iterator
		for (u = units.begin(); u != units.end(); u++)//For each unit
			(*u)->moved = false;
	}
	
	//Function returning if there are units to be controlled
	bool valid(){
		deque<unit*>::iterator i;//Iterator
		for (i = units.begin(); i != units.end(); i++)//For each unit
			if ((*i)->ready()) return true;//If unit is ready returns true
			
		return false;//Returns false
	}
};

//Function to compare objects by position
template <class aClass, class bClass> bool compare_position (aClass* a, bClass* b){
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
	
	deque<unit*> units;//Units on map
	deque<deco*> decos;//Decorations on map
	deque<projectile*> projs;//Projectiles on map
	
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
	
	//Function to sort projectiles according to position
	void sortProjs(){
		sort(projs.begin(), projs.end(),sortProj_compare);
	}
	
	//Function to create an unit on map
	unit* createUnit(string id, string name, int x, int y, int side){
		unit* u = get(&unitDb, id);//Required unit
		
		if (u && isFree(x, y, u->flying)){//If unit exists and tile is free
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
		deque<projectile*>::iterator p = projs.begin();//Projectile iterator
		
		while (u != units.end() || d != decos.end() || p != projs.end()){//While there's still something to print
			if (u != units.end() && (d == decos.end() || compare_position(*u, *d)) && (p == projs.end() || compare_position(*u, *p))){//If unit is before deco and projectile
				(*u)->print(target, offset.x + tilesSide * (*u)->x + tilesSide / 2, offset.y + tilesSide * (*u)->y + tilesSide / 2);//Prints unit
				u++;//Next unit
			}
			
			else if (d != decos.end() && (p == projs.end() || compare_position(*d, *p))){//Else if deco is before projectile
				(*d)->print(target, offset.x + tilesSide * (*d)->x + tilesSide / 2, offset.y + tilesSide * (*d)->y + tilesSide / 2);//Prints deco
				d++;//Next deco
			}
			
			else if (p != projs.end()){//Else
				(*p)->print(target, offset.x + tilesSide * (*p)->x + tilesSide / 2, offset.y + tilesSide * (*p)->y + tilesSide / 2);//Prints projectile
				p++;//Next projectile
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
			
			makePict(true);//Makes picture
			makeMmap();//Makes minimap
		}
		
		#ifdef _ERROR
			else if (errFunc) errFunc(SCRIPT_WARNING_INVALIDOBJTYPE, "[map.h] No map could be loaded from " + o.type + " object");
		#endif
	}
	
	//Function returning if tile is free
	bool isFree(int x, int y, bool flying = false){
		if (getTile(x, y)){//If terrain exists
			terrain* tile = getTile(x, y);//Gets terrain
			
			if (!flying){
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
					if ((*d)->x == x && (*d)->y == y && !(*d)->free) return false;//If deco is in tile returns false
					
				return true;//Returns true if no unit was found
			}
			
			else {
				//Checks if terrain is not free or there's a transition from a non-free terrain
				if (!tile->free_air) return false;
				if (getTile(x, y - 1) && !getTile(x, y - 1)->free_air && getTile(x, y - 1)->layer > tile->layer) return false;
				if (getTile(x + 1, y - 1) && !getTile(x + 1, y - 1)->free_air && getTile(x + 1, y - 1)->layer > tile->layer) return false;
				if (getTile(x + 1, y) && !getTile(x + 1, y)->free_air && getTile(x + 1, y)->layer > tile->layer) return false;
				if (getTile(x + 1, y + 1) && !getTile(x + 1, y + 1)->free_air && getTile(x + 1, y + 1)->layer > tile->layer) return false;
				if (getTile(x, y + 1) && !getTile(x, y + 1)->free_air && getTile(x, y + 1)->layer > tile->layer) return false;
				if (getTile(x - 1, y + 1) && !getTile(x - 1, y + 1)->free_air && getTile(x - 1, y + 1)->layer > tile->layer) return false;
				if (getTile(x - 1, y) && !getTile(x - 1, y)->free_air && getTile(x - 1, y)->layer > tile->layer) return false;
				if (getTile(x - 1, y - 1) && !getTile(x - 1, y - 1)->free_air && getTile(x - 1, y - 1)->layer > tile->layer) return false;
				
				deque<unit*>::iterator u;//Unit iterator
				for (u = units.begin(); u != units.end(); u++)//For each unit
					if ((*u)->x == x && (*u)->y == y) return false;//If unit is in tile returns false
					
				deque<deco*>::iterator d;//Deco iterator
				for (d = decos.begin(); d != decos.end(); d++)//For each deco
					if ((*d)->x == x && (*d)->y == y && !(*d)->free_air) return false;//If deco is in tile returns false
					
				return true;//Returns true if no unit was found
			}
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
		
		for (i = 0; i < projs.size(); i++){//For each projectile
			int oldSize = projs.size();//Projectile amount before this loop
			
			projs[i]->nextFrame();//Unit next frame
			
			if (projs.size() != oldSize) i -= oldSize - projs.size();//Corrects counter position if deque size has changed
		}
	}
	
	//Function to apply effect to unit in given tile
	void applyEffect(int x, int y, effect e, int direction){
		deque<unit*>::iterator i;//Iterator
		
		if (e.shot != ""){//If there's a projectile to shoot
			shoot (x, y, direction, e.shot, e.owner);//Shoots
		}
		
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
		
		for (i = units.begin(); i != units.end(); i++){//For each unit
			if ((*i)->name == name) return *i;//Returns unit if name is matching
		}
			
		return NULL;//Returns null if no unit was found
	}
	
	//Function to shoot a projectile
	void shoot(int x, int y, int dir, string pId, unit* owner = NULL){
		projectile* p = new projectile;//New projectile
		projectile* q = get(&projectileDb, pId);//Gets projectile
		
		if (q) *p = *q;
		p->x = x;
		p->y = y;
		p->direction = dir;
		p->anims.setAnim(dirToString(dir));
		p->parent = this;
		p->onTarget.owner = owner;		
		
		projs.push_back(p);//Adds projectile
	}
	
	//Function to determine if there are still projectiles running
	bool projectiles(){
		return projs.size() == 0;
	}
};

list <map> mapDb;//Maps database

//Script class
class script {
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
	
	//Execution function - campaign
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
				
				if (i->flags & EVENT_ONCE){ i = events.erase(i); i--; }//Erases event if has to be triggered only once
			}
		}
	}
};

//Unit query class
class uQuery {
	public:
	string id;//Query id
	deque<unit*> units;//Units in query
	
	//Constructor
	uQuery(){
		id = "";
	}
	
	//Function to remove dead units from query
	void clearDead(){
		deque<unit*>::iterator i;//Unit iterator
		
		for (i = units.begin(); i != units.end(); i++){//For each unit in query
			if ((*i)->hits() <= 0){ i = units.erase(i); i--; }//Erases unit if dead
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
	
	deque<uQuery> queries;//Unit queries
	
	deque<sequence> seq;//Sequences of campaign, in order of execution
	int curSequence;//Current sequence index
	
	int turnCount;//Turn count

	deque<string> dialogSeq;//Current dialog sequence
	deque<unit*> dialogActors;//Current dialog actors
	
	deque<string> dialogAns[4];//Current dialog answers
	deque<string> dialogOutVar;//Output variable (used to store answers)
	
	int view;//Current view
	
	string questName, questInfo;//Quest name and information strings
	
	deque<var> variables;//Campaign global variables
	
	//Constructor
	campaign(){
		m = NULL;
		curSequence = 0;
		turn = 0;
		turnCount = 1;
		
		view = GAME;
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
	
	//Function to count dialog answers
	int ansCount(){
		int result = 0;//Current answers count
		if (dialogAns[0].size() > 0 && dialogAns[0][0] != "") result++;
		if (dialogAns[1].size() > 0 && dialogAns[1][0] != "") result++;
		if (dialogAns[2].size() > 0 && dialogAns[2][0] != "") result++;
		if (dialogAns[3].size() > 0 && dialogAns[3][0] != "") result++;
		return result;
	}
	
	//Printing function
	void print(SDL_Surface* target, int x, int y){
		if (m){//If map is valid
			if (player.units.size() > 0)//If there's at least one controlled unit
				m->print(target, x, y, player.units[0]);//Prints map centered on first controlled unit
			
			else m->print (target, x, y);//Else prints map normally
		}
				
		//Prints hud
		if (player.units.size() > 0){//If there's a valid unit
			unit* hero = player.units[0];//Gets hero
			
			if (bar_hits){//If there's a valid hitbar
				SDL_Rect o = bar_hits_offset;//Copies offset
				
				SDL_Rect clip;//Clip rectangle
				clip.x = 0;
				clip.y = 0;
				clip.h = bar_hits->h;//Sets clip height
				clip.w = bar_hits->w * hero->hits() / hero->maxHits();//Sets clip width
				
				SDL_BlitSurface(bar_hits, &clip, target, &o);//Prints hitbar
			}
			
			if (bar_mana){//If there's a valid manabar
				SDL_Rect o = bar_mana_offset;//Copies offset
				
				SDL_Rect clip;//Clip rectangle
				clip.x = 0;
				clip.y = 0;
				clip.h = bar_mana->h;//Sets clip height
				clip.w = bar_mana->w * hero->mana() / hero->maxMana();//Sets clip width
				
				SDL_BlitSurface(bar_mana, &clip, target, &o);//Prints manabar
			}
			
			if (bar_xp){//If there's a valid xp bar
				SDL_Rect o = bar_xp_offset;//Copies offset
				
				SDL_Rect clip;//Clip rectangle
				clip.x = 0;
				clip.y = 0;
				clip.h = bar_xp->h;//Sets clip height
				clip.w = bar_xp->w * hero->xp / CALC_REQUIREDXP(hero->level);//Sets clip width
				
				SDL_BlitSurface(bar_xp, &clip, target, &o);//Prints xp bar
			}
		}
		
		if (bar_xp_frame){//If there's a valid xp bar frame
			SDL_Rect o = bar_xp_frame_offset;//Copies offset
			SDL_BlitSurface(bar_xp_frame, NULL, target, &o);//Blits frame
		}
		
		if (bar_frame){//If there's a valid bar frame
			SDL_Rect o = bar_frame_offset;//Copies offset
			SDL_BlitSurface(bar_frame, NULL, target, &o);//Blits frame
		}
		
		//Dialogs
		if (dialogSeq.size() > 0){//If needs to show dialog
			dialogBox.text = dialogSeq[0];
			dialogBox.print(target);
		}
		
		if (dialogActors.size() > 0 && dialogActors[0]){//If needs to show actor
			dialogBox_actor.i = &dialogActors[0]->portrait;//Sets portrait
			dialogBox_actor.id = dialogActors[0]->name;//Sets name
			dialogBox_actor.print(target);
		}
	
		if (dialogSeq.size() > 0 && ansCount() > 0){//If there are answers
			int n;//Counter
			for (n = 0; n < ansCount(); n++){//For each answer
				btn_answers[n].text = dialogAns[n][0];//Sets text
				btn_answers[n].x = dialogBox.x + dialogBox.w / 2 - (btn_answers[n].w * ansCount() + 10 * (ansCount() - 1)) / 2 + n * (btn_answers[n].w + 10);
				btn_answers[n].print(target);
			}
		}
		
		//Player info view
		if (view == PLAYER){
			updateInfoPanel();//Updates info panel
			infoPanel.print(target);//Prints info panel
		}
		
		//Inventory
		if (view == INVENTORY){
			updateInventoryPanel();//Updates inventory panel
			inventoryPanel.print(target);//Prints inventory panel
		}
		
		//Level up 1
		if (view == LEVELUP_1){
			hp_mp_levelUp.print(target);//Prints levelup panel
		}
		
		//Level up 2
		if (view == LEVELUP_2){
			stats_levelUp.print(target);//Prints levelup panel
		}
		
		//Quest info
		if (view == QUEST){
			updateQuestPanel();//Updates quests panel
			questPanel.print(target);//Prints quests panel
		}
		
		//Exchange view
		if (view == EXCHANGE){
			exchangePanel.print(target);//Prints exchange panel
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
		if (m && view == GAME){
			m->nextFrame();//Goes to map next frame if possible
			seq[curSequence].checkEvents(this);//Checks sequence events
		}
	}
	
	//Turn moves function
	void turnMoves(){
		if (dialogSeq.size() > 0 || view != GAME) return;//Does nothing if there's dialog text or is not in game
		
		if (turn == 0 && player.ready()){//If player turn
			if (!player.moved()) player.getInput();//Gets player input
			if (player.moved() && player.ready() && m->projectiles()) nextTurn();//Goes to AI turn if moved
		}
		
		if (turn == 1 && player.ready() && ai.readyOrDead() && ai.AI())//When all AI units have been moved
			nextTurn();//Next turn
	}
	
	//Campaign setup function
	void setup(){
		curSequence = 0;//Sets first sequence
		if (seq.size() > 0) seq.front().begin.exec(this);//Execs first sequence beginning script
	}
	
	//End turn function
	void nextTurn(){
		deque<uQuery>::iterator i;//Query iterator
		for (i = queries.begin(); i != queries.end(); i++) i->clearDead();//Removes dead units from query
		
		if (player.units.size() > 0 && player.units[0]->xp >= CALC_REQUIREDXP(player.units[0]->level)){//If player reached new level
			view = LEVELUP_1;//Shows level up dialog
			
			player.units[0]->xp -= CALC_REQUIREDXP(player.units[0]->level);//Removes xp
			player.units[0]->level++;//Next level
		}
		
		if (turn == 0) turn++;
		
		else if (turn == 1){
			m->tempEffects();//Applies temporary effects
			
			turn = 0;//Restarts turn
			turnCount++;//Increases turn count
			
			player.resetMoved();
			ai.resetMoved();
		}
	}
	
	//Function to move on to next dialog
	void nextDialog(){
		if (dialogSeq.size() > 0){//If there are still dialogs
			dialogSeq.pop_front();//Removes first dialog text
		}
		
		if (dialogActors.size() > 0){//If there are dialog actors
			dialogActors.pop_front();//Removes first dialog actor
		}
		
		if (dialogAns[0].size() > 0) dialogAns[0].pop_front();
		if (dialogAns[1].size() > 0) dialogAns[1].pop_front();
		if (dialogAns[2].size() > 0) dialogAns[2].pop_front();
		if (dialogAns[3].size() > 0) dialogAns[3].pop_front();
		
		if (dialogOutVar.size() > 0) dialogOutVar.pop_front();
	}
	
	//Function to update info panel content
	void updateInfoPanel(){
		if (player.units.size() > 0){//If there's a valid unit
			//Gets stats
			int str = player.units[0]->strength();
			int con = player.units[0]->constitution();
			int intel = player.units[0]->intelligence();
			int wis = player.units[0]->wisdom();
			
			//Sets colors
			if (str > player.units[0]->baseStrength) strBox.foreColor = infoPanel_col3;
			else if (str < player.units[0]->baseStrength) strBox.foreColor = infoPanel_col4;
			else strBox.foreColor = infoPanel_col2;
			
			if (con > player.units[0]->baseConstitution) conBox.foreColor = infoPanel_col3;
			else if (con < player.units[0]->baseConstitution) conBox.foreColor = infoPanel_col4;
			else conBox.foreColor = infoPanel_col2;
			
			if (intel > player.units[0]->baseIntelligence) intBox.foreColor = infoPanel_col3;
			else if (intel < player.units[0]->baseIntelligence) intBox.foreColor = infoPanel_col4;
			else intBox.foreColor = infoPanel_col2;
			
			if (wis > player.units[0]->baseWisdom) wisBox.foreColor = infoPanel_col3;
			else if (wis < player.units[0]->baseWisdom) wisBox.foreColor = infoPanel_col4;
			else wisBox.foreColor = infoPanel_col2;
			
			nameBox.id = player.units[0]->name;//Sets name box
			nameBox.i = &player.units[0]->portrait;//Sets name box picture
			hpBox.text = getText("hits_abbr") + ": " + toString(player.units[0]->hits()) + "/" + toString(player.units[0]->maxHits());//HP box
			mpBox.text = getText("mana_abbr") + ": " + toString(player.units[0]->mana()) + "/" + toString(player.units[0]->maxMana());//MP box
			xpBox.text = getText("level") + " " + toString(player.units[0]->level) + " - XP: " + toString(player.units[0]->xp) + "/" + toString(CALC_REQUIREDXP(player.units[0]->level));//Xp box
			strBox.text = getText("str_abbr") + ": " + toString(player.units[0]->strength());//Sets strength box
			conBox.text = getText("con_abbr") + ": " + toString(player.units[0]->constitution());//Sets constitution box
			intBox.text = getText("int_abbr") + ": " + toString(player.units[0]->intelligence());//Sets intelligence box
			wisBox.text = getText("wis_abbr") + ": " + toString(player.units[0]->wisdom());//Sets wisdom box
			
			//Sets effect box
			effBox.text = "";
			list<effect>::iterator i;//Effect iterator
			for (i = player.units[0]->tempEffects.begin(); i != player.units[0]->tempEffects.end(); i++)//For each temporary effect on unit
				effBox.text += i->description + " (" + toString(i->duration + 1) + ") \\n ";//Adds effect info to box
				
			//Sets resistance boxes
			list<textBox*>::iterator t;//Text box iterator
			for (t = resBox.begin(); t != resBox.end(); t++){//For each text box
				double effRes = player.units[0]->typeMult((*t)->id);//Gets effect resistance
				
				(*t)->text = get(&damTypeDb, (*t)->id)->shownName + " " + toString((1 - effRes) * 100) + "\%";//Sets label text
				
				if (effRes > 1) (*t)->foreColor = infoPanel_col4;
				else if (effRes < 1) (*t)->foreColor = infoPanel_col3;
				else if (effRes == 1) (*t)->foreColor = infoPanel_col2;
			}
		}
	}
	
	//Function to update inventory panel
	void updateInventoryPanel(){
		if (player.units.size() > 0){//If there's a valid unit
			//Prints equipped items boxes
			if (player.units[0]->head) headBox.i = &player.units[0]->head->icon;
			else headBox.i = NULL;
			
			if (player.units[0]->body) bodyBox.i = &player.units[0]->body->icon;
			else bodyBox.i = NULL;
			
			if (player.units[0]->legs) legsBox.i = &player.units[0]->legs->icon;
			else legsBox.i = NULL;
			
			if (player.units[0]->primary) weaponBox.i = &player.units[0]->primary->icon;
			else weaponBox.i = NULL;
			
			if (player.units[0]->spell) spellBox.i = &player.units[0]->spell->icon;
			else spellBox.i = NULL;
			
			//Prints inventory boxes
			int i;
			for (i = 0; i < 12; i++){
				if (player.units[0]->inv[i]) inv_slots[i].i = &player.units[0]->inv[i]->icon;
				else inv_slots[i].i = NULL;
			}
			
			//Sets use button text
			if (slot_selected != -1 && slot_selected < 12){
				item* i = player.units[0]->inv[slot_selected];//Selected item
				if (i && i->itemType == DISPOSABLE) btn_use.text = getText("use");
				else if (i) btn_use.text = getText("equip");
			}
			
			else if (slot_selected != -1 && slot_selected < 17)
				btn_use.text = getText("unequip");
			
			else btn_use.text = getText("use");
			
			//Sets info box
			if (slot_selected != -1){//If there's a selected item
				item* it = NULL;//Selected item
				if (slot_selected < 12) it = player.units[0]->inv[slot_selected];
				else switch (slot_selected){
					case 12: it = player.units[0]->head; break;
					case 13: it = player.units[0]->body; break;
					case 14: it = player.units[0]->legs; break;
					case 15: it = player.units[0]->primary; break;
					case 16: it = player.units[0]->spell; break;
				}
				
				if (it && (it->shownName != "" || it->description != "")){
					string typeString;
					switch (it->itemType){
						case DISPOSABLE: typeString = getText("item"); break;
						case WEAPON: typeString = getText("weapon"); break;
						case SPELL: typeString = getText("spell"); break;
						case CLOTHING:
							cloth* c = (cloth*) it;
							if (c->type == CLOTH_HEAD) typeString = getText("head");
							else if (c->type == CLOTH_BODY) typeString = getText("body");
							else if (c->type == CLOTH_LEGS) typeString = getText("legs");
							break;
					}
					
					itemInfoBox.text = it->shownName + " (" + typeString + ") \\n " + it->description;
				}
					
				else if (it) itemInfoBox.text = getText("noInfoAvailable");
				else itemInfoBox.text = getText("noItemSelected");
			}
			
			else itemInfoBox.text = getText("noItemSelected");
		}
	}
	
	//Function to update quest panel
	void updateQuestPanel(){
		questNameBox.text = getText("currentQuest") + " " + questName;
		questInfoBox.text = questInfo;
	}
	
	//Function to check events
	void events(SDL_Event e){
		if (view == GAME){
			if (ansCount() > 0) btn_answers[0].checkEvents(e);
			if (ansCount() > 1) btn_answers[1].checkEvents(e);
			if (ansCount() > 2) btn_answers[2].checkEvents(e);
			if (ansCount() > 3) btn_answers[3].checkEvents(e);
		}
		
		if (view == PLAYER) infoPanel.checkEvents(e);
		else if (view == INVENTORY) inventoryPanel.checkEvents(e);
		else if (view == LEVELUP_1) hp_mp_levelUp.checkEvents(e);
		else if (view == LEVELUP_2) stats_levelUp.checkEvents(e);
		else if (view == QUEST) questPanel.checkEvents(e);
	}
} current;

list<campaign> campaignDb;//Campaign database

//Function to handle use button click
void btn_use_click(control* p, mouseEvent ev){
	if (slot_selected == -1) return;//Exits if there's no slot selected
		
	item* i = current.player.units[0]->inv[slot_selected];//Item to use
	
	if (slot_selected < 12){//If inventory slot was selected
		if (i->itemType == DISPOSABLE){//If item can be used
			disposable *d = (disposable*) i;
			current.player.units[0]->applyEffect(d->use);//Applies effect on player
			current.player.units[0]->inv[slot_selected] = NULL;//Frees inventory slot
		}
		
		else if (i->itemType == WEAPON && !current.player.units[0]->primary){//if item is a weapon and player has no weapon
			current.player.units[0]->primary = (weapon*) i;//Equips weapon
			current.player.units[0]->inv[slot_selected] = NULL;//Removes from inventory
		}
		
		else if (i->itemType == SPELL && !current.player.units[0]->spell){//If item is a spell and player has no spell
			current.player.units[0]->spell = (weapon*) i;//Equips spell
			current.player.units[0]->inv[slot_selected] = NULL;//Removes from inventory
		}
		
		else if (i->itemType == CLOTHING){//If item is clothing
			cloth* c = (cloth*) i;//Converts to cloth
			
			if (c->type == CLOTH_HEAD && !current.player.units[0]->head){//If it's head clothing and player head is free
				current.player.units[0]->head = c;//Sets head
				current.player.units[0]->inv[slot_selected] = NULL;//Removes from inventory
				current.player.units[0]->head->overlay.setAnim(current.player.units[0]->anims.current()->id);//Sets anim
			}
			
			if (c->type == CLOTH_BODY && !current.player.units[0]->body){//If it's body clothing and player body is free
				current.player.units[0]->body = c;//Sets body
				current.player.units[0]->inv[slot_selected] = NULL;//Removes from inventory
				current.player.units[0]->body->overlay.setAnim(current.player.units[0]->anims.current()->id);//Sets anim
			}
			
			if (c->type == CLOTH_LEGS && !current.player.units[0]->legs){//If it's legs clothing and player legs are free
				current.player.units[0]->legs = c;//Sets legs
				current.player.units[0]->inv[slot_selected] = NULL;//Removes from inventory
				current.player.units[0]->legs->overlay.setAnim(current.player.units[0]->anims.current()->id);//Sets anim
			}
		}
	}
	
	else if (current.player.units[0]->freeSlot() != -1){//If equipped slot was selected
		int slot = current.player.units[0]->freeSlot();//First inventory free slot
		
		if (slot_selected == 12){//If selected head slot
			current.player.units[0]->inv[slot] = (cloth*) current.player.units[0]->head;
			current.player.units[0]->head = NULL;
		}
		
		if (slot_selected == 13){//If selected body slot
			current.player.units[0]->inv[slot] = (cloth*) current.player.units[0]->body;
			current.player.units[0]->body = NULL;
		}
		
		if (slot_selected == 14){//If selected legs slot
			current.player.units[0]->inv[slot] = (cloth*) current.player.units[0]->legs;
			current.player.units[0]->legs = NULL;
		}
		
		if (slot_selected == 15){//If selected weapon slot
			current.player.units[0]->inv[slot] = (weapon*) current.player.units[0]->primary;
			current.player.units[0]->primary = NULL;
		}
		
		if (slot_selected == 16){//If selected spell slot
			current.player.units[0]->inv[slot] = (weapon*) current.player.units[0]->spell;
			current.player.units[0]->spell = NULL;
		}
	}
}

//Function to handle drop button click
void btn_drop_click(control* p, mouseEvent ev){
	if (slot_selected == -1) return;//Exits function if no slot is selected
	current.player.units[0]->inv[slot_selected] = NULL;//Removes inventory object
}

//Function to handle hpUp click
void hpUp_click(control* p, mouseEvent ev){
	current.player.units[0]->baseMaxHits += 5;
	current.player.units[0]->varyHits(5);
	
	current.view = LEVELUP_2;
}

//Function to handle mpUp click
void mpUp_click(control* p, mouseEvent ev){
	current.player.units[0]->baseMaxMana += 5;
	current.player.units[0]->varyMana(5);
	
	current.view = LEVELUP_2;
}

//Function to handle strUp click
void strUp_click(control* p, mouseEvent ev){
	current.player.units[0]->baseStrength += 1;
	current.view = GAME;
}

//Function to handle conUp click
void conUp_click(control* p, mouseEvent ev){
	current.player.units[0]->baseConstitution += 1;
	current.view = GAME;
}

//Function to handle intUp click
void intUp_click(control* p, mouseEvent ev){
	current.player.units[0]->baseIntelligence += 1;
	current.view = GAME;
}

//Function to handle intUp click
void wisUp_click(control* p, mouseEvent ev){
	current.player.units[0]->baseWisdom += 1;
	current.view = GAME;
}

//Function to handle quit button click
void quitButtonClick(control* p, mouseEvent ev){
	current.view = GAME;
}

//Projectile next frame function
void projectile::nextFrame(){
	anims.next();
	
	switch (direction){
		case NORTH:
		dY -= PROJSTEP;
		if (dY <= -tilesSide) {dY = 0; y--; dist++;}
		break;
		
		case WEST:
		dX -= PROJSTEP;
		if (dX <= -tilesSide) {dX = 0; x--; dist++;}
		break;
		
		case SOUTH:
		dY += PROJSTEP;
		if (dY >= tilesSide) {dY = 0; y++; dist++;}
		break;
		
		case EAST:
		dX += PROJSTEP;
		if (dX >= tilesSide) {dX = 0; x++; dist++;}
		break;
	}
	
	if (parent){//If there's a parent map
		if (!parent->isFree(x, y, flying) || dist >= range - 1){//If reached destination
			parent->applyEffect(x, y, onTarget, direction);//Applies effect on parent map
			
			//Removes projectile from parent
			deque<projectile*>::iterator i;//Projectile iterator
			for (i = parent->projs.begin(); i != parent->projs.end(); i++) if (*i == this) {parent->projs.erase(i); break;}//Removes projectile if it is this
		}
	}
}

//Execution function
int script::exec(campaign* c){
	if (!c || !c->m) return false;//Exits function if no target was given
	
	int i;//Counter
	int result = VOID;//Script result (false on errors)
	
	deque<var> vars = c->variables;//Known variables
	
	for (i = 0; i < cmds.size(); i++){//For each instruction		
		deque<string> tokens = tokenize(cmds[i], "\t ");
		
		/*Variable substitution*/
		int j;//Counter
		for (j = 0; j < tokens.size(); j++){//For each token
			if (tokens[j][0] == '$'){//If token is a variable
				//Player info variables [to expand]
				if (tokens[j] == "$player.name") tokens[j] = c->player.units[0]->name;
				else if (tokens[j] == "$player.id") tokens[j] = c->player.units[0]->id;
				
				else if (tokens[j] == "$player.x") tokens[j] = toString(c->player.units[0]->x);
				else if (tokens[j] == "$player.y") tokens[j] = toString(c->player.units[0]->y);
				
				else if (tokens[j] == "$player.hits") tokens[j] = toString(c->player.units[0]->hits());
				if (tokens[j] == "$player.maxHits") tokens[j] = toString(c->player.units[0]->maxHits());
				if (tokens[j] == "$player.mana") tokens[j] = toString(c->player.units[0]->mana());
				if (tokens[j] == "$player.maxMana") tokens[j] = toString(c->player.units[0]->maxMana());
				if (tokens[j] == "$player.strength") tokens[j] = toString(c->player.units[0]->strength());
				if (tokens[j] == "$player.constitution") tokens[j] = toString(c->player.units[0]->constitution());
				if (tokens[j] == "$player.intelligence") tokens[j] = toString(c->player.units[0]->intelligence());
				if (tokens[j] == "$player.wisdom") tokens[j] = toString(c->player.units[0]->wisdom());
				
				//Query info variables [to expand]
				else if (tokens[j].substr(0, 7) == "$query."){//If variable is related to query
					string qName = tokens[j].substr(7, tokens[j].find(".", 7) - 7);//Query name
					string vName = tokens[j].substr(7 + qName.size() + 1);//Variable name
					
					uQuery *q = get(&c->queries, qName);//Requested query
					if (q){//If query was found
						if (vName == "count") tokens[j] = toString(q->units.size());//Query count
					}
				}
				
				//Unit info variable [to expand]
				else if (tokens[j].substr(0, 6) == "$unit."){//If variable is related to unit
					string uName = tokens[j].substr(6, tokens[j].find(".", 6) - 6);//Unit name
					string vName = tokens[j].substr(6 + uName.size() + 1);//Variable name
					
					unit* u = c->m->getUnit_name(uName);//Requested unit
					if (u){//If unit was found
						if (vName == "name") tokens[j] = u->name;
						else if (vName == "id") tokens[j] = u->id;
						
						else if (vName == "x") tokens[j] = toString(u->x);
						else if (vName == "y") tokens[j] = toString(u->y);
						
						else if (vName == "hits") tokens[j] = toString(u->hits());
						if (vName == "maxHits") tokens[j] = toString(u->maxHits());
						if (vName == "mana") tokens[j] = toString(u->mana());
						if (vName == "maxMana") tokens[j] = toString(u->maxMana());
						if (vName == "strength") tokens[j] = toString(u->strength());
						if (vName == "constitution") tokens[j] = toString(u->constitution());
						if (vName == "intelligence") tokens[j] = toString(u->intelligence());
						if (vName == "wisdom") tokens[j] = toString(u->wisdom());
					}
				}
				
				//Variable existance info
				else if (tokens[j].substr(0, 7) == "$exist."){//Asks for variable existance
					string vName = tokens[j].substr(7);//Variable name
					
					int result = 0;//Result
					int n;//Counter
					for (n = 0; n < vars.size(); n++) if (vars[n].name == vName) result = 1;//Sets result to true if found variable
					
					tokens[j] = toString(result);//Sets token
				}
				
				//Variable not found yet
				else {
					deque<var>::iterator v;//Variable iterator
					for (v = vars.begin(); v != vars.end(); v++){//For each variable
						if (v->name == tokens[j].substr(1)){//If variable matches
							tokens[j] = v->value;//Sets token
							break;//Exits loop
						}
					}
				}
			}
		}
		
		/*Variable handling instructions*/{
		if (tokens[0] == "int" && tokens.size() >= 3){//Integer variable declaration
			string s = "";//Expression
			int j;//Counter
			for (j = 2; j < tokens.size(); j++) s += tokens[j] + " ";//Adds each token to string
			
			var newVar (tokens[1], expr(s, &ops_int));//New variable
			vars.push_back(newVar);//Adds variable
			c->variables.push_back(newVar);//Adds variable
		}
		
		if (tokens[0] == "string" && tokens.size() >= 3){//String variable declaration
			string s = "";//Expression
			int j;//Counter
			for (j = 2; j < tokens.size(); j++) s += tokens[j] + " ";//Adds each token to string
			
			var newVar (tokens[1], expr(s, &ops_str));//New variable
			vars.push_back(newVar);//Adds variable
			c->variables.push_back(newVar);//Adds variable
		}
		
		if (tokens[0] == "var" && tokens.size() >= 3){//General variable declaration
			string s = "";//Expression
			int j;//Counter
			for (j = 2; j < tokens.size(); j++) s += tokens[j] + " ";//Adds each token to string
			s.erase(s.size() - 1);//Removes last space
			
			var newVar(tokens[1], s);//New variable
			vars.push_back(newVar);//Adds variable
			c->variables.push_back(newVar);//Adds variable
		}
		}
		
		/*Unit creation*/{
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
		
		if (tokens[0] == "createPlayer" && tokens.size() >= 6){//Create unit controlled by player instruction
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
		
		if (tokens[0] == "createAI" && tokens.size() >= 6){//Create unit controlled by AI
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
		}
		
		/*Inventory handling*/{
		if (tokens[0] == "giveWeapon_primary" && tokens.size() >= 3){//Give primary weapon instruction
			unit* u = c->m->getUnit_name(tokens[1]);//Gets unit
			if (u) u->giveWeapon_primary(tokens[2]);//Gives weapon to unit
		}
		
		if (tokens[0] == "wear" && tokens.size() >= 3){//Gives clothing to unit
			unit* u = c->m->getUnit_name(tokens[1]);//Gets unit
			if (u) u->wear(tokens[2]);//Gives cloth to unit
		}
		
		if (tokens[0] == "giveItem" && tokens.size() >= 3){//Gives item to unit
			unit* u = c->m->getUnit_name(tokens[1]);//Gets unit
			if (u) u->giveItem(tokens[2]);//Gives item to unit
		}
		}
		
		/*Querying*/{
		if (tokens[0] == "query" && tokens.size() >= 3){
			string s = "";//Expression
			int i;//Counter
			for (i = 2; i < tokens.size(); i++) s += tokens[i] + " ";//Adds each token to string
			
			uQuery newQuery;//New query
			newQuery.id = tokens[1];//Sets query id
			
			deque<unit*>::iterator j;//Counter
			for (j = c->m->units.begin(); j < c->m->units.end(); j++){//For each unit in map
				deque<string> toks = tokenize(s, " ");//Expression tokens
				
				int k;//Counter
				for (k = 0; k < toks.size(); k++){//For each token
					if (toks[k][0] == '$'){//If token is a variable
						string vName = toks[k].substr(1);//Variable name
						
						if (vName == "id") toks[k] = (*j)->id;
						if (vName == "name") toks[k] = (*j)->name;
						
						if (vName == "x") toks[k] = toString((*j)->x);
						if (vName == "y") toks[k] = toString((*j)->y);
						
						if (vName == "hits") toks[k] = toString((*j)->hits());
						if (vName == "maxHits") toks[k] = toString((*j)->maxHits());
						if (vName == "mana") toks[k] = toString((*j)->mana());
						if (vName == "maxMana") toks[k] = toString((*j)->maxMana());
						if (vName == "strength") toks[k] = toString((*j)->strength());
						if (vName == "constitution") toks[k] = toString((*j)->constitution());
						if (vName == "intelligence") toks[k] = toString((*j)->intelligence());
						if (vName == "wisdom") toks[k] = toString((*j)->wisdom());
					}
				}
				
				string q = "";//Expression
				for (i = 0; i < toks.size(); i++) q += toks[i] + " ";//Adds token to expression
			
				if (atoi(expr(q, &ops_bool).c_str())) newQuery.units.push_back(*j);//Adds unit if matches expression
			}
			
			c->queries.push_back(newQuery);//Adds query to campaign
		}
		
		if (tokens[0] == "addToQuery" && tokens.size() >= 3){
			uQuery* q = get(&c->queries, tokens[1]);//Requested query
			unit* u = c->m->getUnit_name(tokens[2]);//Requested unit
			
			if (q && u) q->units.push_back(u);//Adds unit to query
		}
		}
		
		/*Units control*/{
		if (tokens[0] == "giveControl" && tokens.size() >= 2){//Give control to player instruction
			unit* u = c->m->getUnit_name(tokens[1]);//Gets unit
			if (u) c->player.addUnit(u);//Gives unit to player
		}
		}
		
		/*Effects on units*/{
		if (tokens[0] == "hits" && tokens.size() >= 3){//Vary hits instruction
			unit* u = c->m->getUnit_name(tokens[1]);//Requested unit
			if (u) u->varyHits(atoi(tokens[2].c_str()));//Varies hits
		}
		
		if (tokens[0] == "mana" && tokens.size() >= 3){//Vary mana instruction
			unit* u = c->m->getUnit_name(tokens[1]);//Requested unit
			if (u) u->varyMana(atoi(tokens[2].c_str()));//Varies mana
		}
		
		if (tokens[0] == "kill" && tokens.size() >= 2){//Kill instruction
			unit* u = c->m->getUnit_name(tokens[1]);//Requested unit
			if (u) u->kill();//Kills unit
		}
		}
		
		/*Story telling*/{
		if (tokens[0] == "dialog" && tokens.size() >= 2){//Dialog instruction
			c->dialogSeq.push_back(getText(tokens[1]));//Adds dialog
			
			if (tokens.size() >= 3) c->dialogActors.push_back(c->m->getUnit_name(tokens[2]));//Adds dialog actor if given
			else c->dialogActors.push_back(NULL);//Else adds null actor
			
			int n;//Counter
			for (n = 2; n < tokens.size(); n++){
				if (tokens[n] == "ans"){//Answers
					if (n + 1 < tokens.size()) c->dialogOutVar.push_back(tokens[n + 1]); else c->dialogOutVar.push_back("");
					if (n + 2 < tokens.size()) c->dialogAns[0].push_back(getText(tokens[n + 2])); else c->dialogAns[0].push_back("");
					if (n + 3 < tokens.size()) c->dialogAns[1].push_back(getText(tokens[n + 3])); else c->dialogAns[1].push_back("");
					if (n + 4 < tokens.size()) c->dialogAns[2].push_back(getText(tokens[n + 4])); else c->dialogAns[2].push_back("");
					if (n + 5 < tokens.size()) c->dialogAns[3].push_back(getText(tokens[n + 5])); else c->dialogAns[3].push_back("");
					break;
				}
			}
		}
		}
		
		/*Quests*/{
		if (tokens[0] == "questName" && tokens.size() >= 2){//Quest name command
			c->questName = getText(tokens[1]);
		}
		
		if (tokens[0] == "questInfo" && tokens.size() >= 2){//Quest info command
			c->questInfo = getText(tokens[1]);
		}
		}
		
		/*Flow control*/{
		if (tokens[0] == "if" && tokens.size() >= 2){//If statement
			string s = "";//Expression
			int j;//Counter
			for (j = 1; j < tokens.size(); j++) s += tokens[j] + " ";//Adds each token to string
			
			string result = expr(s, &ops_bool);//Calculates expression
			if (!atoi(result.c_str())) i++;//Skips next instruction if statement is false
		}
		}
		
		/*Misc*/{
		if (tokens[0] == "return" && tokens.size() >= 2){//Return statement
			if (tokens[1] == "true") return TRUE;
			else if (tokens[1] == "false") return FALSE;
			else return VOID;
		}
		
		if (tokens[0] == "endSequence"){//End sequence instruction
			c->nextSeq();//Moves to next sequence
		}
		}
	}
	
	return result;//Returns result
}

//Walking function
void unit::walk(int direction, bool turnOnly){
	if (parent){//If there's a reference map
		//Checks free tiles
		switch (direction){//According to direction
			case NORTH:
			if (GETDIR(action) != direction && turnOnly || !parent->isFree(x, y - 1, flying)){
				turn(NORTH);
				return;
			}
			break;
			
			case WEST:
			if (GETDIR(action) != direction && turnOnly || !parent->isFree(x - 1, y, flying)){
				turn(WEST);
				return;
			}
			break;
			
			case SOUTH:
			if (GETDIR(action) != direction && turnOnly || !parent->isFree(x, y + 1, flying)){
				turn(SOUTH);
				return;
			}
			break;
			
			case EAST:
			if (GETDIR(action) != direction && turnOnly || !parent->isFree(x + 1, y, flying)){
				turn(EAST);
				return;
			}
			break;
		}
	}

	moved = true;
	action = GETACODE(ACT_WALK, direction);//Sets action codes
	anims.setAnim("walk_" + dirToString(direction));//Sets walking animation
	if (head) head->overlay.setAnim("walk_" + dirToString(direction));//Sets cloths animation
	if (body) body->overlay.setAnim("walk_" + dirToString(direction));//Sets cloths animation
	if (legs) legs->overlay.setAnim("walk_" + dirToString(direction));//Sets cloths animation
}

//Function to go to next frame
void unit::nextFrame(){
	anims.next();//Next animation frame
	
	//Next frame for clothes
	if (head) head->overlay.next();
	if (body) body->overlay.next();
	if (legs) legs->overlay.next();
	
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
		if (primary->imgGiven) primary->overlay.next();//Next frame for weapon animation
		
		if (anims.current()->curFrame == anims.current()->duration() - 1) stop();//Stops if reached end of animation
		
		if (anims.current()->curFrame == primary->strikeFrame && parent){//At half striking animation
			applyEffect(primary->onStrike);//Applies effect
			
			switch (GETDIR(action)){//According to direction
				case NORTH: parent->applyEffect(x, y - 1, primary->getEff(this), GETDIR(action)); break;
				case WEST: parent->applyEffect(x - 1, y, primary->getEff(this), GETDIR(action)); break;
				case SOUTH: parent->applyEffect(x, y + 1, primary->getEff(this), GETDIR(action)); break;
				case EAST: parent->applyEffect(x + 1, y, primary->getEff(this), GETDIR(action)); break;
			}
		}
	}
	
	else if (GETACT(action) == ACT_SPELL && spell){//If unit is casting spell
		if (spell->imgGiven) spell->overlay.next();//Next frame for spell animation
		
		if (anims.current()->curFrame == anims.current()->duration() - 1) stop();//Stops if reached end of animation
		
		if (anims.current()->curFrame == spell->strikeFrame && parent){//At half striking animation
			applyEffect(spell->onStrike);//Applies effect
			
			switch (GETDIR(action)){//According to direction
				case NORTH: parent->applyEffect(x, y - 1, spell->getEff(this), GETDIR(action)); break;
				case WEST: parent->applyEffect(x - 1, y, spell->getEff(this), GETDIR(action)); break;
				case SOUTH: parent->applyEffect(x, y + 1, spell->getEff(this), GETDIR(action)); break;
				case EAST: parent->applyEffect(x + 1, y, spell->getEff(this), GETDIR(action)); break;
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
void unit::AI(bool turnOnly){
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
		if (closest->y < y && parent->isFree(x, y - 1)) walk(NORTH, turnOnly);
		else if (closest->x < x && parent->isFree(x - 1, y)) walk(WEST, turnOnly);
		else if (closest->y > y && parent->isFree(x, y + 1)) walk(SOUTH, turnOnly);
		else if (closest->x > x && parent->isFree(x + 1, y)) walk(EAST, turnOnly);
	}
}

//Controller AI function
bool controller::AI(){
	if (units.size() == 0) return true;//Returns true if there are no units
	
	begin:
	if ((AI_current >= 0 || units[AI_current - 1]->ready() || units[AI_current - 1]->action == ACT_DEAD)/* && (!units[AI_current]->parent || units[AI_current]->parent->projectiles())*/){//If current unit is the first or the previous is ready or dead
		if (AI_current == units.size()){//If that was last unit
			AI_current = 0;//Goes back to first unit
			return true;//Returns true
		}
		
		else {//Else
			units[AI_current]->AI(false);//Execs current unit's AI function
			AI_current++;//Next unit
			
			if (units[AI_current - 1]->ready() || units[AI_current - 1]->action == ACT_DEAD) goto begin;//Restarts from beginning if unit did nothing
			return false;//Returns false
		}
	}
}

//Function to set campaign
void setCampaign(string id){
	if (get(&campaignDb, "tutorial")){
		current = *get(&campaignDb, "tutorial");
		current.setup();
	}
}

//New game click event handler
void btn_newGame_click(control* p, mouseEvent ev){
	gamePhase = GAME_PHASE;
}

//Quit click event handler
void btn_quitMenu_click(control* p, mouseEvent ev){
	running = false;
}

//Function to handle click on dialog answers
void btn_ans_click(control* p, mouseEvent ev){
	var newVar (current.dialogOutVar[0], p->id);//Answer variable
	current.variables.push_back(newVar);//Adds variable to campaign
	cout << newVar.name << " set to " << newVar.value << endl;
	current.nextDialog();//Next dialog
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
		
		if (i->type == OBJTYPE_DAMTYPE){//If object is a damage type
			damType newDT;//New type
			newDT.fromScriptObj(*i);//Loads type
			damTypeDb.push_back(newDT);//Adds type to database
		}
		
		if (i->type == OBJTYPE_WEAPON || i->type == OBJTYPE_SPELL){//If object is a weapon
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
		
		if (i->type == OBJTYPE_PROJECTILE){//If object is a projectile
			projectile newProj;//New projectile
			newProj.fromScriptObj(*i);//Loads projectile
			projectileDb.push_back(newProj);//Adds projectile
		}
		
		if (i->type == OBJTYPE_ITEM){//If object is an item
			disposable newItem;//New item
			newItem.fromScriptObj(*i);//Loads item
			itemDb.push_back(newItem);//Adds item to database
		}
		
		if (i->type == OBJTYPE_LOCALE){//If object is a localization
			loc newLoc;//New locale
			newLoc.fromScriptObj(*i);//Loads locale
			
			if (!get(&locales, i->name))//If locale doesn't exist yet
				locales.push_back(newLoc);//Adds locale to database
			else *get(&locales, i->name) += newLoc;//Else adds locale to previous
		}
	}
	
	cout << "Loaded successfully!" << endl;
	cout << terrainDb.size() << " terrains\n" << mapDb.size() << " maps\n" << unitDb.size() << " units\n" << weaponDb.size() << " weapons\n" << clothDb.size() << " cloths\n" << decoDb.size() << " decos\n" << campaignDb.size() << " campaigns\n";
}

//Initialization function
void game_init(string dbFile, string settingsFile, string themeFile){
	/*Basic initialization*/{
	srand(time(NULL));//Randomize
		
	uiInit();//Initializes UI
	image_init();//Initializes image library
		
	initWindow(1000, 600, "Tales of Gydia");//Window setup
	}
	
	/*Loads game settings*/{
	fileData s (settingsFile);//Settings file
	object settings = s.objGen("settings");//Loads settings
	
	if (settings.getVar("locale")) curLoc = settings.getVar("locale")->value;//Gets locale variable
	}
	
	/*Loads game database*/{
	fileData f (dbFile);//File data for database
	loadDatabase(f.objGen("db"));//Loads database
	}
	
	/*Loads game interface data*/{
	fileData t (themeFile);//Theme file
	object theme = t.objGen("theme");//Loads theme
	
	if (theme.getVar("font_global")) globalFont = TTF_OpenFont(theme.getVar("font_global")->value.c_str(), 12);//Opens global font
	if (theme.getVar("font_panels_major")) panelFont_major = TTF_OpenFont(theme.getVar("font_panels_major")->value.c_str(), 18);//Opens panel major font
	if (theme.getVar("font_panels_minor")) panelFont_minor = TTF_OpenFont(theme.getVar("font_panels_minor")->value.c_str(), 14);//Opens panel minor font
	if (theme.getVar("font_dialogs")) dialogFont = TTF_OpenFont(theme.getVar("font_dialogs")->value.c_str(), 14);//Opens dialog font
	
	if (theme.getVar("bar_frame")) bar_frame = CACHEDIMG(theme.getVar("bar_frame")->value);//Gets bar frame
	if (theme.getVar("bar_frame_offset_x")) bar_frame_offset.x = theme.getVar("bar_frame_offset_x")->intValue();//Gets bar frame offset x
	if (theme.getVar("bar_frame_offset_y")) bar_frame_offset.y = theme.getVar("bar_frame_offset_y")->intValue();//Gets bar frame offset y
	
	if (theme.getVar("bar_xp_frame")) bar_xp_frame = CACHEDIMG(theme.getVar("bar_xp_frame")->value);//Gets xp bar frame
	if (theme.getVar("bar_xp_frame_offset_x")) bar_xp_frame_offset.x = theme.getVar("bar_xp_frame_offset_x")->intValue();//Gets xp bar frame offset x
	if (theme.getVar("bar_xp_frame_offset_y")) bar_xp_frame_offset.y = theme.getVar("bar_xp_frame_offset_y")->intValue();//Gets xp bar frame offset y
	
	if (theme.getVar("bar_hits")) bar_hits = CACHEDIMG(theme.getVar("bar_hits")->value);//Gets hitbar fill
	if (theme.getVar("bar_hits_offset_x")) bar_hits_offset.x = theme.getVar("bar_hits_offset_x")->intValue();//Gets hitbar offset x
	if (theme.getVar("bar_hits_offset_y")) bar_hits_offset.y = theme.getVar("bar_hits_offset_y")->intValue();//Gets hitbar offset y
	
	if (theme.getVar("bar_mana")) bar_mana = CACHEDIMG(theme.getVar("bar_mana")->value);//Gets manabar fill
	if (theme.getVar("bar_mana_offset_x")) bar_mana_offset.x = theme.getVar("bar_mana_offset_x")->intValue();//Gets manabar offset x
	if (theme.getVar("bar_mana_offset_y")) bar_mana_offset.y = theme.getVar("bar_mana_offset_y")->intValue();//Gets manabar offset y
	
	if (theme.getVar("bar_xp")) bar_xp = CACHEDIMG(theme.getVar("bar_xp")->value);//Gets xp bar fill
	if (theme.getVar("bar_xp_offset_x")) bar_xp_offset.x = theme.getVar("bar_xp_offset_x")->intValue();//Gets xp bar offset x
	if (theme.getVar("bar_xp_offset_y")) bar_xp_offset.y = theme.getVar("bar_xp_offset_y")->intValue();//Gets xp bar offset y
	
	if (theme.getVar("infoPanel_col1")) infoPanel_col1 = strtol(theme.getVar("infoPanel_col1")->value.c_str(), NULL, 0);//Loads panel first color (name box)
	if (theme.getVar("infoPanel_col2")) infoPanel_col2 = strtol(theme.getVar("infoPanel_col2")->value.c_str(), NULL, 0);//Loads panel second color (stats boxes)
	if (theme.getVar("infoPanel_col3")) infoPanel_col3 = strtol(theme.getVar("infoPanel_col3")->value.c_str(), NULL, 0);//Loads panel third color (bonuses)
	if (theme.getVar("infoPanel_col4")) infoPanel_col4 = strtol(theme.getVar("infoPanel_col4")->value.c_str(), NULL, 0);//Loads panel third color (maluses)
	
	if (theme.getVar("dialogBox_img")) dialog_pict = CACHEDIMG(theme.getVar("dialogBox_img")->value);//Loads dialog box picture
	if (theme.getVar("dialogBox_col")) dialog_col = strtol(theme.getVar("dialogBox_col")->value.c_str(), NULL, 0);//Loads dialog color
	if (theme.getVar("dialogBox_actor_img")) dialog_actor_pict = CACHEDIMG(theme.getVar("dialogBox_actor_img")->value);//Loads actor dialog box picture
	if (theme.getVar("dialogBox_actor_imgX")) dialog_actor_x = theme.getVar("dialogBox_actor_imgX")->intValue();//Loads actor image offset
	if (theme.getVar("dialogBox_actor_imgY")) dialog_actor_y = theme.getVar("dialogBox_actor_imgY")->intValue();//Loads actor image offset
	if (theme.getVar("dialogBox_actor_captionY")) dialog_actor_nameY = theme.getVar("dialogBox_actor_captionY")->intValue();//Loads actor name offset
	
	if (theme.getVar("bigPanel_img")) bigPanel_pict = CACHEDIMG(theme.getVar("bigPanel_img")->value);//Loads big panel picture
	if (theme.getVar("slot_img")) slot_pict = CACHEDIMG(theme.getVar("slot_img")->value);//Loads weapon box picture
	if (theme.getVar("slot_focus_img")) slot_pict_focus = CACHEDIMG(theme.getVar("slot_focus_img")->value);//Loads weapon focused box picture
	
	if (theme.getVar("button_released")) button_released = CACHEDIMG(theme.getVar("button_released")->value);//Loads released button picture
	if (theme.getVar("button_pressed")) button_pressed = CACHEDIMG(theme.getVar("button_pressed")->value);//Loads pressed button picture
	if (theme.getVar("button_col")) buttons_col = strtol(theme.getVar("button_col")->value.c_str(), NULL, 0);//Loads panel color
	
	if (theme.getVar("button_big_released")) button_big_released = CACHEDIMG(theme.getVar("button_big_released")->value);//Loads released button picture
	if (theme.getVar("button_big_pressed")) button_big_pressed = CACHEDIMG(theme.getVar("button_big_pressed")->value);//Loads pressed button picture
	if (theme.getVar("button_big_col")) buttons_big_col = strtol(theme.getVar("button_big_col")->value.c_str(), NULL, 0);//Loads panel color
	
	if (theme.getVar("button_quit_released")) button_quit_released = CACHEDIMG(theme.getVar("button_quit_released")->value);//Loads released quit button picture
	if (theme.getVar("button_quit_pressed")) button_quit_pressed = CACHEDIMG(theme.getVar("button_quit_pressed")->value);//Loads pressed quit button picture
	
	if (theme.getVar("midPanel_img")) midPanel_pict = CACHEDIMG(theme.getVar("midPanel_img")->value);//Loads mid panel picture
	
	if (theme.getVar("title_img")) title_image = CACHEDIMG(theme.getVar("title_img")->value);//Loads title image
	}
	
	/*Main menu*/{
	btn_newGame.w = button_big_released->w;
	btn_newGame.h = button_big_released->h;
	btn_newGame.x = (window->w - btn_newGame.w) / 2;
	btn_newGame.y = (window->h - 4 * (btn_newGame.h + 5)) / 2;
	btn_newGame.foreColor = buttons_big_col;
	btn_newGame.font = panelFont_minor;
	btn_newGame.text = getText("newGame");
	btn_newGame.printMethod = bigButtonPrint;
	btn_newGame.setClickArea();
	
	btn_loadGame = btn_newGame;
	btn_loadGame.y += btn_loadGame.h + 5;
	btn_loadGame.text = getText("loadGame");
	
	btn_quitMenu = btn_loadGame;
	btn_quitMenu.y += 2 * (btn_quitMenu.h + 5);
	btn_quitMenu.text = getText("quit");
	
	btn_newGame.addHandler_click(btn_newGame_click);
	btn_quitMenu.addHandler_click(btn_quitMenu_click);
	}
	
	/*Player info panel*/{
	//Sets up info panel
	infoPanel.w = bigPanel_pict->w;
	infoPanel.h = bigPanel_pict->h;
	infoPanel.x = (window->w - infoPanel.w) / 2;
	infoPanel.y = (window->h - infoPanel.h) / 2;
	infoPanel.printMethod = bigPanelPrint;
	infoPanel.lockPosition = false;
	infoPanel.setClickArea();
	
	//Quit button
	btn_quit.w = button_quit_released->w;
	btn_quit.h = button_quit_released->h;
	btn_quit.x = infoPanel.w - btn_quit.w - 7;
	btn_quit.y = 7;
	btn_quit.printMethod = quitPrint;
	btn_quit.addHandler_click(quitButtonClick);
	infoPanel.controls.push_back(&btn_quit);
	
	//Name text box
	nameBox.w = dialog_actor_pict->w;
	nameBox.h = dialog_actor_pict->h;
	nameBox.x = 10;
	nameBox.y = 15;
	nameBox.printMethod = dialogPortraitPrint;
	infoPanel.controls.push_back(&nameBox);
	
	//Hp and mp text boxes
	hpBox.x = nameBox.x + nameBox.w + 5;
	hpBox.h = 20;
	hpBox.y = nameBox.y;
	hpBox.w = infoPanel.w / 2 - 7;
	hpBox.setAlpha(0);
	hpBox.font = panelFont_minor;
	hpBox.foreColor = infoPanel_col1;
	infoPanel.controls.push_back(&hpBox);
	
	mpBox = hpBox;
	mpBox.y += mpBox.h + 2;
	infoPanel.controls.push_back(&mpBox);
	
	xpBox = hpBox;
	xpBox.x = nameBox.x;
	xpBox.y = nameBox.y + nameBox.h + 5;
	infoPanel.controls.push_back(&xpBox);
	
	//Statistics text boxes
	strBox.x = 5;
	strBox.y = xpBox.y + xpBox.h + 10;
	strBox.w = infoPanel.w / 2 - 7;
	strBox.h = 15;
	strBox.setAlpha(0);
	strBox.foreColor = infoPanel_col2;
	strBox.font = panelFont_minor;
	infoPanel.controls.push_back(&strBox);
	
	intBox = strBox;
	intBox.x += 60;
	infoPanel.controls.push_back(&intBox);
	
	conBox = strBox;
	conBox.y += conBox.h + 5;
	infoPanel.controls.push_back(&conBox);
	
	wisBox = intBox;
	wisBox.y += wisBox.h + 5;
	infoPanel.controls.push_back(&wisBox);
	
	//Effects box
	effBox = conBox;
	effBox.y += effBox.h + 20;
	effBox.w = infoPanel.w - 40;
	infoPanel.controls.push_back(&effBox);
	
	//Resistance boxes
	list<damType>::iterator i;//Damage type iterator
	int n = 0;
	for (i = damTypeDb.begin(); i != damTypeDb.end(); i++){//For each damage type loaded
		textBox *t = new textBox;//New text box
		
		*t = conBox;
		t->id = i->id;
		t->x = nameBox.x + nameBox.w + 100;
		t->y = nameBox.y + 15 + n * (t->h + 1);
		t->text = i->shownName;
		
		infoPanel.controls.push_back(t);
		resBox.push_back(t);
		
		n++;
	}
	}
	
	/*Dialog box*/{
	dialogBox.x = window->w / 2 - dialog_pict->w / 2;
	dialogBox.y = window->h - dialog_pict->h;
	dialogBox.w = dialog_pict->w;
	dialogBox.h = dialog_pict->h;
	dialogBox.printMethod = dialogPrint;
	dialogBox.font = panelFont_minor;
	dialogBox.foreColor = infoPanel_col1;
	dialogBox.textAlign = CENTER;
	
	dialogBox_actor.x = dialogBox.x + 15;
	dialogBox_actor.y = dialogBox.y + (dialogBox.h - dialog_actor_pict->h) / 2;
	dialogBox_actor.w = dialog_actor_pict->w;
	dialogBox_actor.h = dialog_actor_pict->h;
	dialogBox_actor.printMethod = dialogPortraitPrint;
	
	int i;
	for (i = 0; i < 4; i++){
		btn_answers[i] = btn_newGame;
		btn_answers[i].removeHandler_click(btn_newGame_click);
		btn_answers[i].y = dialogBox.y + dialogBox.h - btn_answers[i].h - 5;
		btn_answers[i].id = toString(i);
		btn_answers[i].addHandler_click(btn_ans_click);
	}
	}
	
	/*Inventory screen*/{
		inventoryPanel.w = bigPanel_pict->w;
		inventoryPanel.h = bigPanel_pict->h;
		inventoryPanel.x = (window->w - inventoryPanel.w) / 2;
		inventoryPanel.y = (window->h - inventoryPanel.h) / 2;
		inventoryPanel.printMethod = bigPanelPrint;
		inventoryPanel.setClickArea();
		
		inventoryPanel.controls.push_back(&btn_quit);
		
		btn_use.x = 10 + 4 * slot_pict->w + (bigPanel_pict->w - 10 - 4 * slot_pict->w - button_released->w) / 2;
		btn_use.y = 10 + 3 * slot_pict->h;
		btn_use.w = button_released->w;
		btn_use.h = button_released->h;
		btn_use.setClickArea();
		btn_use.font = panelFont_minor;
		btn_use.foreColor = buttons_col;
		btn_use.text = getText("use");
		btn_use.printMethod = buttonPrint;
		inventoryPanel.controls.push_back(&btn_use);
		
		btn_drop = btn_use;
		btn_drop.y += btn_drop.h;
		btn_drop.text = getText("drop");
		inventoryPanel.controls.push_back(&btn_drop);
		
		btn_use.addHandler_click(btn_use_click);
		btn_drop.addHandler_click(btn_drop_click);
		
		int i;
		for (i = 0; i < 12; i++){
			inv_slots[i].x = (i % 4) * slot_pict->w + 10;
			inv_slots[i].y = floor(i / 4) * slot_pict->h + 10;
			inv_slots[i].w = slot_pict->w;
			inv_slots[i].h = slot_pict->h;
			inv_slots[i].printMethod = slotPrint;
			inv_slots[i].id = toString(i);
			inv_slots[i].addHandler_getFocus(slot_getFocus);
			inv_slots[i].addHandler_loseFocus(slot_loseFocus);
			inventoryPanel.controls.push_back(&inv_slots[i]);
		}
		
		headBox.x = 10 + 4 * slot_pict->w + (bigPanel_pict->w - 10 - 4 * slot_pict->w) / 2 - slot_pict->w;
		headBox.y = 10;
		headBox.w = slot_pict->w;
		headBox.h = slot_pict->h;
		headBox.id = "12";
		headBox.printMethod = slotPrint;
		headBox.addHandler_getFocus(slot_getFocus);
		headBox.addHandler_loseFocus(slot_loseFocus);
		inventoryPanel.controls.push_back(&headBox);
		
		bodyBox = headBox;
		bodyBox.y += slot_pict->h;
		bodyBox.id = "13";
		bodyBox.addHandler_getFocus(slot_getFocus);
		bodyBox.addHandler_loseFocus(slot_loseFocus);
		inventoryPanel.controls.push_back(&bodyBox);
		
		legsBox = bodyBox;
		legsBox.y += slot_pict->h;
		legsBox.id = "14";
		legsBox.addHandler_getFocus(slot_getFocus);
		legsBox.addHandler_loseFocus(slot_loseFocus);
		inventoryPanel.controls.push_back(&legsBox);
		
		weaponBox = bodyBox;
		weaponBox.x += slot_pict->w;
		weaponBox.y -= slot_pict->h / 2;
		weaponBox.id = "15";
		inventoryPanel.controls.push_back(&weaponBox);
		
		spellBox = weaponBox;
		spellBox.y += slot_pict->h;
		spellBox.id = "16";
		inventoryPanel.controls.push_back(&spellBox);
		
		itemInfoBox.x = 10;
		itemInfoBox.y = btn_use.y;
		itemInfoBox.w = btn_use.x - 20;
		itemInfoBox.h = inventoryPanel.h - itemInfoBox.y - 20;
		itemInfoBox.setAlpha(0);
		itemInfoBox.font = panelFont_minor;
		itemInfoBox.foreColor = infoPanel_col1;
		inventoryPanel.controls.push_back(&itemInfoBox);
	}
	
	/*First level up panel*/{
	hp_mp_levelUp.w = midPanel_pict->w;
	hp_mp_levelUp.h = midPanel_pict->h;
	hp_mp_levelUp.x = (window->w - hp_mp_levelUp.w) / 2;
	hp_mp_levelUp.y = (window->h - hp_mp_levelUp.h) / 2;
	hp_mp_levelUp.printMethod = midPanelPrint;
	hp_mp_levelUp.setClickArea();
	
	hpUp = btn_use;
	hpUp.text = getText("hits") + " +5";
	hpUp.x = (hp_mp_levelUp.w - hpUp.w) / 2;
	hpUp.y = hp_mp_levelUp.h / 2 - hpUp.h;
	hp_mp_levelUp.controls.push_back(&hpUp);
	
	mpUp = hpUp;
	mpUp.text = getText("mana") + " +5";
	mpUp.y += mpUp.h;
	hp_mp_levelUp.controls.push_back(&mpUp);
	
	hpUp.addHandler_click(hpUp_click);
	mpUp.addHandler_click(mpUp_click);
	
	levelUp_caption.w = hp_mp_levelUp.w;
	levelUp_caption.h = hp_mp_levelUp.h;
	levelUp_caption.x = (hp_mp_levelUp.w - levelUp_caption.w) / 2;
	levelUp_caption.y = 10;
	levelUp_caption.text = getText("levelUp");
	levelUp_caption.foreColor = infoPanel_col1;
	levelUp_caption.font = panelFont_major;
	levelUp_caption.setAlpha(0);
	levelUp_caption.textAlign = CENTER;
	hp_mp_levelUp.controls.push_back(&levelUp_caption);
	}
	
	/*Second level up panel*/{
	stats_levelUp = hp_mp_levelUp;
	stats_levelUp.controls.clear();
	
	strUp = hpUp;
	strUp.x -= strUp.w / 2;
	strUp.text = getText("str_abbr") + " +1";
	stats_levelUp.controls.push_back(&strUp);
	
	conUp = strUp;
	conUp.y += conUp.h;
	conUp.text = getText("con_abbr") + " +1";
	stats_levelUp.controls.push_back(&conUp);
	
	intUp = strUp;
	intUp.x += intUp.w;
	intUp.text = getText("int_abbr") + " +1";
	stats_levelUp.controls.push_back(&intUp);
	
	wisUp = intUp;
	wisUp.y += wisUp.h;
	wisUp.text = getText("wis_abbr") + " +1";
	stats_levelUp.controls.push_back(&wisUp);
	
	strUp.addHandler_click(strUp_click);
	conUp.addHandler_click(conUp_click);
	intUp.addHandler_click(intUp_click);
	wisUp.addHandler_click(wisUp_click);
	
	stats_levelUp.controls.push_back(&levelUp_caption);
	}
	
	/*Quest info panel*/{
	questPanel.w = bigPanel_pict->w;
	questPanel.h = bigPanel_pict->h;
	questPanel.x = (window->w - questPanel.w) / 2;
	questPanel.y = (window->h - questPanel.h) / 2;
	questPanel.printMethod = bigPanelPrint;
	questPanel.setClickArea();
	
	questNameBox.x = 5;
	questNameBox.y = 5;
	questNameBox.w = questPanel.w;
	questNameBox.h = 30;
	questNameBox.font = panelFont_major;
	questNameBox.foreColor = infoPanel_col1;
	questNameBox.setAlpha(0);
	questNameBox.textAlign = CENTER;
	questPanel.controls.push_back(&questNameBox);
	
	questInfoBox = questNameBox;
	questInfoBox.x += 5;
	questInfoBox.y += questInfoBox.h;
	questInfoBox.w -= 10;
	questInfoBox.h = questPanel.h - questInfoBox.y - 5;
	questInfoBox.font = panelFont_minor;
	questInfoBox.textAlign = LEFT;
	questPanel.controls.push_back(&questInfoBox);
	
	questPanel.controls.push_back(&btn_quit);
	}
	
	/*Exchange panel*/{
	exchangePanel.w = bigPanel_pict->w;
	exchangePanel.h = bigPanel_pict->h;
	exchangePanel.x = (window->w - questPanel.w) / 2;
	exchangePanel.y = (window->h - questPanel.h) / 2;
	exchangePanel.printMethod = bigPanelPrint;
	exchangePanel.setClickArea();
	
	int i;
	for (i = 0; i < 12; i++){
		exchangePanel.controls.push_back(&inv_slots[i]);
		
		exch_slots[i] = inv_slots[i];
		exch_slots[i].id = toString(atoi(inv_slots[i].id.c_str()) * 2);
		exch_slots[i].x += exchangePanel.w - 20 - 4 * exch_slots[i].w;
		exchangePanel.controls.push_back(&exch_slots[i]);
	}
	}
	
	/*Sets operators*/{
	op bool_equal ("=", 1, ops_bool_equal);
	op bool_greater (">", 1, ops_bool_greater);
	op bool_less ("<", 1, ops_bool_less);
	op bool_and ("&", 0, ops_bool_and);
	op bool_or ("|", 0, ops_bool_or);
	op int_sum ("+", 0, ops_int_sum);
	op int_subt ("-", 0, ops_int_subt);
	op int_mult ("*", 1, ops_int_mult);
	op int_div ("/", 1, ops_int_div);
	op str_concat ("+", 0, ops_str_concat);
	
	ops_bool.push_back(bool_equal);
	ops_bool.push_back(bool_greater);
	ops_bool.push_back(bool_less);
	ops_bool.push_back(bool_and);
	ops_bool.push_back(bool_or);
	ops_int.push_back(int_sum);
	ops_int.push_back(int_subt);
	ops_int.push_back(int_mult);
	ops_int.push_back(int_div);
	ops_str.push_back(str_concat);
	}
}

#endif