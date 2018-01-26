/*
main.cpp
Snowball AI
CS 405 - Dr. Genetti
All main functions that pertain to generating valid moves will be
called from here to keep things organized.
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

	if (isSide)
		return false;

	if (piece == RED) //TO DO: side checks in here
		return (boardString[nextPosition]-'0') == 0;
		
}

std::vector<board> generateRandomMoves(board currentBoard)
{
	//left side: 0, 8, 16, 24
	//right side: 7, 15, 23, 31

	std::vector<board> validMoves;

	for (int i = 0; i < currentBoard.getBoardStateString().length(); i++)
	{
		std::string nextBoard = currentBoard.getBoardStateString();

		if ((nextBoard[i]-'0') == RED)
		{
			if (canMove(i, i + 4, currentBoard[i], nextBoard)) //can we move left?
			{
				std::swap(nextBoard[i], nextBoard[i + 4]);
				board temp(nextBoard);
				validMoves.push_back(temp);
			}

			if (canMove(i, i + 5, currentBoard[i], nextBoard)) //can we move left?
			{
				nextBoard = currentBoard.getBoardStateString();
				std::swap(nextBoard[i], nextBoard[i + 5]);
				board temp(nextBoard);
				validMoves.push_back(temp);
			}
		}

	}

	return validMoves;
}

int main()
{
	board b(readBoardState());
	std::vector<board> validMoves = generateRandomMoves(b);
	
	for (const auto & n : validMoves)
	{
		std::cout << n << std::endl;
	}

	std::cin.get();
	return 0;
}

