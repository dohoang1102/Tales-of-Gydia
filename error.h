//Error handling header

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