/*
IOHandler.cpp
Snowball AI
CS 405 - Dr. Genetti
Input is read in from the JS output file, sent to the
lookup table for updates, and compressed into a double
for easy calculations later
*/

#include "../include/board.h" //for custom board array class
#include <fstream>
using std::ifstream;
using std::ofstream;
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


string shadowState = "../comm/shadowstate.txt";
string boardState = "../comm/currentgame/turn0.txt";
string handshake0 = "../comm/handshake0.txt";
string handshake1 = "../comm/handshake1.txt";

std::ifstream test(string pathName) {
	ifstream idk(pathName);
	return idk;
}

int setupGame()
{
	while (true)
	{
		ifstream interfaceInput(handshake0);

		while (!interfaceInput) {
			interfaceInput = test(handshake0);
		}

		ofstream computerOutput(handshake1, std::ofstream::trunc);

		string handshake0;
		getline(interfaceInput, handshake0);

		if (!interfaceInput)
		{
			computerOutput << '0';

			if (!computerOutput) //WIP
				throw;

			continue;
		}

		istringstream iss(handshake0);

		int startingDetails;

		iss >> startingDetails;

		if (!iss)
		{
			computerOutput << '0';

			if (!computerOutput) //WIP
				throw;

			continue;
		}
		return startingDetails;
	}
}

bool startGame(int startingDetails)
{
	ofstream computerOutput(handshake1, std::ofstream::trunc);

	if (!computerOutput) //WIP
		throw;

	//(startingDetails == 0) //go first as red
	//(startingDetails == 1) //go second as black

	if(startingDetails != 0 && startingDetails != 1)
	{
		computerOutput << '0';

		if (!computerOutput) //WIP
		{
			std::cout << "Failed to read Handshake0" << std::endl;
			setupGame();
		}
	}

	computerOutput << '1';

	return startingDetails == 1;

}
static int fileIncrementer = 1;
string updateFileName()
{
	const string beginningPath = "../comm/currentgame/";
	// static int fileIncrementer = 1;
	boardState = beginningPath + "turn" + std::to_string(fileIncrementer) + ".txt";
	fileIncrementer++;
	return boardState;
}

void print(const Board & b)
{
	std::cout << "mUv NuM: " << fileIncrementer << std::endl;
	std::cout << "_________________________" << std::endl;

	for (int k = 0; k < 8; k++)
	{
		for (int i = 0; i < 4; i++)
		{
			if (k % 2 == 0)
				std::cout << "|" << b[4 * k + i] /*4 * k + i*/ << " |  ";
			else
				std::cout << "|  |" << b[4 * k + i] /*4 * k + i*/ << " ";
		}
		std::cout << "|" << std::endl;
	}

	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
}

string readBoardState()
{
	ifstream inFile(boardState);

	string inputLine;
	while(!inFile || inputLine.size() < 32)
	{
		inFile = test(boardState);

		if (inFile) {
			getline(inFile, inputLine);
		}
		// ifstream inFile(boardState);
		/*cout << "Error opening file" << endl;
		return "BOARDSTATE.TXT FAILED TO OPEN";
		throw;*/
		// cerr << "BOARDSTATE.TXT FAILED TO OPEN" << endl;
		// throw;
	}

	//vector<string> allBoardStates;
	/*while (true)
	{*/
		// string inputLine;
		// getline(inFile, inputLine);

		if (!inFile)
		{
			// getline(inFile, inputLine);
			/*if (inFile.eof())
				break;*/

			cerr << "ERROR READING TURN-SOMETHING.TXT" << endl;
			throw;
		}

		string boardState;
		istringstream iss(inputLine);
		iss >> boardState;

		//allBoardStates.push_back(boardState);
	//}

		/***DEBUG***/
		// std::cout << "RECEIVED BOARD: " << boardState << std::endl;
		// std::cout << "INTERPRETATION: \n";
		// print({ boardState });

	return boardState;
}

int outputNewBoardState(const vector<Board> & validMoves)
{
	ofstream shadowOutFile(shadowState);
	ofstream boardStateOutFile(boardState, std::ofstream::trunc);

	// std::cout << "Black Center Move Generations: \n" << std::endl;
	// for (auto & n : validMoves)
	// {
	// 	std::cout << n << std::endl;
	// 	print(n);
	// 	shadowOutFile << n.getBoardStateString() << std::endl;
	// }

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(0, validMoves.size() - 1);

	int choice = dis(gen);

	// std::cout << "INDEX USED: " << choice << "\nSENT BOARD: " << validMoves[choice].getBoardStateString() << std::endl;
	// std::cout << "INTERPRETATION: \n";
	// print(validMoves[choice]);

	boardStateOutFile << validMoves[choice].getBoardStateString();

	return choice;
}
