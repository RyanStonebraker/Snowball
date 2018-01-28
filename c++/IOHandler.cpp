/*
IOHandler.cpp
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
#include <random>
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;


string shadowState = "..\\..\\..\\comm\\shadowstate.txt";
string boardState = "..\\..\\..\\comm\\boardstate.txt";

string readBoardState()
{
	ifstream inFile(boardState);

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

std::string outputNewBoardState(const vector<board> & validMoves)
{
	std::ofstream shadowOutFile(shadowState);
	std::ofstream boardStateOutFile(boardState, std::ofstream::trunc);

	std::cout << "Black Center Move Generations: \n" << std::endl;
	for (const auto & n : validMoves)
	{
		std::cout << n << std::endl;
		shadowOutFile << n.getBoardStateString() << std::endl;
	}

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(0, validMoves.size() - 1);

	int choice = dis(gen);

	std::cout << "I chose to use index: " << choice << " with move string: " << validMoves[choice].getBoardStateString() << std::endl;

	boardStateOutFile << validMoves[choice].getBoardStateString() << std::endl;
	
	return validMoves[choice].getBoardStateString();
}
