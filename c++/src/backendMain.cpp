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

	//left side: 0, 8, 16, 24
	//right side: 7, 15, 23, 31
*/

#include "../include/backend.h"
#include "../include/board.h"
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <chrono>
using std::chrono::system_clock;
using std::chrono::duration;

vector<Board> flipColor(vector<Board> validMoves)
{
	//std::move
	std::transform(validMoves.begin(), validMoves.end(), validMoves.begin(), [](Board & b) {

		string flippedBoard = "";
		string boardString = b.getBoardStateString();
		int quality = b.getQuality();

		std::reverse(boardString.begin(), boardString.end());

		b = { boardString, quality};

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

		Board temp(flippedBoard, quality);
		return temp;

	});

	return validMoves;
}

int main()
{
	while (true)
	{
		// Board b(readBoardState());

		if (startGame(setupGame())) //computer is black and goes second
		{
			auto startGameTime = system_clock::now(); /***DEBUG***/
			double totalAverageMoveGenTimes = 0.; /***DEBUG***/

			while (true)
			{
				auto startFullTurnCycleTime = system_clock::now();

				updateFileName();
				Board b(readBoardState());

				auto startGenMoves = system_clock::now(); /***DEBUG***/

				// TODO: POPULATE VALID MOVES WITH WEIGHTED MOVES, TAKES BOARD B
				// NOTE: QUALITY IS A RANKING OF VALID KILLS AVAILABLE ON THE POTENTIAL NEXT MOVE

				vector<Board> validMoves = generateRandomMoves(b);
				std::sort(validMoves.begin(), validMoves.end(), [](const Board & a, const Board & b) { return a.getQuality() > b.getQuality(); });
				auto endGenMoves = system_clock::now(); /***DEBUG***/

				duration<double> elapsedMoveGenTime = endGenMoves - startGenMoves; /***DEBUG***/
				totalAverageMoveGenTimes += validMoves.size() / elapsedMoveGenTime.count(); /***DEBUG***/
				std::cout << "Moves generated in " << elapsedMoveGenTime.count() << " seconds" << std::endl; /***DEBUG***/
				std::cout << "Average " << validMoves.size() / elapsedMoveGenTime.count() << " moves/sec" << std::endl; /***DEBUG***/

				if (validMoves.size() == 0)
					break;

				updateFileName();
				outputNewBoardState(validMoves);

				auto endFullTurnCycleTime = system_clock::now(); /***DEBUG***/
				duration<double> elapsedTurnTime = endFullTurnCycleTime - startFullTurnCycleTime; /***DEBUG***/
				std::cout << "Turn complete in " << elapsedTurnTime.count() << " seconds" << std::endl; /***DEBUG***/

			}

			auto endGameTime = system_clock::now(); /***DEBUG***/
			duration<double> elapsedGameTime = endGameTime - startGameTime; /***DEBUG***/
			std::cout << "\nGame lasted " << elapsedGameTime.count() << " seconds" << std::endl; /***DEBUG***/
			std::cout << "Average " << getFileIncrementer() / elapsedGameTime.count() << " turns/sec" << std::endl; /***DEBUG***/
			std::cout << "Average " << totalAverageMoveGenTimes/getFileIncrementer() << " moves/sec" << std::endl; /***DEBUG***/
		}
		else //computer is red and goes first
		{
			auto startGameTime = system_clock::now(); /***DEBUG***/
			double totalAverageMoveGenTimes = 0.; /***DEBUG***/
			Board b(readBoardState());
			// int indexOfMoveChosen;

			while (true)
			{
				auto startFullTurnCycleTime = system_clock::now();

				updateFileName();

				auto startGenMoves = system_clock::now(); /***DEBUG***/

				// TODO: POPULATE VALID MOVES WITH WEIGHTED MOVES, TAKES BOARD B

				vector<Board> validMoves = generateRandomMoves(b);
				vector<Board> flippedValidMoves = flipColor(validMoves);
				std::sort(flippedValidMoves.begin(), flippedValidMoves.end(), [](const Board & a, const Board & b) { return a.getQuality() > b.getQuality(); });
				auto endGenMoves = system_clock::now(); /***DEBUG***/

				duration<double> elapsedMoveGenTime = endGenMoves - startGenMoves; /***DEBUG***/
				totalAverageMoveGenTimes += validMoves.size() / elapsedMoveGenTime.count(); /***DEBUG***/
				std::cout << "Moves generated in " << elapsedMoveGenTime.count() << " seconds" << std::endl; /***DEBUG***/
				std::cout << "Average " << validMoves.size() / elapsedMoveGenTime.count() << " moves/sec" << std::endl; /***DEBUG***/

				if (validMoves.size() == 0)
					break;

				outputNewBoardState(flippedValidMoves);

				updateFileName();

				vector <Board> flipInputBoard = {readBoardState()};
				b = flipColor(flipInputBoard)[0];

				auto endFullTurnCycleTime = system_clock::now(); /***DEBUG***/
				duration<double> elapsedTurnTime = endFullTurnCycleTime - startFullTurnCycleTime; /***DEBUG***/
				std::cout << "Turn complete in " << elapsedTurnTime.count() << " seconds" << std::endl; /***DEBUG***/
			}

			auto endGameTime = system_clock::now(); /***DEBUG***/
			duration<double> elapsedGameTime = endGameTime - startGameTime; /***DEBUG***/
			std::cout << "\nGame lasted " << elapsedGameTime.count() << " seconds" << std::endl; /***DEBUG***/
			std::cout << "Average " << getFileIncrementer() / elapsedGameTime.count() << " turns/sec" << std::endl; /***DEBUG***/
			std::cout << "Average " << totalAverageMoveGenTimes / getFileIncrementer() << " moves/sec" << std::endl; /***DEBUG***/
		}

		std::cout << "gameover" << std::endl;

		std::cin.get();
		return 0; //this is for aesthetic appeal
	}
}
