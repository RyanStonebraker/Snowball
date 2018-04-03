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
#include "WeightedNode.h"
#include <random>
using std::random_device;
using std::mt19937;
using std::uniform_real_distribution;
#include <stdlib.h>
#include "childEvolver.h"
#include "NeuralNetwork.h"

void printStatistics(NeuralNetwork & aiPlayer, unsigned previousMovesGenerated, const std::string & nextMove) {
	auto elapsedMoveGenTime = aiPlayer.getEvaluationTime();
	auto boardsEvaluated = MoveGenerator::totalMoveGens - previousMovesGenerated;
	std::cout << "Playing Move : " << nextMove << " with weight: " << aiPlayer.getBestWeight() << std::endl;
	std::cout << "Boards Evaluated: " << boardsEvaluated << " boards" << std::endl;
	std::cout << "Time To Move: " << elapsedMoveGenTime << " seconds" << std::endl;
	std::cout << "Evaluation Speed: " << boardsEvaluated / elapsedMoveGenTime << " Boards/Sec" << std::endl;
	aiPlayer.clearEvaluationTime();
}

int main (int argc, char* argv[]) {
	auto currentWeights = NeuralNetwork::generateRandomWeights();
	string loadFromHere = "";
	auto userDefinedDepth = -1;
	if (argc >= 2) {
		userDefinedDepth = atoi(argv[1]);
	}

	if (argc >= 3) {
		if (strncmp(argv[2], "evolve", 6) == 0) {
			auto generations = 2;
			if (argc >= 4) {
				generations = atoi(argv[3]);
			}

			WeightedNode defaultWeights;
			if (userDefinedDepth > 0)
				defaultWeights.depth = userDefinedDepth;
			ChildEvolver evolver(10, defaultWeights);
			// We changed this to 10% on April 2nd 2018 in Murie on Level 3 of the back glass classrooms on a clear sunny day. :)
			evolver.setMutationRate(0.1);
			evolver.setGenerationAmount(generations);
			evolver.startGeneration();

			return 0;
		}
		else {
			loadFromHere = argv[2];
		}
	}

	NeuralNetwork aiPlayer;
	if (loadFromHere.size() > 0)
		aiPlayer.loadStartingWeightsFromFile(loadFromHere);
	else
		aiPlayer.loadStartingWeights(currentWeights);

	if (userDefinedDepth > 0)
		aiPlayer.setDepth(userDefinedDepth);

	IOHandler gameController(CLEAN_UP_TEMP_FILES);

	auto startingPlayer = gameController.setupGame();
	gameController.startGame(startingPlayer);

	aiPlayer.setMoveColor(startingPlayer);

	bool playedFirstMove = false;
	while(true) {
		if (startingPlayer == COMPUTER_BLACK || playedFirstMove)
			gameController.updateFileName();

		auto currentBoard = gameController.readBoardState();

		aiPlayer.receiveMove(currentBoard);

		auto previousMovesGenerated = MoveGenerator::totalMoveGens;

		std::cout << "Move Received: " << currentBoard << std::endl;
		auto nextMove = aiPlayer.getBestMove();
		printStatistics(aiPlayer, previousMovesGenerated, nextMove);

		if (!playedFirstMove)
			playedFirstMove = true;

		gameController.updateFileName();
		gameController.outputNewBoardState(nextMove);

		if (aiPlayer.gameIsOver()) {
			std::cout << "Game Ended! " << aiPlayer.getWinner() << std::endl;
			std::cout << "Ending Board: " << currentBoard << " : " << nextMove << std::endl;
			break;
		}
	}
}
