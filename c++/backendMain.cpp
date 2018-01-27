﻿/*
main.cpp
Snowball AI
CS 405 - Dr. Genetti
All main functions that pertain to generating valid moves will be
called from here to keep things organized.
*/


/*
			THE BOARD

	_________________________
	|0 |  | 1|  | 2|  | 3|  |
	|  |4 |  |5 |  |6 |  |7 |
	|8 |  |9 |  |10|  |11|  |
	|  |12|  |13|  |14|  |15|
	|16|  |17|  |18|  |19|  |
	|  |20|  |21|  |22|  |23|
	|24|  |25|  |26|  |27|  |
	|  |28|  |29|  |30|  |31|
	‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾

*/

#include "backend.h"
#include "board.h"
#include <iostream>
#include <vector>

#define RED				1
#define BLACK			2
#define RED_KING		3
#define BLACK_KING		4

//std::string binaryConvert(unsigned long long int value)
//{
//	std::string output = "";
//	unsigned long long int remainder;
//
//	if (value <= 1)
//		return output;
//
//	remainder = value % 2;
//	binaryConvert(value / 2);
//	output += remainder;
//}
//
//std::string printBitString(board b)
//{
//	return binaryConvert(b.getBoardStateBits()._64a);
//}

bool canMove(int position, int nextPosition, int piece, std::string boardString)
{
	bool isSide = position == 0 || position == 8 || position == 16 || position == 24 ||
		position == 7 || position == 15 || position == 23 || position == 31;

	int nextLocationState = boardString[nextPosition] - '0';

	if (isSide || nextPosition > 31 || nextPosition < 0)
		return false;

	if (piece == BLACK) //TO DO: side checks in here
	{
		return nextLocationState == 0;
	}
		
}

std::string checkKill(int position, const board & currentBoard, std::string & visited)
{
	std::string nextBoard = currentBoard.getBoardStateString();

	bool leftKillCondition = (canMove(position, position - 9, currentBoard[position], nextBoard)) &&
							 (visited[position - 9] != 'x') &&
							 (currentBoard[position - 4] == RED ||
							 currentBoard[position - 4] == RED_KING);

	bool rightKillCondition = (canMove(position, position - 7, currentBoard[position], nextBoard)) &&
							  (visited[position - 7] != 'x') &&
							  (currentBoard[position - 3] == RED ||
							  currentBoard[position - 3] == RED_KING);

	if (!(leftKillCondition && rightKillCondition))
		visited[position] = 'x';
	

	if (leftKillCondition) //left kill condition
	{
		std::swap(nextBoard[position], nextBoard[position - 9]);
		nextBoard[position - 4] = '0';

		/*if(!rightKillCondition)
			visited[position - 9] = 'x';*/

		return checkKill(position - 9, { nextBoard }, visited);
	}

	if (rightKillCondition) //right kill condition
	{
		std::swap(nextBoard[position], nextBoard[position - 7]);
		nextBoard[position - 3] = '0';

		/*if(!leftKillCondition)
			visited[position - 7] = 'x';*/
		
		return checkKill(position - 7, { nextBoard }, visited);
	}

	return nextBoard;

}

void workhorse(int position, const board & currentBoard, std::vector<board> & validMoves, std::string visited)
{
	while (true)
	{
		std::string nextBoard = checkKill(position, currentBoard, visited);

		if (nextBoard == currentBoard.getBoardStateString())
			break;

		validMoves.push_back({ nextBoard });
	}
}

std::vector<board> generateRandomMoves(board currentBoard)
{
	//left side: 0, 8, 16, 24
	//right side: 7, 15, 23, 31

	std::vector<board> validMoves;

	for (int position = 0; position < currentBoard.getBoardStateString().length(); position++)
	{
		std::string nextBoard = currentBoard.getBoardStateString();

		if (currentBoard[position] == BLACK)
		{
			if (canMove(position, position - 4, currentBoard[position], nextBoard)) //can we move left?
			{
				std::swap(nextBoard[position], nextBoard[position - 4]);
				validMoves.push_back({ nextBoard });
			}
			else
			{
				workhorse(position, currentBoard, validMoves, currentBoard.getBoardStateString());
			}

			if (canMove(position, position - 3, currentBoard[position], nextBoard)) //can we move right?
			{
				nextBoard = currentBoard.getBoardStateString();
				std::swap(nextBoard[position], nextBoard[position - 3]);
				validMoves.push_back({ nextBoard });
			}
			else
			{
				workhorse(position, currentBoard, validMoves, currentBoard.getBoardStateString());
			}
		}

	}

	return validMoves;
}

int main()
{
	board b(readBoardState());
	std::vector<board> validMoves = generateRandomMoves(b);
	
	std::cout << "Black Center Move Generations: \n" << std::endl;
	for (const auto & n : validMoves)
	{
		std::cout << n << std::endl;
	}

	std::cin.get();
	return 0;
}
