//Image handling header

/* Tales of Gydia - turn based RPG
Copyright (C) 2012  Michele Bucelli
 
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#ifndef _IMAGE
#define _IMAGE

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

class image;//Image class prototype

#ifdef _SCRIPT
	#define OBJTYPE_IMAGE 	"image"
	#define OBJTYPE_ANIM	"anim"
	#define OBJTYPE_ANIMSET "animSet"
	#define OBJTYPE_RECT	"rect"
	
	//Function to convert script object in rectangle
	SDL_Rect objToRect(object o){
		if (o.type == OBJTYPE_RECT){//If the type matches
			var* x = o.getVar("x");
			var* y = o.getVar("y");
			var* w = o.getVar("w");
			var* h = o.getVar("h");
			
			SDL_Rect result;
			
			if (x) result.x = x->intValue();
			if (y) result.y = y->intValue();
			if (w) result.w = w->intValue();
			if (h) result.h = h->intValue();
			
			return result;
		}
		
		#ifdef _ERROR
			if (errFunc) errFunc(SCRIPT_WARNING_INVALIDOBJTYPE, "[image.h] No rectangle could be loaded from " + o.type + " object");
		#endif
		
		return SDL_Rect {0,0,0,0};
	}
	
	//Function to convert rectangle to script object
	object rectToObj(SDL_Rect r, string name){
		object result;//Result
		
		result.name = name;
		result.type = OBJTYPE_RECT;
		result.setVar("x", r.x);
		result.setVar("y", r.y);
		result.setVar("w", r.w);
		result.setVar("h", r.h);
		
		return result;
	}
#endif

#ifdef _ERROR
	int IMAGE_ERROR 						= getErrorCode();//Code for generic image errors
	int IMAGE_INITERROR						= getErrorCode();//Code for initialization error
	int IMAGE_WARNING						= getErrorCode();//Code for generic image warnings
	int IMAGE_WARNING_INVALIDFRAME			= getErrorCode();//Invalid frame in animation
	int IMAGE_WARNING_INVALIDANIM			= getErrorCode();//Invalid anim in animSet
#endif

//Initializing function
int image_init(){
	int sdl = SDL_Init(SDL_INIT_EVERYTHING);//Initializes SDL
	int sdl_image = IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);//Initializes SDL_image
	
	if (sdl != 0){//If there was an error initializing SDL
		#ifdef _ERROR
			if (errFunc) errFunc(IMAGE_INITERROR, "Error initializing SDL");
		#endif
		return 1;
	}
	
	if (!sdl_image){//If there was an error initializing SDL_image
		#ifdef _ERROR
			if (errFunc) errFunc(IMAGE_INITERROR, "Error initializing SDL_image");
		#endif
		return 2;
	}
	
	return 0;//Initialization successful
}

#ifdef _CACHE//If the cache handling header has been included
	cache <SDL_Surface*, &IMG_Load> imgCache;//Image cache
	#define CACHEDIMG(ID)	imgCache.getObj(ID)//Macro to get image from cache
#endif

#define ANIM_STANDARD	0b00000000//Standard animation
#define ANIM_PLAYONCE	0b00000001//Play-once animation (NOTE: this is not handled directly by the image.h header - you must handle it in your program)
#define ANIM_DOUBLE		0b00000010//Play twice each frame (actually duplicates sequence when loading)

//Image class
class image{
	public:
	string id;//Id of the image
	
	SDL_Surface* img;//Surface of the image
	string imageFile;//Image file
	
	SDL_Rect rect;//Portion of the image to print
	
	int cShiftX, cShiftY;//Shift the center (reference when printing)
	
	//Constructor
	image(){
		img = NULL;
		rect = {0,0,0,0};
		
		cShiftX = 0;
		cShiftY = 0;
	}

	//Function that prints the image (coordinates refer to the center)
	void print(SDL_Surface* target, int x, int y){
		SDL_Rect o {x - rect.w / 2 + cShiftX, y - rect.h / 2 + cShiftY};//Offset rectangle
		SDL_BlitSurface(img, &rect, target, &o);//Blits the image on the target surface
	}
	
	//Function to print (coordinates referred to top-left corner)
	void print_corner(SDL_Surface* target, int x, int y){
		SDL_Rect o {x, y};//Offset rectangle
		SDL_BlitSurface(img, &rect, target, &o);//Blits the image on the target surface
	}
	
	#ifdef _SCRIPT//If the script handling header has been included
		//Function to load the image from a script object
		void fromScriptObj(object o){
			if (o.type == OBJTYPE_IMAGE){//If the object type is matching
				var* fileName = o.getVar("image");//Image file variable
				object* rect = o.getObj("rect");//Rectangle object
				var* cShiftX = o.getVar("cShiftX"), *cShiftY = o.getVar("cShiftY");//Centre shifting on two axes
				
				id = o.name;//The id is the name of the object
				
				if (fileName){
					imageFile = fileName->value;
					
					#ifdef _CACHE//If the cache handling header has been included
					img = CACHEDIMG(fileName->value);//Loads the image from the cache
					#else
					img = IMG_Load(fileName->value.c_str());//Loads the image from the file
					#endif
				}
				
				if (rect) this->rect = objToRect(*rect);//Loads rectangle
				
				//Reads centre shifting
				if (cShiftX) this->cShiftX = cShiftX->intValue();
				if (cShiftY) this->cShiftY = cShiftY->intValue();
			}
			
			#ifdef _ERROR
				else if (errFunc) errFunc(SCRIPT_WARNING_INVALIDOBJTYPE, "[image.h] No image could be loaded from " + o.type + " object");
			#endif
		}
		
		//Function to convert to script object
		object toScriptObj(){
			object result;//Result
			
			result.name = id;
			result.type = OBJTYPE_IMAGE;
			
			result.setVar("image", imageFile);
			result.setVar("cShiftX", cShiftX);
			result.setVar("cShiftY", cShiftY);
			
			result.o.push_back(rectToObj(rect, "rect"));
			
			return result;
		}
	#endif
};

//Animation class
class anim{
	public:
	string id;//Id of the animation
	deque<image> frames;//Frames of the animation
	deque<string> sequence;//Names of the frames in sequence
	
	Uint8 flags;//Animation flags
	
	unsigned int curFrame;//Current frame
	
	//Constructor
	anim(){
		curFrame = 0;
		flags = ANIM_STANDARD;
	}
	
	//Function to print the animation
	void print(SDL_Surface* target, int x, int y){
		image* i = getFrame(sequence[curFrame]);//Image to print
		if (i) i->print(target, x, y);//Prints the image if found
		
		#ifdef _ERROR
			else if (errFunc) errFunc(IMAGE_WARNING_INVALIDFRAME, "[image.h] " + sequence[curFrame] + " is not a valid frame of anim " + id);
		#endif
	}
	
	//Function to get a pointer to the requested frame
	image* getFrame(string id){
		deque<image>::iterator i;//Iterator for the images
		for (i = frames.begin(); i != frames.end(); i++)//For each frame
			if (i->id == id) return &*i;//Returns the image if matching
			
		return NULL;//Returns NULL if no frame was found
	}
	
	//Function to go to the next frame
	void next(){
		curFrame++;//Increases the frame count
		if (curFrame >= sequence.size()) curFrame = 0;//If reached the end goes back to the first frame
	}
	
	#ifdef _SCRIPT//If the script handling header has been included
		//Function to load from script object
		void fromScriptObj(object o){
			if (o.type == OBJTYPE_ANIM){//If the object type is matching
				var* seq = o.getVar("sequence");//Sequence variable
				var* playOnce = o.getVar("playOnce");//Play once variable
				var* dup = o.getVar("dup");//Duplicate variable
				
				id = o.name;//The id is the name of the object
				
				if (playOnce && playOnce->intValue()) flags |= ANIM_PLAYONCE;//Play once flag
				if (dup && dup->intValue()) flags |= ANIM_DOUBLE;//Double flag
				
				if (seq){//If the sequence variable exists
					char* cstr = (char*) seq->value.c_str();//C-string variable
					char* tok = strtok(cstr, ", ");//Token of the sequence
					
					while (tok){//While there are tokens left
						sequence.push_back(string(tok));//Adds the string to the sequence
						if (flags & ANIM_DOUBLE) sequence.push_back(string(tok));//Adds the string to the sequence
						tok = strtok(NULL, ", ");//Next token
					}
				}
				
				deque<object>::iterator i;//Iterator for sub-objects
				for (i = o.o.begin(); i != o.o.end(); i++){//For each object
					if (i->type == OBJTYPE_IMAGE){//If the object's an image
						image m;//New image
						m.fromScriptObj(*i);//Loads the image
						frames.push_back(m);//Adds the image to the frames
					}
				}
			}
			
			#ifdef _ERROR
				else if (errFunc) errFunc(SCRIPT_WARNING_INVALIDOBJTYPE, "[image.h] No animation could be loaded from " + o.type + " object");
			#endif
		}
		
		//Function to convert to script object
		object toScriptObj(){
			object result;//Result
			
			result.type = OBJTYPE_ANIM;
			result.name = id;
			
			int i;
			for (i = 0; i < frames.size(); i++) result.o.push_back(frames[i].toScriptObj());
			
			string seqVar = "";
			for (i = 0; i < sequence.size(); i++) seqVar += "," + sequence[i];
			seqVar.erase(0, 1);
			result.setVar("sequence", seqVar);
			
			return result;
		}
	#endif
	
	//Function returning total duration
	int duration(){
		return sequence.size();
	}
	
	//Function returning current frame
	image* current(){
		return getFrame(sequence[curFrame]);
	}
};

//Animation set class
class animSet{
	public:
	string id;//Id of the animation set
	deque<anim> anims;//Animations
	int curAnim;//Current animation
	
	//Constructor
	animSet(){
		curAnim = 0;
	}
	
	//Function to get a pointer to the requested anim
	anim* getAnim(string id){
		deque<anim>::iterator i;//Iterator for the animations
		for (i = anims.begin(); i != anims.end(); i++)//For each animation
			if (i->id == id) return &*i;//Returns the pointer to the animation if matches
			
		return NULL;//Returns null if no animation was found
	}
	
	//Function to get a pointer to current anim
	anim* current(){
		return &anims[curAnim];//Returns current animation
	}
	
	//Functiont to set the anim by id
	void setAnim(string id){
		if (anims[curAnim].id == id) return;//Exit function if the anim is the same as the current one
		
		int i;//Counter
		for (i = 0; i < anims.size(); i++){//For each animation
			if (anims[i].id == id){//If animation is matching
				curAnim = i;//Sets animation
				anims[curAnim].curFrame = 0;//Restarts the anim
				return;//Exit function
			}
		}
		
		#ifdef _ERROR
			if (errFunc) errFunc(IMAGE_WARNING_INVALIDANIM, "[image.h] " + id + " is not a valid animation of animation set " + this->id);
		#endif
		
		curAnim = 0;//Sets first anim if no animation was found
		anims[curAnim].curFrame = 0;//Restarts the anim
	}
	
	//Function to go to the next frame
	void next(){
		anims[curAnim].next();//Goes to the next frame
	}
	
	//Function to print the anim
	void print(SDL_Surface* target, int x, int y){
		anims[curAnim].print(target, x, y);//Prints the anim
	}
	
	#ifdef _SCRIPT//If the script handling header has been included
		//Function to load from script object
		void fromScriptObj(object o){
			if (o.type == OBJTYPE_ANIMSET){
				id = o.name;//The id is the name of the object
				
				deque<object>::iterator i;//Iterator for the sub-objects
				for (i = o.o.begin(); i != o.o.end(); i++){//For each sub-object
					if (i->type == OBJTYPE_ANIM){//If the object type is matching
						anim newAnim;//New animation
						newAnim.fromScriptObj(*i);//Loads the anim from the object
						anims.push_back(newAnim);//Adds the anim
					}
				}
			}
			
			#ifdef _ERROR
				else if (errFunc) errFunc(SCRIPT_WARNING_INVALIDOBJTYPE, "[image.h] No animation set could be loaded from " + o.type + " object");
			#endif
		}
		
		//Function to convert to script object
		object toScriptObj(){
			object result;//Result
			
			result.name = id;
			result.type = OBJTYPE_ANIMSET;
			
			int i;
			for (i = 0; i < anims.size(); i++) result.o.push_back(anims[i].toScriptObj());
			
			return result;
		}
	#endif
};

#endif