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

#include "constants.h"
#include "board.h"
#include "IOHandler.h"
#include "moveGenerator.h"
#include <vector>
using std::vector;
#include <string>
using std::string;
#include <chrono>
using std::chrono::system_clock;
using std::chrono::duration;
#include "neuron.h"
#include "BranchTracker.h"
#include "WeightedNode.h"
#include <random>
using std::random_device;
using std::mt19937;
using std::uniform_real_distribution;
#include <stdlib.h>

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

// Statistics
unsigned boardCount;
double totalElapsedTime;

int main(int argc, char* argv[])
{
	IOHandler ioHandler(true);

	Neuron _head;

	random_device rdev;

	mt19937 rand_gen(rdev());

	uniform_real_distribution<> rand_weight(0, 1);

	// Random starting weights
	WeightedNode currentWeights;
	currentWeights.kingingWeight = rand_weight(rand_gen);
	currentWeights.qualityWeight = rand_weight(rand_gen);
	currentWeights.availableMovesWeight = rand_weight(rand_gen);
	currentWeights.riskFactor = rand_weight(rand_gen);
	currentWeights.enemyFactor = rand_weight(rand_gen);

	// Depth must be at least 1 so that it can see every possible move it can make
	currentWeights.depth = 2;

	if (argc == 2) {
		currentWeights.depth = atoi(argv[1]);
	}

	std::cout << "Game Start" << std::endl;
	if (ioHandler.startGame(ioHandler.setupGame())) //computer is black and goes second
	{
		auto startGameTime = system_clock::now(); /***DEBUG***/

		while (true)
		{
			auto startFullTurnCycleTime = system_clock::now();

			ioHandler.updateFileName();
			Board b(ioHandler.readBoardState());

			auto startGenMoves = system_clock::now(); /***DEBUG***/

			BranchTracker potentialBranch(b, currentWeights);
			Board nextMove = potentialBranch.getBestMove(BranchTracker::Color::BLACK_PIECE);
			vector <Board> validMoves {nextMove};

			auto endGenMoves = system_clock::now(); /***DEBUG***/

			duration<double> elapsedMoveGenTime = endGenMoves - startGenMoves; /***DEBUG***/
			double totalChildrenCount = potentialBranch.getTotalChildren();

			boardCount += totalChildrenCount;
			totalElapsedTime += elapsedMoveGenTime.count();

			std::cout << "Cumulative Boards: " << MoveGenerator::totalMoveGens << std::endl;
			std::cout << "Cumulative Elapsed Time: " << totalElapsedTime << std::endl;
			std::cout << "Running Average BPS: " << MoveGenerator::totalMoveGens / totalElapsedTime << std::endl;
			std::cout << "Moves generated in " << elapsedMoveGenTime.count() << " seconds" << std::endl; /***DEBUG***/

			if (validMoves.size() == 0)
				break;

			ioHandler.updateFileName();
			ioHandler.outputNewBoardState(validMoves);

			auto endFullTurnCycleTime = system_clock::now(); /***DEBUG***/
			duration<double> elapsedTurnTime = endFullTurnCycleTime - startFullTurnCycleTime; /***DEBUG***/
			std::cout << "Turn complete in " << elapsedTurnTime.count() << " seconds" << std::endl; /***DEBUG***/
		}

		auto endGameTime = system_clock::now(); /***DEBUG***/
		duration<double> elapsedGameTime = endGameTime - startGameTime; /***DEBUG***/
		std::cout << "\nGame lasted " << elapsedGameTime.count() << " seconds" << std::endl; /***DEBUG***/
		std::cout << "Average " << ioHandler.getCurrentFileNumber() / elapsedGameTime.count() << " turns/sec" << std::endl; /***DEBUG***/
	}
	else //computer is red and goes first
	{
		auto startGameTime = system_clock::now(); /***DEBUG***/
		Board b(ioHandler.readBoardState());

		while (true)
		{
			auto startFullTurnCycleTime = system_clock::now();

			ioHandler.updateFileName();

			auto startGenMoves = system_clock::now(); /***DEBUG***/

			BranchTracker potentialBranch(b, currentWeights);

			Board nextMove = potentialBranch.getBestMove(BranchTracker::Color::RED_PIECE);
			vector <Board> validMoves {nextMove};

			auto endGenMoves = system_clock::now(); /***DEBUG***/

			duration<double> elapsedMoveGenTime = endGenMoves - startGenMoves; /***DEBUG***/
			std::cout << "Moves generated in " << elapsedMoveGenTime.count() << " seconds" << std::endl; /***DEBUG***/
			std::cout << "Average " << validMoves.size() / elapsedMoveGenTime.count() << " moves/sec" << std::endl; /***DEBUG***/

			if (validMoves.size() == 0)
				break;

			ioHandler.outputNewBoardState(validMoves);
			ioHandler.updateFileName();

			vector <Board> flipInputBoard = {ioHandler.readBoardState()};
			b = flipColor(flipInputBoard)[0];

			auto endFullTurnCycleTime = system_clock::now(); /***DEBUG***/
			duration<double> elapsedTurnTime = endFullTurnCycleTime - startFullTurnCycleTime; /***DEBUG***/
			std::cout << "Turn complete in " << elapsedTurnTime.count() << " seconds" << std::endl; /***DEBUG***/
		}

		auto endGameTime = system_clock::now(); /***DEBUG***/
		duration<double> elapsedGameTime = endGameTime - startGameTime; /***DEBUG***/
		std::cout << "\nGame lasted " << elapsedGameTime.count() << " seconds" << std::endl; /***DEBUG***/
		std::cout << "Average " << ioHandler.getCurrentFileNumber() / elapsedGameTime.count() << " turns/sec" << std::endl; /***DEBUG***/
	}

	std::cout << "gameover" << std::endl;

	std::cin.get();
	return 0; //this is for aesthetic appeal
}
