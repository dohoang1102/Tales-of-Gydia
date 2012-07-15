//Localization header

#ifndef _LOCALE
#define _LOCALE

#include <string>
#include <list>

#ifdef _SCRIPT
	#define OBJTYPE_TEXT	"text"//Script object type text
	#define OBJTYPE_LOCALE	"locale"//Script object type locale
#endif

#ifdef _ERROR
	int LOCALE_ERROR 						= getErrorCode();//Code for generic locale errors
	int LOCALE_WARNING						= getErrorCode();//Code for generic locale warnings
	int LOCALE_INVALIDTEXTIDWARNING			= getErrorCode();//Code for invalid text id warning
#endif

//Text class (id, lang, content)
class txt{
	public:
	string id;//Text id
	string lang;//Language
	string text;//Content
	
	//Constructor
	txt(){
		id = "";
		lang = "";
		text = "";
	}
	
	#ifdef _SCRIPT//If the script header has been included
		//Function to load from script object
		void fromScriptObj(object o){
			if (o.type == OBJTYPE_TEXT){//If the object type is matching
				var* lang = o.getVar("lang");//Language variable
				var* text = o.getVar("text");//Text variable
				
				id = o.name;//Object name is text id
				
				if (lang) this->lang = lang->value;//Loads language
				if (text) this->text = text->value;//Loads text
			}
			
			#ifdef _ERROR
				else if (errFunc) errFunc(SCRIPT_WARNING_INVALIDOBJTYPE, "[locale.h] No text could be loaded from " + o.type + " object");
			#endif
		}
	#endif
};

//Class localization
class loc{
	public:
	string id;//Language id
	list <txt> texts;//Texts of the locale
	
	//Constructor
	loc(){
		id = "";
	}
	
	//Function to add new text
	void add(string id, string text){
		txt t;//New text
		t.id = id;
		t.lang = this->id;
		t.text = text;
		texts.push_back(t);//Adds new text
	}
	
	//Function to get text with given id
	string getText(string id){
		list<txt>::iterator t;//Iterator
		for (t = texts.begin(); t != texts.end(); t++)//For each text
			if (t->id == id) return t->text;//Returns text if matching
			
		#ifdef _ERROR
			if (errFunc) errFunc(LOCALE_INVALIDTEXTIDWARNING, "Invalid text id " + id + "," + this->id);
		#endif
		
		return id;//Returns empty string if no text was found
	}
	
	#ifdef _SCRIPT//If the script header has been included
		//Function to load from script object
		void fromScriptObj(object o){
			if (o.type == OBJTYPE_LOCALE){//If the object type is matching
				id = o.name;
				
				deque<object>::iterator i;//Iterator
				for (i = o.o.begin(); i != o.o.end(); i++){//For each sub-object
					if (i->type == OBJTYPE_TEXT){//If object is a text
						txt newText;//New text
						newText.fromScriptObj(*i);//Loads text
						newText.lang = id;//Corrects language
						texts.push_back(newText);//Adds new text
					}
				}
			}
			
			#ifdef _ERROR
				else if (errFunc) errFunc(SCRIPT_WARNING_INVALIDOBJTYPE, "[locale.h] No locale could be loaded from " + o.type + " object");
			#endif
		}
	#endif
	
	//Function to merge two localizations (new localization contains texts from both old ones)
	loc operator + (loc l){
		loc result;//Result localization
		
		result.id = id;//Sets id
		
		list<txt>::iterator i;//Iterator
		for (i = texts.begin(); i != texts.end(); i++) result.texts.push_back(*i);//Adds texts from this locale
		for (i = l.texts.begin(); i != l.texts.end(); i++) result.texts.push_back(*i);//Adds texts from other locale
		
		return result;//Returns result
	}
	
	//Increment operator
	void operator += (loc l){
		*this = *this + l;
	}
};

#endif