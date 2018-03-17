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

	if (argc >= 2) {
		currentWeights.depth = atoi(argv[1]);
	}

	if (argc >= 3) {
		if (strncmp(argv[2], "evolve", 6) == 0) {
			ChildEvolver evolver(1000, currentWeights);
		}
	}

	NeuralNetwork aiPlayer;
	aiPlayer.loadStartingWeights(currentWeights);

	// *** For Loading Generations: ***
	// aiPlayer.writeWeightsToFile("gen0.txt");
	aiPlayer.loadStartingWeightsFromFile("../comm/tempWeights.txt");

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
		std::cout << "Playing Move : " << nextMove << " with weight: " << aiPlayer.getBestWeight() << std::endl;
		printStatistics(aiPlayer, previousMovesGenerated, nextMove);

		playedFirstMove = true;
		gameController.updateFileName();
		gameController.outputNewBoardState(nextMove);

		if (aiPlayer.gameIsOver()) {
			std::cout << "Game Ended! " << aiPlayer.getWinner() << std::endl;
			break;
		}
	}
}
