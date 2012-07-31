//Error handling header

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

#ifndef _ERROR
#define _ERROR

#include <iostream>
#include <string>

using namespace std;

//Default error handling function
void defaultErrFunc(int code, string msg){
	cerr << code << ":" << msg << endl;
}

void (*errFunc)(int, string) = &defaultErrFunc;//Function to handle errors (arguments: code, message)

int eCodes = 0;//Error codes created

//Function to get a valid and unused error code
int getErrorCode(){
	return eCodes++;//Returns the error code and increments the number of error codes created
}

#endif