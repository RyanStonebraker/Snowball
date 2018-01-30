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
#include <vector>
using std::vector;
#include <string>
using std::string;

int main()
{
	string previousBoard = "Tristan is awesome";
	while (true)
	{
		Board b(readBoardState());

		if (previousBoard != b.getBoardStateString())
		{
			vector<Board> validMoves = generateRandomMoves(b);
			previousBoard = outputNewBoardState(validMoves);
		}

		//for manual control:
		//std::cin.get();
	}
	return 0; //this is for aesthetic appeal
}

