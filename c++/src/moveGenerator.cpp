/*
moveGenerator.cpp
Snowball AI
CS 405 - Dr. Genetti
All moves are generated from here
*/

#include "../include/backend.h"
#include "../include/board.h"

bool indexIsValid(int position)
{
	return (position <= 31 && position >= 0);
}

bool canMove(int position, int nextPosition, int piece, std::string boardString, bool isSideAttack)
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

	if (piece == BLACK || piece == BLACK_KING) //TO DO: side checks in here
	{
		return nextLocationState == 0;
	}
	
	return false;
}

std::string checkKill(int position, const Board & currentBoard, std::string & visited, std::string & updateVisited)
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

	bool leftKillCondition = position%8 != 0 && ((indexIsValid(leftShift) ? (currentBoard[leftShift] == RED || currentBoard[leftShift] == RED_KING) : false) ?
		(canMove(position, position - 9, currentBoard[position], nextBoard, isSide) ? (visited[position - 9] != 'x') : false) : false);

	bool rightKillCondition = position+1 % 8 != 0 && ((indexIsValid(rightShift) ? (currentBoard[rightShift] == RED || currentBoard[rightShift] == RED_KING) : false) ?
		(canMove(position, position - 7, currentBoard[position], nextBoard, isSide) ? (visited[position - 7] != 'x') : false) : false);


	bool backLeftKillCondition = position % 8 != 0 && isKing && ((indexIsValid(backLeftShift) ? (currentBoard[backLeftShift] == RED || currentBoard[backLeftShift] == RED_KING) : false) ?
		(canMove(position, position + 7, currentBoard[position], nextBoard, isSide) ? (visited[position + 7] != 'x') : false) : false);

	bool backRightKillCondition = position + 1 % 8 != 0 && isKing && ((indexIsValid(backRightShift) ? (currentBoard[backRightShift] == RED || currentBoard[backRightShift] == RED_KING) : false) ?
		(canMove(position, position + 9, currentBoard[position], nextBoard, isSide) ? (visited[position + 9] != 'x') : false) : false);

	if (!((leftKillCondition && rightKillCondition) || (leftKillCondition && backLeftKillCondition) ||
		(leftKillCondition && backRightKillCondition) || (rightKillCondition && backLeftKillCondition) ||
		(rightKillCondition && backRightKillCondition) || (backLeftKillCondition && backRightKillCondition)))
		updateVisited[position] = 'x'; //might change ***


	if (leftKillCondition) //left kill condition
	{
		std::swap(nextBoard[position], nextBoard[position - 9]);
		nextBoard[((position / 4) % 2) == 0 ? position - 5 : position - 4] = '0';

		return checkKill(position - 9, { nextBoard }, visited, updateVisited);
	}

	if (backLeftKillCondition) //back left kill condition
	{
		std::swap(nextBoard[position], nextBoard[position + 7]);
		nextBoard[((position / 4) % 2) == 0 ? position + 3 : position + 4] = '0';

		return checkKill(position + 7, { nextBoard }, visited, updateVisited);
	}

	if (rightKillCondition) //right kill condition
	{
		std::swap(nextBoard[position], nextBoard[position - 7]);
		nextBoard[((position / 4) % 2) == 0 ? position - 4 : position - 3] = '0';

		return checkKill(position - 7, { nextBoard }, visited, updateVisited);
	}

	if (backRightKillCondition) //back right kill condition
	{
		std::swap(nextBoard[position], nextBoard[position + 9]);
		nextBoard[((position / 4) % 2) == 0 ? position + 4 : position + 5] = '0';

		return checkKill(position + 9, { nextBoard }, visited, updateVisited);
	}


	return nextBoard;

}

std::string workhorse(int position, const Board & currentBoard, std::vector<Board> & validMoves, std::string visited)
{
	std::string updateVisited = visited; //might remove ***
	while (true)
	{
		std::string nextBoard = checkKill(position, currentBoard, visited, updateVisited);
		visited = updateVisited; //might remove ***

		if (nextBoard == currentBoard.getBoardStateString())
			break;

		validMoves.push_back({ nextBoard });
	}

	return visited;
}

std::vector<Board> generateRandomMoves(Board currentBoard)
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