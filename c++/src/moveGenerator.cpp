/*
moveGenerator.cpp
Snowball AI
CS 405 - Dr. Genetti
All moves are generated from here
*/

#include "moveGenerator.h"
#include "board.h"
#include "constants.h"

bool MoveGenerator::indexIsValid(int position)
{
	return (position <= 31 && position >= 0);
}

bool MoveGenerator::canMove(int position, int nextPosition, int piece, std::string boardString, bool isSideAttack)
{
	bool isSide = position == 0 || position == 8 || position == 16 || position == 24 ||
		position == 7 || position == 15 || position == 23 || position == 31;

	if (!indexIsValid(nextPosition))
		return false;

	int nextLocationState = boardString[nextPosition] - '0';

	if (isSideAttack)
		return (std::abs(position - nextPosition) == 7 || std::abs(position - nextPosition) == 9) && nextLocationState == 0;

	else if (isSide && std::abs(position - nextPosition) != 4)
		return false;

	if (piece == BLACK || piece == BLACK_KING) 
	{
		return nextLocationState == 0;
	}

	return false;
}

std::string MoveGenerator::checkKill(int position, const Board & currentBoard, std::string & visited, std::string & updateVisited, int & quality)
{
	std::string nextBoard = currentBoard.getBoardStateString();

	bool isLeftSide = position == 0 || position == 8 || position == 16 || position == 24;
	bool isRightSide = position == 7 || position == 15 || position == 23 || position == 31;
	bool isSide = isLeftSide || isRightSide;
	bool isKing = currentBoard[position] == 3 || currentBoard[position] == 4;

	int leftShift = ((position / 4) % 2) == 0 ? position - 5 : position - 4;
	int rightShift = ((position / 4) % 2) == 0 ? position - 4 : position - 3;
	int backLeftShift = ((position / 4) % 2) == 0 ? position + 3 : position + 4;
	int backRightShift = ((position / 4) % 2) == 0 ? position + 4 : position + 5;

	bool leftKillCondition = position % 8 != 0 && position % 8 != 4 && ((indexIsValid(leftShift) ? (currentBoard[leftShift] == RED || currentBoard[leftShift] == RED_KING) : false) ?
		(canMove(position, position - 9, currentBoard[position], nextBoard, isSide) ? (visited[position - 9] != 'x') : false) : false);

	bool rightKillCondition = (position + 1) % 8 != 0 && position % 8 != 3 && ((indexIsValid(rightShift) ? (currentBoard[rightShift] == RED || currentBoard[rightShift] == RED_KING) : false) ?
		(canMove(position, position - 7, currentBoard[position], nextBoard, isSide) ? (visited[position - 7] != 'x') : false) : false);


	bool backLeftKillCondition = position % 8 != 0 && position % 8 != 4 && isKing && ((indexIsValid(backLeftShift) ? (currentBoard[backLeftShift] == RED || currentBoard[backLeftShift] == RED_KING) : false) ?
		(canMove(position, position + 7, currentBoard[position], nextBoard, isSide) ? (visited[position + 7] != 'x') : false) : false);

	bool backRightKillCondition = (position + 1) % 8 != 0 && isKing && position % 8 != 3 && ((indexIsValid(backRightShift) ? (currentBoard[backRightShift] == RED || currentBoard[backRightShift] == RED_KING) : false) ?
		(canMove(position, position + 9, currentBoard[position], nextBoard, isSide) ? (visited[position + 9] != 'x') : false) : false);

	if (!((leftKillCondition && rightKillCondition) || (leftKillCondition && backLeftKillCondition) ||
		(leftKillCondition && backRightKillCondition) || (rightKillCondition && backLeftKillCondition) ||
		(rightKillCondition && backRightKillCondition) || (backLeftKillCondition && backRightKillCondition)))
		updateVisited[position] = 'x'; //might change ***


	if (leftKillCondition) //left kill condition
	{
		std::swap(nextBoard[position], nextBoard[position - 9]);
		nextBoard[((position / 4) % 2) == 0 ? position - 5 : position - 4] = '0';
		quality++;

		return checkKill(position - 9, { nextBoard }, visited, updateVisited, quality);
	}

	if (backLeftKillCondition) //back left kill condition
	{
		std::swap(nextBoard[position], nextBoard[position + 7]);
		nextBoard[((position / 4) % 2) == 0 ? position + 3 : position + 4] = '0';
		quality++;

		return checkKill(position + 7, { nextBoard }, visited, updateVisited, quality);
	}

	if (rightKillCondition) //right kill condition
	{
		std::swap(nextBoard[position], nextBoard[position - 7]);
		nextBoard[((position / 4) % 2) == 0 ? position - 4 : position - 3] = '0';
		quality++;

		return checkKill(position - 7, { nextBoard }, visited, updateVisited, quality);
	}

	if (backRightKillCondition) //back right kill condition
	{
		std::swap(nextBoard[position], nextBoard[position + 9]);
		nextBoard[((position / 4) % 2) == 0 ? position + 4 : position + 5] = '0';
		quality++;

		return checkKill(position + 9, { nextBoard }, visited, updateVisited, quality);
	}


	return nextBoard;

}

std::string MoveGenerator::workhorse(int position, const Board & currentBoard, std::vector<Board> & validMoves, std::string visited)
{
	std::string updateVisited = visited; //might remove ***
	while (true)
	{
		int quality = 0;
		std::string nextBoard = checkKill(position, currentBoard, visited, updateVisited, quality);
		visited = updateVisited; //might remove ***

		if (nextBoard == currentBoard.getBoardStateString())
			break;

		for(int kingCheck = 0; kingCheck < 4; kingCheck++)
			if(nextBoard[kingCheck] == BLACK + '0')
				nextBoard[kingCheck] = BLACK_KING + '0';

		validMoves.push_back({ nextBoard, quality });
	}

	return visited;
}

std::vector<Board> MoveGenerator::generateRandomMoves(Board currentBoard)
{
	std::vector<Board> validMoves;
	std::string visited = currentBoard.getBoardStateString();

	for (unsigned int position = 0; position < currentBoard.getBoardStateString().length(); position++)
	{
		std::string nextBoard = currentBoard.getBoardStateString();
		int leftShift = ((position / 4) % 2) == 0 ? position - 5 : position - 4;
		int rightShift = ((position / 4) % 2) == 0 ? position - 4 : position - 3;
		int backLeftShift = ((position / 4) % 2) == 0 ? position + 3 : position + 4;
		int backRightShift = ((position / 4) % 2) == 0 ? position + 4 : position + 5;

		if (currentBoard[position] == BLACK || currentBoard[position] == BLACK_KING)
		{
			if (canMove(position, leftShift, currentBoard[position], nextBoard)) //can we move left?
			{

				std::swap(nextBoard[position], nextBoard[leftShift]);

				if (leftShift == 0 || leftShift == 1 || leftShift == 2 || leftShift == 3) //will this move make a king?
					nextBoard[leftShift] = '4';

				validMoves.push_back({ nextBoard });
			}
			else if (!(position == 0 || position == 8 || position == 16 || position == 24))
			{
				visited = workhorse(position, currentBoard, validMoves, visited);
			}

			if (canMove(position, rightShift, currentBoard[position], nextBoard)) //can we move right?
			{
				nextBoard = currentBoard.getBoardStateString();
				std::swap(nextBoard[position], nextBoard[rightShift]);

				if (rightShift == 0 || rightShift == 1 || rightShift == 2 || rightShift == 3) //will this move make a king?
					nextBoard[rightShift] = '4';

				validMoves.push_back({ nextBoard });
			}
			else if (!(position == 7 || position == 15 || position == 23 || position == 31))
			{
				visited = workhorse(position, currentBoard, validMoves, visited);
			}

			if (currentBoard[position] == BLACK_KING)
			{
				if (canMove(position, backLeftShift, currentBoard[position], nextBoard)) //can we move bottom left?
				{
					nextBoard = currentBoard.getBoardStateString();
					std::swap(nextBoard[position], nextBoard[backLeftShift]);

					validMoves.push_back({ nextBoard });
				}
				else if (!(position == 0 || position == 8 || position == 16 || position == 24))
				{
					visited = workhorse(position, currentBoard, validMoves, visited);
				}

				if (canMove(position, backRightShift, currentBoard[position], nextBoard)) //can we move bottom right?
				{
					nextBoard = currentBoard.getBoardStateString();
					std::swap(nextBoard[position], nextBoard[backRightShift]);

					validMoves.push_back({ nextBoard });
				}
				else if (!(position == 7 || position == 15 || position == 23 || position == 31))
				{
					visited = workhorse(position, currentBoard, validMoves, visited);
				}
			}
		}
	}
	return validMoves;
}
