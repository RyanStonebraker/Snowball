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

#include "../include/backend.h"
#include "../include/board.h"
#include <vector>
using std::vector;
#include <string>
using std::string;

vector<Board> flipColor(vector<Board> validMoves)
{
	//std::move
	std::transform(validMoves.begin(), validMoves.end(), validMoves.begin(), [](Board & b) {
		
		string flippedBoard = "";
		string boardString = b.getBoardStateString();
		
		std::reverse(boardString.begin(), boardString.end());

		b = { boardString };

		for (auto & n : b.getBoardStateString())
		{
			int piece = n - '0';

			if (piece == BLACK)
				flippedBoard += '1';
			else if (piece == RED)
				flippedBoard += '2';
			else if (piece == BLACK_KING)
				flippedBoard += '3';
			else if (piece == RED_KING)
				flippedBoard += '4';
			else
				flippedBoard += '0';
		}

		Board temp(flippedBoard);
		return temp;

	});

	return validMoves;
}

int main()
{
<<<<<<< HEAD
=======
	string previousBoard = "Tristan is awesome";
	while (true)
	{
		Board b(readBoardState());
>>>>>>> 7646212e63d48f9143fec26f4aecbf944164731a

	if (startGame(setupGame())) //computer is black and goes second
	{
		while (true)
		{
<<<<<<< HEAD
			//updateFileName();
			Board b(readBoardState());

			//for manual control:
			if (std::cin.get())
				std::cout << "Next Turn" << std::endl;

			vector<Board> validMoves = generateRandomMoves(b);

			if (validMoves.size() == 0)
				break;

			updateFileName();
			outputNewBoardState(validMoves);
=======
			vector<Board> validMoves = generateRandomMoves(b);
			previousBoard = outputNewBoardState(validMoves);
>>>>>>> 7646212e63d48f9143fec26f4aecbf944164731a
		}
	}
	else //computer is red and goes first
	{
		Board b(readBoardState());
		int indexOfMoveChosen;

		while (true)
		{
			updateFileName();
			//for manual control:

			if (std::cin.get())
				std::cout << "Next Turn" << std::endl;

			vector<Board> validMoves = generateRandomMoves(b);

			if (validMoves.size() == 0)
				break;

			vector<Board> flippedValidMoves = flipColor(validMoves);
			indexOfMoveChosen = outputNewBoardState(flippedValidMoves);
			b = validMoves[indexOfMoveChosen];
			//updateFileName();
		}
	}

	std::cout << "gameover" << std::endl;

	std::cin.get();
	return 0; //this is for aesthetic appeal
}

