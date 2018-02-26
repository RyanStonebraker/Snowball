/*
IOHandler.cpp
Snowball AI
CS 405 - Dr. Genetti
Input is read in from the JS output file, sent to the
lookup table for updates, and compressed into a double
for easy calculations later
*/

#include "IOHandler.h"
#include "board.h" //for custom board array class
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
#include <stdio.h>

#ifdef _WIN32
	#include <windows.h>
	#define usleep(tm) Sleep(tm * 0.001)
#endif
#ifndef _WIN32
	#include <unistd.h>
#endif

#define AVG_SLEEP_BLOCK 10


IOHandler::IOHandler (bool cleanup) : _cleanUp(cleanup) {
	this->shadowState = "../comm/shadowstate.txt";
	this->currentGameDirectory = "../comm/currentgame/";
	this->boardState = currentGameDirectory + "turn0.txt";
	this->handshake0 = "../comm/handshake0.txt";
	this->handshake1 = "../comm/handshake1.txt";
	currentFileNumber = 1;
}

IOHandler::~IOHandler() {
	if (this->_cleanUp) {
		removeDirectoryFiles();
	}
}

void IOHandler::removeDirectoryFiles() {
	if (remove(handshake0.c_str()) != 0) {
		std::cerr << "Error Removing handshake0!" << std::endl;
	}
	else if (remove(handshake1.c_str()) != 0) {
		std::cerr << "Error Removing handshake1!" << std::endl;
	}

	for (unsigned i = 0; i < currentFileNumber; ++ i) {
		std::string turnToRemove = this->currentGameDirectory + "turn" + std::to_string(i) + ".txt";
		if (remove(turnToRemove.c_str()) != 0) {
			std::cerr << "Error Removing " << turnToRemove << "!" << std::endl;
		}
	}
}

std::string IOHandler::getFirstHandshakeLocation() {
	return handshake0;
}

std::string IOHandler::getSecondHandshakeLocation() {
	return handshake1;
}

std::string IOHandler::getCurrentGameDirectory() {
	return currentGameDirectory;
}


int IOHandler::setupGame()
{
	while (true)
	{
		ifstream interfaceInput(handshake0);

		while (!interfaceInput) {
			interfaceInput = ifstream(handshake0);
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

bool IOHandler::startGame(int startingDetails)
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
			std::cout << "Failed to read handshake0" << std::endl;
			setupGame();
		}
	}

	computerOutput << '1';

	return startingDetails == 1;

}

string IOHandler::updateFileName()
{
	boardState = this->currentGameDirectory + "turn" + std::to_string(currentFileNumber) + ".txt";
	++currentFileNumber;
	return boardState;
}

int IOHandler::getCurrentFileNumber()
{
	return currentFileNumber;
}

void IOHandler::print(const Board & b)
{
	std::cout << "mUv NuM: " << currentFileNumber << std::endl;
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

string IOHandler::readBoardState()
{
	ifstream inFile(boardState);

	string inputLine;
	while(!inFile || inputLine.size() < 32)
	{
		usleep(AVG_SLEEP_BLOCK); /***DEBUG WIN***/
		inFile = ifstream(boardState);

		if (inFile) {
			getline(inFile, inputLine);
		}
	}

	if (!inFile)
	{
		std::string currentTurn = this->currentGameDirectory + "turn" + std::to_string(this->currentFileNumber) + ".txt";
		cerr << "ERROR READING: " << currentTurn << endl;
		throw;
	}

	string boardState;
	istringstream iss(inputLine);
	iss >> boardState;

	return boardState;
}

int IOHandler::outputNewBoardState(const vector<Board> & validMoves)
{
	ofstream shadowOutFile(shadowState);
	ofstream boardStateOutFile(boardState, std::ofstream::trunc);

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(0, validMoves.size() - 1);

	int choice = dis(gen);

	boardStateOutFile << validMoves[choice].getBoardStateString();

	return choice;
}
