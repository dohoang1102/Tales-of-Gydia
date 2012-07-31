//String expression handling header

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

#include <iostream>
#include <string>
#include <deque>
#include <algorithm>

//Operator class
class op {
	public:
	string sign;//Sign character
	int priority;//Operator priority (highest priority = first execution)
	string (*func) (string, string);//Operator function
	
	//Constructor
	op(){
		sign = "";
		priority = 0;
		func = NULL;
	}
	
	//Constructor
	op(string sign, int priority, string (*func) (string, string)){
		this->sign = sign;
		this->priority = priority;
		this->func = func;
	}
};

//Operator comparison function (used when sorting)
bool opCompare(op a, op b){
	if (a.priority < b.priority) return true;
	return false;
}

//Function to get operator
op* getOp(string sign, int priority, deque<op> *operators){
	deque<op>::iterator i;
	for (i = operators->begin(); i != operators->end(); i++)//For each operator
		if (i->sign == sign && (priority == -1 || i->priority == priority)) return &*i;//Returns operator if matching
		
	return NULL;//Returns null
}

//Expression solving function
//Takes the expression source, the list of available operators and a pointer to deque of variables
string expr(string source, deque<op> *operators){
	string s (source);//Copies string
	
	deque<string> tokens = tokenize(s, "\t ");//String tokens
		
	sort(operators->begin(), operators->end(), opCompare);//Sorts operators by priority
	
	int i;//Counter
	int curPriority;//Current priority
	for (curPriority = 0; curPriority <= operators->back().priority; curPriority++){//For each priority value
		for (i = tokens.size() - 1; i >= 0; i--){//For each token (from the last to the first)
			if (getOp(tokens[i], curPriority, operators)){//If operator exists
				string left = "", right = "";//Left and right side of operator
				
				int j;//Counter
				for (j = 0; j < i; j++) left += tokens[j] + " ";//Fills left side
				for (j = i + 1; j < tokens.size(); j++) right += tokens[j] + " ";//Fills right side
				
				string result = getOp(tokens[i], curPriority, operators)->func(expr(left, operators), expr(right, operators));//Calculates result
				return result;//Returns result
			}
		}
	}
	
	return tokens[0];//Returns plain string if couldn't do anything
}