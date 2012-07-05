//Tool to create .cfg files related to a spritesheet
//Takes as arguments the image file to split, its width, its sizes (rows and columns in frames), the frames sizes (in pixels) and the output file path
//Generates a .cfg file containing the image objects for all the frames in the spritesheet
//arguments: -ifFILE -ofOUTPUT -iwIMAGE_WIDTH -ihIMAGE_HEIGHT -fwFRAMEWIDTH -fhFRAMEHEIGHT

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>

using namespace std;

int main (int argc, char* argv[]){
	string imgFilePath;//Path of the image file
	int imgColumns, imgRows, frameW, frameH;//Rows and columns (in frames) of the image and of each frame
	string oFilePath;//Path of the output file
	string prefix = "";
	
	int i;//Counter
	for (i = 0; i < argc; i++){//For each argument
		string s = argv[i];

		if (s.size() <= 3) continue;
		if (s.substr(0,3) == "-if") imgFilePath = s.substr(3);
		if (s.substr(0,3) == "-of") oFilePath = s.substr(3);
		if (s.substr(0,3) == "-iw") imgColumns = atoi(s.substr(3).c_str());
		if (s.substr(0,3) == "-ih") imgRows = atoi(s.substr(3).c_str());
		if (s.substr(0,3) == "-fw") frameW = atoi(s.substr(3).c_str());
		if (s.substr(0,3) == "-fh") frameH = atoi(s.substr(3).c_str());
		if (s.substr(0,3) == "-pf") prefix = s.substr(3);
	}
	
	int x, y;//Current frame's x and y coordinates
	
	ofstream o (oFilePath.c_str());//Output file
	
	for (y = 0; y < imgRows; y++){//For each row
		for (x = 0; x < imgColumns; x++){//For each column
			o << "image " << prefix << y << "_" << x << " {\n\timage = " << imgFilePath << ";\n\n\trect rect {\n\t\tx = " << x * frameW << ";\n\t\ty = " << y * frameH << ";\n\t\tw = " << frameW << ";\n\t\th = " << frameH << ";\n\t};\n};\n\n";//Prints the object content
		}
	}
	
	o.close();//Closes the file
	
	return 0;
}