//SCRIPTING HEADER

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

//SYNTAX:
//[object type] [object name] {
//	[variable name] = [variable value]; (this creates variable)
//	...
//};

#ifndef _SCRIPT
#define _SCRIPT

#include <cstdlib>//Includes standard library

#include <string>//Includes strings

#include <iostream>//Includes streams
#include <fstream>//Includes filestreams
#include <sstream>//Includes stringstreams (used for data-to-string conversions)

#include <deque>//Includes double-ended queues (easy element access by index)

#include <cstring>//Includes standard C string library

using namespace std;

#ifdef _ERROR//If the error handling file has been included
	int SCRIPT_ERROR 						= getErrorCode();//Code for generic script errors
	int SCRIPT_ERROR_FILENOTFOUND 			= getErrorCode();//File not found errors
	int SCRIPT_ERROR_BRACKETS				= getErrorCode();//Brackets errors
	int SCRIPT_WARNING						= getErrorCode();//Code for generic script warnings
	int SCRIPT_WARNING_UNRESOLVEDLINE		= getErrorCode();//Unresolved line warnings
	int SCRIPT_WARNING_INVALIDOBJTYPE		= getErrorCode();//Invalid object type warnings
#endif

class var;//Variable class prototype
class object;//Object class prototype
class fileData;//File data class prototype

//Function to get a string deque of string tokens
deque <string> tokenize(string source, string delimiters){
	deque<string> result;//Result
	
	char *s = new char [source.size() + 1];
	char *d = new char [delimiters.size() + 1];
	
	strcpy (s, source.c_str());
	strcpy (d, delimiters.c_str());
	
	char *tokPtr = strtok(s, d);//First token
	
	while (tokPtr != NULL){//While there are tokens left
		result.push_back(string(tokPtr));//Adds token to result
		tokPtr = strtok(NULL,d);//Next token
	}
	
	return result;//Returns result
}

//Function to convert any data to string
template <class type> string toString(type t){
	stringstream s;
	s << t;
	return s.str();
}

//Function to count the repetition of chars in a string
int howMany(string s, char c){
	int result = 0;//Result
	int pos = s.find(c);//Position of char to find
	
	while (pos != s.npos){//While other occurrencies of the char are found
		result++;
		pos = s.find(c, pos + 1);//Next char
	}
	
	return result;//Returns the result
}

//Variable class
//Used to store content variables; values are stored as strings
class var{
	public:
	string name;//Variable name
	string value;//Variable value
	
	//Constructor
	var(){
		name = "";
		value = "";
	}
	
	//Constructor
	template <class type> var(string name, type value){
		this->name = name;
		set(value);
	}
	
	//Function to set variable value
	template <class type> void set(type t){
		value = toString(t);//Sets value converted to string
	}
	
	//Function to return value as integer
	int intValue(){
		return atoi(value.c_str());
	}
	
	//Function to return value as double
	double doubleValue(){
		return atof(value.c_str());
	}
};

//Object class
//Like C structure, multiple variables stored in an object only; used to represent content items
class object{
	public:
	string type;//Object type
	string name;//Object name
	
	deque<var> v;//Object variables
	deque<object> o;//Sub-objects
	
	//Constructor
	object(){
		type = "";
		name = "";
	}
	
	//Constructor
	object(string type, string name){
		this->type = type;
		this->name = name;
	}
	
	//Function that returns a pointer to a variable
	var* getVar(string name){
		deque<var>::iterator i;//Iterator
		for (i = v.begin(); i != v.end(); i++)//For each variable
			if (i->name == name) return &*i;//Returns the variable if matching
			
		return NULL;//Returns null if no variable was found
	}
	
	//Function that returns a pointer to a sub-object
	object* getObj(string name){
		deque<object>::iterator i;//Iterator
		for (i = o.begin(); i != o.end(); i++)//For each sub-object
			if (i->name == name) return &*i;//Returns the object if matching
			
		return NULL;//Retnurs null if no object was found
	}
	
	//Function to set the value of a variable
	//if the variable doesn't exist, the function creates it
	template <class type> void setVar(string name, type value){
		var* v = getVar(name);//Pointer to the variable
		
		if (v) v->set(value);//Sets the value if the variable exists

		else{//If not
			var newVar (name, value);//New variable
			this->v.push_back(newVar);//Adds the variable
		}
	}
	
	//Operator + to join two objects
	//If there are variables with the same name, they'll take the value of the second object (argument of the function)
	//If there are sub-objects with the same name, they'll be joined
	object operator + (object b){
		object result = *this;//Result
		
		deque<var>::iterator i;//Iterator for variables
		for (i = b.v.begin(); i != b.v.end(); i++)//For each variable of the other object
			result.setVar(i->name, i->value);//Sets result variable
		
		deque<object>::iterator j;//Iterator for objects
		for (j = b.o.begin(); j != b.o.end(); j++){//For each object of the other object
			object* ob = result.getObj(j->name);//Pointer to the required object
			
			if (ob) *ob += *j;//If the object already exists, joins the two objects
			else result.o.push_back(*j);//Else adds the object to the result
		}
		
		return result;//Returns the result
	}
	
	//Operator +=
	void operator += (object b){
		*this = *this + b;
	}
	
	//Function to read the object from script strings (only prototype; function defined below class fileData)
	void fromStrings(string type, string name, deque<string> lines);
	
	//Function to print the object onto a string
	string toString(int indent = 0){
		string result = "";//Result
		int i;//Counter to indent
		
		deque<object>::iterator l;//Iterator
		for (l = o.begin(); l != o.end(); l++){//For each object
			for (i = 0; i < indent; i++) result += "\t";//Indent
			result += l->type + " " + l->name + " {\n";//Object declaration
			result += l->toString(indent + 1);//Object content
			for (i = 0; i < indent; i++) result += "\t";//Indent
			result += "};\n";//Close bracket
			
			if (l != o.end() - 1 || v.size() > 0) result += "\n";
		}
		
		deque<var>::iterator j;//Iterator
		for (j = v.begin(); j != v.end(); j++){//For each variable
			for (i = 0; i < indent; i++) result += "\t";//Indent
			result += j->name + " = " + j->value + ";\n";//Print variable
		}
		
		return result;//Returns the result
	}
};

//File data class
//Used to read files and make content from them
class fileData{
	public:
	string fileName;//File name
	
	string readContent;//Content directly read from the file, all in one line
	deque<string> procContent;//Processed content, obtained by splitting readContent in semicolon-divided lines
	
	//Constructor
	fileData(){
		fileName = "";
		readContent = "";
	}
	
	//Constructor
	fileData(string path){
		read(path);//Reads file
		proc();//Processes the content
	}
	
	//File reading function
	//Reads the content of the file and stores it on a single line in readContent
	void read(string path){
		ifstream file(path.c_str());//Opens file for reading
		string tmpString;//Temporary string
		
		fileName = path;//Sets file name
		readContent = "";//Resets content
		
		#ifdef _ERROR//If the error handling file has been included
			if (!file.good() && errFunc)//If the file isn't good and there is an error function
				errFunc(SCRIPT_ERROR_FILENOTFOUND, "[script.h] File not found " + path);//Calls error function
		#endif
		
		while (file.good()){//While the file is good for reading
			getline(file, tmpString);//Reads line from file
			readContent += " " + tmpString;//Adds the line to the content
		}
		
		file.close();//Closes file
	}
	
	//File processing function
	//Generates procContent
	void proc(){
		int pos, lastPos = -1;//Position in the content and position of the last semicolon
		string tmpString;//Temporary string for each line
		
		char* tok = strtok((char*) readContent.c_str(), ";");//First token
			
		while (tok){//While there are still tokens left
			procContent.push_back(string(tok));//Adds the token to the content
			tok = strtok(NULL, ";");//Next token
		}
	}
	
	//Function to generate file object; the parent object contains the child objects declared in the file
	object objGen(string name){
		object o;//New object
		o.fromStrings("file", name, procContent);//Generates the object from the content
		return o;//Returns the object
	}
};

//Function to read an object from script strings
void object::fromStrings(string type, string name, deque<string> lines){
	deque<string>::iterator i;//Iterator
	int lCount = 1;//Line counter (for error-finding purposes)
	
	//Sets type and name
	this->type = type;
	this->name = name;
	
	#ifdef _ERROR
		if (errFunc){
			int open = 0;
			for (i = lines.begin(); i != lines.end(); i++){
				open += howMany(*i, '{');
				open -= howMany(*i, '}');
			}
			
			if (open != 0) errFunc(SCRIPT_ERROR_BRACKETS, "[script.h] Brackets not matching in object " + name);
		}
	#endif
	
	for (i = lines.begin(); i != lines.end(); i++, lCount++){//For each line
		//Checks for null chars in the middle of the string and removes them (possibly generated when splitting strings...)
		int n;
		for (n = 0; n < i->size(); n++){
			if ((*i)[n] == '\0') (*i)[n] = ' ';
		}
		
		string ln = *i;//Line content (saved for error checking before splitting into tokens)
		
		deque<string> tokens;//Tokens of the line
		char* tok = strtok((char*)i->c_str(), " \t");//First token
		
		while (tok){//While there are tokens left
			tokens.push_back(string(tok));//Adds the token
			tok = strtok(NULL, " \t");//Next token
		}
		
		if (tokens.size() >= 3 && tokens[1] == "="){//Variable assignment
			setVar(tokens[0], ln.substr(ln.find(tokens[2])));//Sets the variable
		}
		
		else if (tokens.size() >= 3 && tokens[2] == "{"){//Object declaration
			object newObj;//New object
			
			deque<string> subLines;//Lines of the sub-object
			subLines.push_back(ln.substr(ln.find("{") + 1));//Adds first line
			
			int open = 1;//Open brackets counter
			deque<string>::iterator j;//Iterator for object end
			
			open += howMany(subLines[0], '{');//Counts open brackets of the first line
			open -= howMany(subLines[0], '}');//Counts close brackets of the first line
			
			for (j = i + 1; j != lines.end(); j++){//For each of the following lines
				open += howMany(*j, '{');//Open brackets
				open -= howMany(*j, '}');//Close brackets
				
				if (open <= 0) break;//Exit loop if all brackets have been closed
				else subLines.push_back(*j);//Else adds line to sub-object
			}
			
			newObj.fromStrings(tokens[0],tokens[1],subLines);//Creates object
			
			o.push_back(newObj);//Adds object
			i = j;//Next line
		}
		
		else if (tokens.size() >= 2 && tokens[0] == "file"){//File insertion
			fileData f (ln.substr(ln.find(tokens[1])));//New file
			*this += f.objGen(tokens[1]);//Adds the content of the file to the object
		}
		
		#ifdef _ERROR//If the error handling file has been included
			else if (tokens.size() > 0 && errFunc)//If there's an error function and the line is unresolved
				errFunc(SCRIPT_WARNING_UNRESOLVEDLINE, "[script.h] Unresolved line " + name + ":" + toString(lCount) + ":" + ln);//Calls error function 
		#endif
	}
}

#endif