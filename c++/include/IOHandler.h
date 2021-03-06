#ifndef IOHANDLER_H_INCLUDED
#define IOHANDLER_H_INCLUDED

#define CLEAN_UP_TEMP_FILES 1
#define AVG_SLEEP_BLOCK 10

#include <string>
#include "board.h"

class IOHandler {
public:
	IOHandler(bool cleanup = false);
	~IOHandler();

	std::string readBoardState();
	int outputNewBoardState(const std::vector<Board> & validMoves);
	void outputNewBoardState(const std::string & nextMove);

	std::string updateFileName();
	int setupGame();
	bool startGame(int startingDetails);
	void print(const Board & b);
	int getCurrentFileNumber();

	std::string getFirstHandshakeLocation();
	std::string getSecondHandshakeLocation();
	std::string getCurrentGameDirectory();

private:
	void removeDirectoryFiles();

	std::string shadowState;
	std::string boardState;
	std::string handshake0;
	std::string handshake1;
	std::string currentGameDirectory;
	int currentFileNumber;
	bool _cleanUp;
};

#endif
