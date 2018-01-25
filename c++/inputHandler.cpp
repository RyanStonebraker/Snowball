/*
backend.h
Snowball AI
CS 405 - Dr. Genetti
Input is read in from the JS output file, sent to the
lookup table for updates, and compressed into a double
for easy calculations later
*/

#include "board.h" //for custom board array class
#include <fstream>
using std::ifstream;
#include <string>
using std::string;
#include <exception>
using std::exception;
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;
#include <sstream>
using std::istringstream;
#include <vector>
using std::vector;

string readBoardState()
{
	string filename = "../../../comm/boardstate.txt";

	ifstream inFile(filename);

	if (!inFile)
	{
		cout << "Error opening file" << endl;
		return "BOARDSTATE.TXT FAILED TO OPEN";
		throw;
	}

	//vector<string> allBoardStates;
	/*while (true)
	{*/
		string inputLine;
		getline(inFile, inputLine);

		if (!inFile)
		{
			/*if (inFile.eof())
				break;*/

			return "ERROR READING BOARDSTATE.TXT";
			throw;
		}

		string boardState;
		istringstream iss(inputLine);
		iss >> boardState;

		//allBoardStates.push_back(boardState);
	//}

	return boardState;
}
