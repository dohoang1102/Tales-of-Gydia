//User-interface handling

#ifndef _UI
#define _UI

#include <string>//Include strings
#include <list>//Include lists
#include <deque>//Include deques
#include <cmath>//Include C math

#include <SDL/SDL.h>//Include base SDL library
#include <SDL/SDL_ttf.h>//Include SDL ttf library

#include <SDL/SDL_rotozoom.h>//Include SDL rotozoom library (required by following header)
#include <SDL/SDL_gfxPrimitives.h>//Include SDL gfx library

#define CLICKTIME				250//Milliseconds to distinguish click and drag
#define GRABTOLERANCE			0//Tolerance to drag panels

SDL_Surface* window = NULL;//Surface for the current window

#ifdef _ERROR//If error.h has been included
	int UI_ERROR 						= getErrorCode();//Code for generic UI errors
	int UI_INITERROR					= getErrorCode();//Code for initialization errors
	int UI_WININITERROR					= getErrorCode();//Code for window initialization error
	int UI_INVALIDEVTYPEERROR			= getErrorCode();//Code for invalid error type
	int UI_WARNING						= getErrorCode();//Code for generic warning
	int UI_NOPRINTMETHODWARNING			= getErrorCode();//Code for no printing method warning
#endif

#define PLAINEV(NAME)			void (*NAME)(control*)//Control plain (no args) event function macro
#define MOUSEEV(NAME)			void (*NAME)(control*, mouseEvent)//Control mouse event function macro
#define MOVEEV(NAME) 			void (*NAME)(control*, moveEvent)//Control move event function macro
#define TYPEEV(NAME)			void (*NAME)(textBox*, typeEvent)//Text box text type event function macro

#define RENDERTEXT				TTF_RenderText_Blended//Text rendering function

#define CONTROL					0//Control base type
#define PANEL					1//Control type panel
#define BUTTON					2//Control type button
#define IMGBOX					3//Control type image box
#define TEXTBOX					4//Control type text box
#define IMAGEBOX				5//Control type image box
#define FILLBAR					6//Control type fillbar

class control;//Control class prototype
class panel;//Panel class prototype

control* moveControl = NULL;//Control being currently moved

typedef struct {int x; int y; int button;} mouseEvent;//Mouse event data structure
typedef struct {int oldX; int oldY; int newX; int newY; int dX; int dY;} moveEvent;//Move event data structure
typedef struct {string oldText; string newText;} typeEvent;//Text type event data structure

typedef enum {VISIBLE, MINIMIZED, HIDDEN, PRESSED} cStatus;//Panel status enumeration
typedef enum {CENTERED, STRETCHED, TOPLEFT} imgStyle;//Image style enumeration
typedef enum {CENTER, LEFT, RIGHT} alignment;//Alignment enumeration

//Initialization function
int uiInit(){
	int sdl = SDL_Init(SDL_INIT_EVERYTHING);//Initializes SDL
	int sdl_ttf = TTF_Init();//Initializes SDL_ttf
		
	if (sdl != 0){//If there was an error initializing SDL
		#ifdef _ERROR
			errFunc(UI_INITERROR, "Error initializing SDL");
		#endif
		return 1;
	}
	
	if (sdl_ttf != 0){//If there was an error initializing SDL_ttf
		#ifdef _ERROR
			errFunc(UI_INITERROR, "Error initializing SDL_ttf");
		#endif
		return 2;
	}
	
	SDL_EnableUNICODE(SDL_ENABLE);//Enables unicode
	
	return 0;//Initialization successfult
}

//Window initialization function
int initWindow(int w, int h, const char* caption){
	window = SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE);//Initializes window
	SDL_WM_SetCaption(caption, NULL);//Sets caption
	
	if (!window){//If initialization failed
		#ifdef _ERROR
			if (errFunc) errFunc(UI_WININITERROR, "Error initializing window");
		#endif
		return 1;
	}
	
	return 0;//Initialization successful
}

void print_default(SDL_Surface* target, control* p);//Default panel printing function prototype

//Control class
//Includes click and move events
class control{
	protected:
	//Events
	list <MOUSEEV()> clickEvents;//Pointer to on-click functions
	list <MOVEEV()> moveEvents;//Motion events
	list <PLAINEV()> getFocusEvents;//Get focus events
	list <PLAINEV()> loseFocusEvents;//Lose focus events
	
	bool focus;//Control active
	
	//Dragging
	int pressTime;//Time when the mouse was pressed (to distinguish click and drag)
	int dX, dY;//Temporary x and y variations while dragging
	
	//Appearance
	Uint32 backColor;//Background color of the control
	cStatus status;//Status of the control
	
	public:
	int type;//Control type
	string id;//Control identifier
	
	bool lockPosition;//Flag indicating if the control can be moved
	
	int x, y, w, h;//Coordinates and size of the control
	int clickX, clickY, clickW, clickH;//Click area coordinates
	int dragX, dragY, dragW, dragH;//Dragging coordinates
	
	void (*printMethod)(SDL_Surface*, control*);//Print function of the control
	
	//Constructor
	control(){
		type = CONTROL;
		
		focus = false;
		lockPosition = true;
		
		id = "";
				
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		clickX = 0;
		clickY = 0;
		clickW = 0;
		clickH = 0;
		dragX = 0;
		dragY = 0;
		dragW = 0;
		dragH = 0;
		
		status = VISIBLE;
		
		backColor = 0x7F7F7FFF;
		printMethod = &print_default;
	}
	
	//Function to get control status
	cStatus getStatus(){
		return status;
	}
	
	//Function to show control
	void show(){
		status = VISIBLE;
	}
	
	//Function to hide control
	void hide(){
		status = HIDDEN;
	}
	
	//Function to toggle visible
	void toggleVisible(){
		if (status == VISIBLE) hide();
		else if (status == HIDDEN) show();
	}
	
	//Function to print the control
	void print(SDL_Surface* target){
		if (printMethod) printMethod(target, this);//Prints control
		
		#ifdef _ERROR
			else if (errFunc) errFunc(UI_NOPRINTMETHODWARNING, "No printing method for control " + id);
		#endif
	}
	
	//Function to set control background color without changing alpha
	void setBackColor(Uint32 newColor){
		Uint8 alpha = backColor & 0x000000FF;//Alpha channel
		backColor = (newColor << 8) + alpha;//Sets back color
	}
	
	//Function to set control alpha without changing color
	void setAlpha(Uint8 alpha){
		Uint32 color = backColor & 0xFFFFFF00;//RGB color
		backColor = color + alpha;//Sets back color
	}
	
	//Function to get control color (RGBA format)
	Uint32 getBackColor(){
		return backColor;//Returns color
	}
	
	//Function to add a handler for click events
	//Function added twice or more will be executed that many times
	//Functions will be executed in their insertion order
	void addHandler_click(MOUSEEV(handler)){
		if (handler) clickEvents.push_back(handler);//Adds handler to handlers list
	}
	
	//Function to remove a handler from click events
	//If more handlers pointing to the same function are available, the function will remove the first added
	void removeHandler_click(MOUSEEV(handler)){
		list <MOUSEEV()> :: iterator i;//Iterator
		
		for (i = clickEvents.begin(); i != clickEvents.end(); i++){//For each item of the event handlers list
			if (*i == handler){//If the handler is the same
				i = clickEvents.erase(i);//Erases the handler
				return;//Exit function
			}
		}
	}
	
	//Function to add a handler for motion events
	void addHandler_move(MOVEEV(handler)){
		if (handler) moveEvents.push_back(handler);
	}
	
	//Function to remove a handler from motion events
	void removeHandler_move(MOVEEV(handler)){
		list <MOVEEV()> :: iterator i;//Iterator
		
		for (i = moveEvents.begin(); i != moveEvents.end(); i++){//For each item of the event handlers list
			if (*i == handler){//If the handler is the same
				i = moveEvents.erase(i);//Erases the handler
				return;//Exit function
			}
		}
	}
	
	//Function to add a handler for get focus events
	void addHandler_getFocus(PLAINEV(handler)){
		if (handler) getFocusEvents.push_back(handler);
	}
	
	//Function to remove a handler from get focus events
	void removeHandler_getFocus(PLAINEV(handler)){
		list <PLAINEV()> :: iterator i;//Iterator
		
		for (i = getFocusEvents.begin(); i != getFocusEvents.end(); i++){//For each item of the event handlers list
			if (*i == handler){//If the handler is the same
				i = getFocusEvents.erase(i);//Erases the handler
				return;//Exit function
			}
		}
	}
	
	//Function to add a handler for lose focus events
	void addHandler_loseFocus(PLAINEV(handler)){
		if (handler) loseFocusEvents.push_back(handler);
	}
	
	//Function to remove a handler from lose focus events
	void removeHandler_loseFocus(PLAINEV(handler)){
		list <PLAINEV()> :: iterator i;//Iterator
		
		for (i = loseFocusEvents.begin(); i != loseFocusEvents.end(); i++){//For each item of the event handlers list
			if (*i == handler){//If the handler is the same
				i = loseFocusEvents.erase(i);//Erases the handler
				return;//Exit function
			}
		}
	}
	
	//Function to call all click event handlers
	void click(int x, int y, int button){
		list<MOUSEEV()> :: iterator i;//Iterator
		mouseEvent data {x, y, button};//Event data
		
		if (x < 0 || y < 0 || x > clickW || y > clickH) return;//Exit function is mouse is out of the click area
		
		for (i = clickEvents.begin(); i != clickEvents.end(); i++){//For each click event handler
			(*i)(this, data);//Calls event function
		}
	}
	
	//Function to call all get focus event handlers
	void getFocus(){
		focus = true;//Gives focus
		
		list<PLAINEV()> :: iterator i;//Iterator
		
		for (i = getFocusEvents.begin(); i != getFocusEvents.end(); i++){//For each click event handler
			(*i)(this);//Calls event function
		}
	}
	
	//Function to call all lose focus event handlers
	void loseFocus(){
		focus = false;//Gives focus
		
		list<PLAINEV()> :: iterator i;//Iterator
		
		for (i = loseFocusEvents.begin(); i != loseFocusEvents.end(); i++){//For each click event handler
			(*i)(this);//Calls event function
		}
	}
	
	//Function to move the panel and call all move event handlers
	void move(int dX, int dY){
		if (dX == 0 && dY == 0) return;//Exit function if there's no motion
		
		//Moves the panel
		x += dX;
		y += dY;
		
		list<MOVEEV()> :: iterator i;//Iterator
		moveEvent data {x - dX, y - dY, x, y, dX, dY};//Event data structure
		
		for (i = moveEvents.begin(); i != moveEvents.end(); i++){//For each move event handler
			(*i)(this, data);//Calls event function
		}
	}
	
	//Function to check the events on the panel
	void checkEvents(SDL_Event e){
		if (status == HIDDEN) return;//Exits the function if the panel is hidden
		
		if (e.type == SDL_MOUSEBUTTONDOWN){//If mouse is pressed
			pressTime = SDL_GetTicks();//Sets press time
			
			dX = 0;
			dY = 0;
			
			if (e.button.x > x + dragX && e.button.x < x + dragX + dragW && e.button.y > y + dragY && e.button.y < y + dragY + dragH){//If mouse is inside dragging area of control
				moveControl = this;//Sets panel being moved
			}
		}
		
		else if (e.type == SDL_MOUSEBUTTONUP){//If mouse has been released
			if (SDL_GetTicks() - pressTime < CLICKTIME){
				if (e.button.button == SDL_BUTTON_LEFT && e.button.x - x > 0 && e.button.x - x < clickW && e.button.y - y > 0 && e.button.y - y < clickH)//If left clicked inside
					getFocus();//Gives focus
				else//Else
					loseFocus();//Removes focus
								
				click(e.button.x - x, e.button.y - y, e.button.button);//Checks click events
			}
							
			if (moveControl == this) moveControl = NULL;
		}
		
		else if (e.type == SDL_MOUSEMOTION && !lockPosition && moveControl == this){//If dragging
			if (abs(dX) > GRABTOLERANCE || abs(dY) > GRABTOLERANCE){//If motion is greater than tolerance
				move(e.motion.xrel, e.motion.yrel);//Moves control
			}
			
			else {//Else
				//Adds current motion
				dX += e.motion.xrel;
				dY += e.motion.yrel;
			}
		}
	}
	
	//Function to get focus
	bool isFocused(){
		return focus;
	}
	
	//Function to set click area
	void setClickArea(){
		clickX = x;
		clickY = y;
		clickW = w;
		clickH = h;
	}
};

//Button class
class button: public control{
	private:
	Uint32 backColor_pressed;//Color when pressed
	
	public:
	string text;//Button text
	TTF_Font* font;//Button font
	Uint32 foreColor;//Foreground color
	
	//Constructor
	button(){
		type = BUTTON;
		
		lockPosition = true;
		
		id = "";
		text = "";
		
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		dragX = 0;
		dragY = 0;
		dragW = 0;
		dragH = 0;
		
		status = VISIBLE;
		
		font = NULL;
		foreColor = 0x000000;
		backColor = 0x7F7F7FFF;
		printMethod = &print_default;
	}
	
	//Function to get foreground color (SDL_Color format)
	SDL_Color getForeColor(){
		return SDL_Color {(foreColor & 0xFF0000) >> 16, (foreColor & 0x00FF00) >> 8, foreColor & 0x0000FF};
	}
	
	//Overloading function to check events
	void checkEvents(SDL_Event e){
		if (e.type == SDL_MOUSEBUTTONDOWN && e.button.x > x && e.button.x < x + w && e.button.y > y && e.button.y < y + h)//If mouse is pressed inside button
			status = PRESSED;
			
		else if (e.type == SDL_MOUSEBUTTONUP) status = VISIBLE;
		
		control::checkEvents(e);//Checks base control events
	}
	
	//Function to set back color when pressed (only RGB)
	void setPressedColor(Uint32 color){
		backColor_pressed = (color << 8) + (backColor_pressed & 0x000000FF);//Sets new color
	}
	
	//Function to set alpha when pressed
	void setPressedAlpha(Uint8 alpha){
		backColor_pressed = (backColor_pressed & 0xFFFFFF00) + alpha;//Sets new color
	}
	
	//Function to get back color when pressed
	Uint32 getBackColor_pressed(){
		return backColor_pressed;
	}
};

//Image box class
class imgBox: public control{
	public:
	SDL_Surface* img;//Image to be shown in the box
	imgStyle style;//Image style
	
	//Constructor
	imgBox(){
		type = IMGBOX;
		style = TOPLEFT;
		img = NULL;
		
		lockPosition = true;
		
		id = "";
		
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		dragX = 0;
		dragY = 0;
		dragW = 0;
		dragH = 0;
		
		status = VISIBLE;
		
		backColor = 0x7F7F7FFF;
		printMethod = &print_default;
	}
};

#ifdef _IMAGE
	//Image.h image box class
	class imageBox: public control{
		public:
		image* i;//Image to be shown
		
		//Constructor
		imageBox(){
			type = IMAGEBOX;
			i = NULL;
			
			lockPosition = true;
			
			id = "";
			
			x = 0;
			y = 0;
			w = 0;
			h = 0;
			dragX = 0;
			dragY = 0;
			dragW = 0;
			dragH = 0;
			
			status = VISIBLE;
			
			backColor = 0x7F7F7FFF;
			printMethod = &print_default;
		}
	};
#endif

//Text box class
class textBox: public control{
	private:
	list <TYPEEV()> typeEvents;//Tipe event handlers
	
	public:
	string text;//Text content
	TTF_Font* font;//Text box font
	Uint32 foreColor;//Text color
	bool edit;//Enable text editing
	alignment textAlign;//Text alignment
	
	//Constructor
	textBox(){
		type = TEXTBOX;
		text = "";
		edit = false;
		font = NULL;
		foreColor = 0x000000;
		textAlign = LEFT;
		
		lockPosition = true;
		
		id = "";
				
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		clickX = 0;
		clickY = 0;
		clickW = 0;
		clickH = 0;
		dragX = 0;
		dragY = 0;
		dragW = 0;
		dragH = 0;
		
		status = VISIBLE;
		
		backColor = 0x7F7F7FFF;
		printMethod = &print_default;
	}
	
	//Function to add a handler for typing events
	void addHandler_type(TYPEEV(handler)){
		if (handler) typeEvents.push_back(handler);
	}
	
	//Function to remove a handler from typing events
	void removeHandler_type(TYPEEV(handler)){
		list <TYPEEV()> :: iterator i;//Iterator
		
		for (i = typeEvents.begin(); i != typeEvents.end(); i++){//For each item of the event handlers list
			if (*i == handler){//If the handler is the same
				i = typeEvents.erase(i);//Erases the handler
				return;//Exit function
			}
		}
	}
	
	//Function to type in char
	void typeText(string newText){
		list <TYPEEV()> :: iterator i;//Iterator
		typeEvent data {text, newText};//Event data
		
		for (i = typeEvents.begin(); i != typeEvents.end(); i++)//For each event handler
			(*i)(this, data);//Calls event function
			
		text = newText;//Adds char
	}
	
	//Function to check events
	void checkEvents(SDL_Event e){
		control::checkEvents(e);//Checks standard event controls
		
		if (e.type == SDL_KEYDOWN && focus && edit){//Key press event
			char c = e.key.keysym.unicode;//Typed character
			
			if (isprint(c)) typeText(text + c);//If character can be printed adds it to text
			else if (e.key.keysym.sym == SDLK_BACKSPACE && text.size() > 0){//If backspace was pressed
				string newText = text;//New text
				newText.erase(text.size() - 1, 1);//Removes last char if pressed backspace
				typeText(newText);//
			}
		}
	}
	
	//Function to get fore color in SDL_Color format
	SDL_Color getForeColor(){
		return SDL_Color {(foreColor & 0xFF0000) >> 16, (foreColor & 0x00FF00) >> 8, foreColor & 0x0000FF};//Returns result
	}
};

//Fillbar class
class fillbar: public control{
	public:
	int fill, fillMax;//Current fill and maximum fill
	Uint32 fillColor;//Fill color
	
	//Constructor
	fillbar(){
		type = FILLBAR;
		fill = 1;
		fillMax = 1;
		fillColor = 0xFFFFFFFF;
		
		lockPosition = true;
		
		id = "";
				
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		clickX = 0;
		clickY = 0;
		clickW = 0;
		clickH = 0;
		dragX = 0;
		dragY = 0;
		dragW = 0;
		dragH = 0;
		
		status = VISIBLE;
		
		backColor = 0x7F7F7FFF;
		printMethod = &print_default;
	}
};

//Panel class
class panel: public control{
	private:
	int oldDragX, oldDragY, oldDragW, oldDragH;//Old dragging area (to store when minimized)
	
	public:
	int minW, minH;//Minimized panel size
	
	TTF_Font* captionFont;//Panel caption font
	Uint32 captionColor;//Panel caption fore color
	string caption;//Panel caption
	
	list <control*> controls;//Controls of the panel
	
	//Constructor
	panel(){
		type = PANEL;
		
		lockPosition = true;
		
		id = "";
		caption = "";
		
		x = 0;
		y = 0;
		w = 0;
		h = 0;
		dragX = 0;
		dragY = 0;
		dragW = 0;
		dragH = 0;
		
		status = VISIBLE;
		
		captionFont = NULL;
		backColor = 0x7F7F7FFF;
		printMethod = &print_default;
	}
	
	//Function to minimize panel
	void minimize(){
		status = MINIMIZED;
		
		clickX = x;
		clickY = y;
		clickW = minW;
		clickH = minH;
		
		oldDragX = dragX;
		oldDragY = dragY;
		oldDragW = dragW;
		oldDragH = dragH;
		
		dragX = 0;
		dragY = 0;
		dragW = minW;
		dragH = minH;
	}
	
	//Function to enlarge panel
	void enlarge(){
		status = VISIBLE;
		
		clickX = x;
		clickY = y;
		clickW = w;
		clickH = h;
		
		dragX = oldDragX;
		dragY = oldDragY;
		dragW = oldDragW;
		dragH = oldDragH;
	}
	
	//Function to toggle minimized
	void toggleMin(){
		if (status == MINIMIZED) enlarge();
		else if (status == VISIBLE) minimize();
	}
	
	//Function to get panel color (SDL_Color format)
	SDL_Color getCaptionColor(){
		return SDL_Color {(captionColor & 0xFF0000) >> 16, (captionColor & 0x00FF00) >> 8, captionColor & 0x0000FF};
	}
	
	//Function to check events
	void checkEvents(SDL_Event e){
		control::checkEvents(e);//Checks base class events
		
		list<control*>::iterator i;//Iterator for controls
		for (i = controls.begin(); i != controls.end(); i++){//For each control
			//Applies offset
			(*i)->x += x;
			(*i)->y += y;
			(*i)->setClickArea();
			
			control* child = *i;//Pointer to the control
			
			if (child->type == PANEL){//If control is a panel
				panel* p = (panel*) child;//Converted pointer
				p->checkEvents(e);//Checks events
			}
			
			else if (child->type == BUTTON){//If control is a button
				button* p = (button*) child;//Converted pointer
				p->checkEvents(e);//Checks events
			}
			
			else if (child->type == TEXTBOX){//If control is a text box
				textBox* p = (textBox*) child;//Converted pointer
				p->checkEvents(e);//Checks events
			}
		
			else child->checkEvents(e);//Checks events for control
			
			//Removes offset
			(*i)->x -= x;
			(*i)->y -= y;
			(*i)->setClickArea();
		}
	}
	
	//Function to get focused control
	control* getFocused(){
		list<control*>::iterator i;//Iterator for controls
		for (i = controls.begin(); i != controls.end(); i++)//For each control
			if ((*i)->isFocused()) return *i;//Returns control if focused
		
		return NULL;
	}
};

//Default panel printing function
void panel_print_default(SDL_Surface* target, panel* e){
	boxColor(target, e->x, e->y, e->x + e->w, e->y + e->h, e->getBackColor());//Prints filled rectangle
	
	if (e->caption != "" && e->captionFont){//If there are valid caption and caption font
		SDL_Surface* txt = RENDERTEXT(e->captionFont, e->caption.c_str(), e->getCaptionColor());//Renders text
		SDL_Rect o {e->x + 5, e->y + 5};//Offset rectangle
		SDL_BlitSurface(txt, NULL, target, &o);//Blits text
		SDL_FreeSurface(txt);//Frees text
	}
	
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

//Default button printing function
void button_print_default(SDL_Surface* target, button* e){
	if (e->getStatus() == PRESSED)//If button is pressed
		boxColor(target, e->x, e->y, e->x + e->w, e->y + e->h, e->getBackColor_pressed());//Prints filled rectangle
	else//Else
		boxColor(target, e->x, e->y, e->x + e->w, e->y + e->h, e->getBackColor());//Prints filled rectangle
	
	if (e->text != "" && e->font){//If there are valid text and font
		SDL_Surface* txt = RENDERTEXT(e->font, e->text.c_str(), e->getForeColor());//Renders text
		SDL_Rect o {e->x + e->w / 2 - txt->w / 2, e->y + 5};//Offset rectangle
		SDL_BlitSurface(txt, NULL, target, &o);//Blits text
		SDL_FreeSurface(txt);//Frees text
	}
}

//Default image box printing function
void imgBox_print_default(SDL_Surface* target, imgBox* e){
	boxColor(target, e->x, e->y, e->x + e->w, e->y + e->h, e->getBackColor());//Prints filled rectangle
	
	if (e->img){//If there's an image
		SDL_Rect offset;//Offset rectangle
		
		switch (e->style){//According to image stile
		case TOPLEFT:
			offset = {e->x, e->y};//Offset rectangle
			SDL_BlitSurface(e->img, NULL, target, &offset);//Prints image
			break;
		case CENTERED:
			offset = {e->x + (e->w - e->img->w) / 2, e->y + (e->h - e->img->h) / 2};//Offset rectangle
			SDL_BlitSurface(e->img, NULL, target, &offset);//Prints image
			break;
		case STRETCHED:
			SDL_Surface* zoomed = zoomSurface(e->img, double(e->w) / double(e->img->w), double(e->h) / double(e->img->h), SMOOTHING_ON);//Zoomed surface
			offset = {e->x, e->y};//Offset rectangle
			SDL_BlitSurface(zoomed, NULL, target, &offset);//Prints image
			SDL_FreeSurface(zoomed);//Frees zoomed surface
			break;
		}
	}
}

#ifdef _IMAGE
	//Default image.h image box printing function
	void imageBox_print_default(SDL_Surface* target, imageBox* e){
		boxColor(target, e->x, e->y, e->x + e->w, e->y + e->h, e->getBackColor());//Prints filled rectangle
		if (e->i) e->i->print(target, e->x + e->w / 2, e->y + e->h / 2);//Prints image
	}
#endif

//Default text box printing function
void textBox_print_default(SDL_Surface* target, textBox* e){
	boxColor(target, e->x, e->y, e->x + e->w, e->y + e->h, e->getBackColor());//Prints filled rectangle
	
	if ((e->text != "" || e->isFocused()) && e->font){//If there are valid text and font
		#ifdef _SCRIPT//If script defined (that is, if tokenize func is available)
			string curLine = "";//Current line
			int w = 0;//Text width
			int i = 0;//Counter
			int curY = e->y + 5;//Current y coord
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
		#else
			SDL_Surface* text = RENDERTEXT(e->font, (e->text + (e->isFocused() && e->edit ? "|" : "")).c_str(), e->getForeColor());//Text
			SDL_Rect offset;//Offset rectangle
			
			if (e->textAlign == LEFT) offset = {e->x + 5, e->y + 5};
			else if (e->textAlign == RIGHT) offset = {e->x + e->w - text->w, e->y + 5};
			else if (e->textAlign == CENTER) offset = {e->x + (e->w - text->w) / 2, e->y + 5};
			
			SDL_BlitSurface(text, NULL, target, &offset);//Prints text
			SDL_FreeSurface(text);//Frees text*/
		#endif
	}
}

//Default fillbar printing function
void fillBar_print_default(SDL_Surface* target, fillbar* e){
	boxColor(target, e->x, e->y, e->x + e->w, e->y + e->h, e->getBackColor());//Prints filled rectangle
	boxColor(target, e->x, e->y, e->x + (e->w * e->fill / e->fillMax), e->y + e->h, e->fillColor);//Prints fill
}

//Default control printing function
void print_default(SDL_Surface* target, control* p){
	if (p->getStatus() == HIDDEN) return;//Exits function if control is hidden
	
	if (p->type == PANEL)//If control is a panel
		panel_print_default(target, (panel*) p);//Prints as panel
	
	else if (p->type == BUTTON)//If control is a button
		button_print_default(target, (button*) p);//Prints as button
	
	else if (p->type == IMGBOX)//If control is an image box
		imgBox_print_default(target, (imgBox*) p);//Prints as image box
	
	else if (p->type == TEXTBOX)//If control is a text box
		textBox_print_default(target, (textBox*) p);//Prints as text box
		
	#ifdef _IMAGE
		else if (p->type == IMAGEBOX)//If control is a image.h imagebox
		imageBox_print_default(target, (imageBox*) p);//Prints as image.h imagebox
	#endif
	
	else boxColor(target, p->x, p->y, p->x + p->w, p->y + p->h, p->getBackColor());//Prints filled rectangle
}

#endif