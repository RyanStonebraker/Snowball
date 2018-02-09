#ifndef IOHANDLER_H_INCLUDED
#define IOHANDLER_H_INCLUDED

#include <string>
#include "board.h"

class IOHandler {
public:
	std::string readBoardState();
	int outputNewBoardState(const std::vector<Board> & validMoves);
	std::string updateFileName();
	int setupGame();
	bool startGame(int startingDetails);
	std::ifstream test(std::string pathName);
	void print(const Board & b);
	int getFileIncrementer();
private:
	int fileIncrementer = 1;
};

#endif