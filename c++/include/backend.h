/*
backend.h
Snowball AI
CS 405 - Dr. Genetti
Contains function declarations for .cpp files
*/


#ifndef FILE_BACKEND_H_INCLUDED
#define FILE_BACKEND_H_INCLUDED

#define RED				1
#define BLACK			2
#define RED_KING		3
#define BLACK_KING		4

#include <string>
#include "board.h"

//IOHandler.cpp
std::string readBoardState();
int outputNewBoardState(const std::vector<Board> & validMoves);
std::string updateFileName();
int setupGame();
bool startGame(int startingDetails);

//moveGenerator.cpp
bool indexIsValid(int position);
bool canMove(int position, int nextPosition, int piece, std::string boardString, bool isSideAttack = false);
std::string checkKill(int position, const Board & currentBoard, std::string & visited, std::string & updateVisited);
std::string workhorse(int position, const Board & currentBoard, std::vector<Board> & validMoves, std::string visited);
std::vector<Board> generateRandomMoves(Board currentBoard);

#endif // !FILE_BACKEND_H_INCLUDED