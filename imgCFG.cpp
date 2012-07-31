//Tool to create .cfg files related to a spritesheet
//Takes as arguments the image file to split, its width, its sizes (rows and columns in frames), the frames sizes (in pixels) and the output file path
//Generates a .cfg file containing the image objects for all the frames in the spritesheet
//arguments: -ifFILE -ofOUTPUT -iwIMAGE_WIDTH -ihIMAGE_HEIGHT -fwFRAMEWIDTH -fhFRAMEHEIGHT

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
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>

using namespace std;

int main (int argc, char* argv[]){
	string imgFilePath = "";//Path of the image file
	int imgColumns = -1, imgRows = -1, frameW = -1, frameH = -1;//Rows and columns (in frames) of the image and of each frame
	string oFilePath = "";//Path of the output file
	string prefix = "";
	string extras = "";
	
	int i;//Counter
	for (i = 0; i < argc; i++){//For each argument
		string s = argv[i];

		if (s.size() >= 3 && s.substr(0,3) == "-if") imgFilePath = s.substr(3);
		else if (s.size() >= 3 && s.substr(0,3) == "-of") oFilePath = s.substr(3);
		else if (s.size() >= 3 && s.substr(0,3) == "-iw") imgColumns = atoi(s.substr(3).c_str());
		else if (s.size() >= 3 && s.substr(0,3) == "-ih") imgRows = atoi(s.substr(3).c_str());
		else if (s.size() >= 3 && s.substr(0,3) == "-fw") frameW = atoi(s.substr(3).c_str());
		else if (s.size() >= 3 && s.substr(0,3) == "-fh") frameH = atoi(s.substr(3).c_str());
		else if (s.size() >= 3 && s.substr(0,3) == "-pf") prefix = s.substr(3);
		else if (i != 0) extras += s + " ";
	}
	
	int x, y;//Current frame's x and y coordinates
	
	if (imgFilePath == ""){
		cout << "Image file path: ";
		cin >> imgFilePath;
	}
	
	if (oFilePath == ""){
		cout << "Output file path: ";
		cin >> oFilePath;
	}
	
	if (imgColumns == -1){
		cout << "Image columns: ";
		cin >> imgColumns;
	}
	
	if (imgRows == -1){
		cout << "Image rows: ";
		cin >> imgRows;
	}
	
	if (frameW == -1){
		cout << "Frame width: ";
		cin >> frameW;
	}
	
	if (frameH == -1){
		cout << "Frame height: ";
		cin >> frameH;
	}
	
	cout << "Current extras: " << extras << endl;
	if (extras == ""){
		cout << "Extra options (enter end when done):\n";
		string t = "";

		getline(cin, t);
		while (t != "end"){
			extras += t + "\n\t";
			getline(cin, t);
		}
	}
	
	ofstream o (oFilePath.c_str());//Output file
	
	for (y = 0; y < imgRows; y++){//For each row
		for (x = 0; x < imgColumns; x++){//For each column
			o << "image " << prefix << y << "_" << x << " {\n\timage = " << imgFilePath << ";\n\n\t" << extras << "\n\n\trect rect {\n\t\tx = " << x * frameW << ";\n\t\ty = " << y * frameH << ";\n\t\tw = " << frameW << ";\n\t\th = " << frameH << ";\n\t};\n};\n\n";//Prints the object content
		}
	}
	
	o.close();//Closes the file
	
	return 0;
}