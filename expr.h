//String expression handling header

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
string expr(string source, deque<op> *operators, deque<var> *vars = NULL){
	string s (source);//Copies string
	
	deque<string> tokens = tokenize(s, "\t ");//String tokens
		
	sort(operators->begin(), operators->end(), opCompare);//Sorts operators by priority
	
	int i;//Counter
	if (vars){//If there's a variable deque
		for (i = 0; i < tokens.size(); i++){//For each token
			if (tokens[i][0] == '$'){//If token is a variable
				int j;//Counter
				for (j = 0; j < vars->size(); j++){//For each variable
					if ((*vars)[j].name == tokens[i].substr(1)){//If var matches
						tokens[i] = (*vars)[j].value;//Sets token
						break;//Exits loop
					}
				}
			}
		}
	}
	
	int curPriority;//Current priority
	for (curPriority = 0; curPriority <= operators->back().priority; curPriority++){//For each priority value
		for (i = tokens.size() - 1; i >= 0; i--){//For each token (from the last to the first)
			if (getOp(tokens[i], curPriority, operators)){//If operator exists
				string left = "", right = "";//Left and right side of operator
				
				int j;//Counter
				for (j = 0; j < i; j++) left += tokens[j] + " ";//Fills left side
				for (j = i + 1; j < tokens.size(); j++) right += tokens[j] + " ";//Fills right side
				
				string result = getOp(tokens[i], curPriority, operators)->func(expr(left, operators, vars), expr(right, operators, vars));//Calculates result
				return result;//Returns result
			}
		}
	}
	
	return tokens[0];//Returns plain string if couldn't do anything
}