/*
backendMain.cpp
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

	//left side: 0 (SPECIAL), 8, 16, 24
	//right side: 7, 15, 23, 31
*/

#include "backend.h"
#include "board.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <random>

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

bool indexIsValid(int position)
{
	return (position <= 31 && position >= 0);
}

bool canMove(int position, int nextPosition, int piece, std::string boardString, bool isSideAttack = false)
{
	bool isSide = position == 0 || position == 8 || position == 16 || position == 24 ||
		position == 7 || position == 15 || position == 23 || position == 31;

	if (!indexIsValid(nextPosition))
		return false;

	int nextLocationState = boardString[nextPosition] - '0';

	if (isSideAttack)
		return std::abs(position - nextPosition) == 7 || std::abs(position - nextPosition) == 9;

	else if (isSide && std::abs(position - nextPosition) != 4)
		return false;

	if (piece == BLACK) //TO DO: side checks in here
	{
		return nextLocationState == 0;
	}
	else //BLACK_KING
	{

	}
		
}

std::string checkKill(int position, const board & currentBoard, std::string & visited, std::string & updateVisited)
{
	std::string nextBoard = currentBoard.getBoardStateString();

	bool isLeftSide = position == 0 || position == 8 || position == 16 || position == 24;
	bool isRightSide = position == 7 || position == 15 || position == 23 || position == 31;
	bool isSide = isLeftSide || isRightSide;

	int leftShift = ((position / 4) % 2) == 0 ? position - 5 : position - 4;
	int rightShift = ((position / 4) % 2) == 0 ? position - 4 : position - 3;
	int backLeftShift = ((position / 4) % 2) == 0 ? position + 3 : position + 4;
	int backRightShift = ((position / 4) % 2) == 0 ? position + 4 : position + 5;

	bool leftKillCondition = (indexIsValid(leftShift) ? (currentBoard[leftShift] == RED || currentBoard[leftShift] == RED_KING) : false) ?
							 (canMove(position, position - 9, currentBoard[position], nextBoard) ? (visited[position - 9] != 'x') : false) : false;

	bool rightKillCondition = (indexIsValid(rightShift) ? (currentBoard[rightShift] == RED || currentBoard[rightShift] == RED_KING) : false) ?
							  (canMove(position, position - 7, currentBoard[position], nextBoard) ? (visited[position - 7] != 'x') : false) : false;
							  

	bool backLeftKillCondition = (indexIsValid(backLeftShift) ? (currentBoard[backLeftShift] == RED || currentBoard[backLeftShift] == RED_KING) : false) ?
								 (canMove(position, position + 7, currentBoard[position], nextBoard, isSide) ? (visited[position + 7] != 'x') : false) : false;

	bool backRightKillCondition = (indexIsValid(backRightShift) ? (currentBoard[backRightShift] == RED || currentBoard[backRightShift] == RED_KING) : false) ?
								  (canMove(position, position + 9, currentBoard[position], nextBoard, isSide) ? (visited[position + 9] != 'x') : false) : false;

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

std::string workhorse(int position, const board & currentBoard, std::vector<board> & validMoves, std::string visited)
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

std::vector<board> generateRandomMoves(board currentBoard)
{
	std::vector<board> validMoves;
	std::string visited = currentBoard.getBoardStateString();

	for (int position = 0; position < currentBoard.getBoardStateString().length(); position++)
	{
		std::string nextBoard = currentBoard.getBoardStateString();
		int leftShift = ((position / 4) % 2) == 0 ? position - 5 : position - 4;
		int rightShift = ((position / 4) % 2) == 0 ? position - 4 : position - 3;

		if (currentBoard[position] == BLACK)
		{
			if (canMove(position, leftShift, currentBoard[position], nextBoard)) //can we move left?
			{
				
				std::swap(nextBoard[position], nextBoard[leftShift]);

				if (leftShift == 0 || leftShift == 1 || leftShift == 2 || leftShift == 3) //will this move make a king?
					nextBoard[leftShift] = '3';
			
				validMoves.push_back({ nextBoard });
			}
			else if(!(position == 0 || position == 8 || position == 16 || position == 24))
			{
				visited = workhorse(position, currentBoard, validMoves, visited);
			}

			if (canMove(position, rightShift, currentBoard[position], nextBoard)) //can we move right?
			{
				nextBoard = currentBoard.getBoardStateString();
				std::swap(nextBoard[position], nextBoard[rightShift]);

				if (rightShift == 0 || rightShift == 1 || rightShift == 2 || rightShift == 3) //will this move make a king?
					nextBoard[rightShift] = '3';

				validMoves.push_back({ nextBoard });
			}
			else if(!(position == 7 || position == 15 || position == 23 || position == 31))
			{
				visited = workhorse(position, currentBoard, validMoves, visited);
			}
		}

	}

	return validMoves;
}

int main()
{
	board b(readBoardState());
	std::vector<board> validMoves = generateRandomMoves(b);

	std::ofstream shadowOutFile("..\\..\\..\\comm\\shadowstate.txt");
	std::ofstream boardStateOutFile("..\\..\\..\\comm\\boardstate.txt" , std::ofstream::trunc);

	std::cout << "Black Center Move Generations: \n" << std::endl;
	for (const auto & n : validMoves)
	{
		std::cout << n << std::endl;
		shadowOutFile << n.getBoardStateString() << std::endl;
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, validMoves.size() - 1);

	int choice = dis(gen);

	std::cout << "I chose to use index: " << choice << " with move string: " << validMoves[choice].getBoardStateString() << std::endl;

	boardStateOutFile << validMoves[choice].getBoardStateString() << std::endl;

	std::cin.get();
	return 0;
}

