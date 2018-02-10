#ifndef MOVEGENERATOR_H_INCLUDED
#define MOVEGENERATOR_H_INCLUDED

#include <string>
#include <vector>
#include "board.h"

class MoveGenerator {
public:
	bool indexIsValid(int position);
	bool canMove(int position, int nextPosition, int piece, std::string boardString, bool isSideAttack = false);
	std::string checkKill(int position, const Board & currentBoard, std::string & visited, std::string & updateVisited, int & quality);
	std::string workhorse(int position, const Board & currentBoard, std::vector<Board> & validMoves, std::string visited);
	std::vector<Board> generateRandomMoves(Board currentBoard);
	bool calculateLeftKillCondition(int position, const Board & currentBoard, const std::string & nextBoard, const std::string & visited, bool isSide);
	bool calculateRightKillCondition(int position, const Board & currentBoard, const std::string & nextBoard, const std::string & visited, bool isSide);
	bool calculateBackLeftKillCondition(int position, const Board & currentBoard, const std::string & nextBoard, const std::string & visited, bool isSide);
	bool calculateBackRightKillCondition(int position, const Board & currentBoard, const std::string & nextBoard, const std::string & visited, bool isSide);
};

#endif