// NeuralNetwork.cpp
// Cpp for the container class for all Snowball functionality
#include "NeuralNetwork.h"
#include <string>
#include <fstream>
#include <utility>
#include "constants.h"
#include "WeightedNode.h"
#include <random>
using std::random_device;
using std::mt19937;
using std::uniform_real_distribution;
#include "moveGenerator.h"
#include "board.h"

#include <iostream>

WeightedNode NeuralNetwork::generateRandomWeights() {
  random_device rdev;
  mt19937 rand_gen(rdev());

  uniform_real_distribution<> rand_weight(0, 1);

  WeightedNode currentWeights;
  currentWeights.kingingWeight = rand_weight(rand_gen);
  currentWeights.qualityWeight = rand_weight(rand_gen);
  currentWeights.availableMovesWeight = rand_weight(rand_gen);
  currentWeights.riskFactor = rand_weight(rand_gen);
  currentWeights.enemyFactor = rand_weight(rand_gen);
  currentWeights.splitTieFactor = rand_weight(rand_gen);

  return currentWeights;
}

void NeuralNetwork::loadStartingWeightsFromFile(const std::string & readLocation) {
  std::ifstream weightReader (readLocation);
  weightReader >> _weights;
}

void NeuralNetwork::loadStartingWeights(WeightedNode & startWeights) {
  _weights = std::move(startWeights);
}

void NeuralNetwork::writeWeightsToFile(const std::string & writeLocation) {
  std::ofstream weightWriter(writeLocation);
  weightWriter << _weights;
}

void NeuralNetwork::setMoveColor(int startingPlayer) {
  _startingColor = startingPlayer;
}

void NeuralNetwork::receiveMove(const std::string & currentMove) {
  _currentMove = currentMove;
}

std::vector<Board> NeuralNetwork::flipAllColor(std::vector<Board> validMoves) {
	std::transform(validMoves.begin(), validMoves.end(), validMoves.begin(), [](Board & b) {

		std::string flippedBoard = "";
		std::string boardString = b.getBoardStateString();
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

std::string NeuralNetwork::flipBoardColor(const std::string & board) {
	std::string flippedBoard = "";

	for (int i = board.size() - 1; i >= 0; --i) {
    auto piece = board[i] - '0';

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

	return flippedBoard;
}

// TODO: Compare board at depth X with best board at depth X, if weight < best board, STOP

double NeuralNetwork::evaluateBoard(const Board & futureState, int depthToLimit) {
  auto futureBoardInfo = getBoardInfo(futureState);
  auto computerIsBlack =  _startingColor == COMPUTER_BLACK;

  auto redPiecesTaken = _topLevelWeights.numberOfRedPieces - futureBoardInfo.numberOfRedPieces;
  auto blackPiecesTaken = _topLevelWeights.numberOfBlackPieces - futureBoardInfo.numberOfBlackPieces;

  auto redKingsMade = futureBoardInfo.numberOfRedKings - _topLevelWeights.numberOfRedKings;
  auto blackKingsMade = futureBoardInfo.numberOfBlackKings - _topLevelWeights.numberOfBlackKings;

  auto enemyPiecesTaken = (computerIsBlack) ? redPiecesTaken : blackPiecesTaken;
  auto piecesLost = (computerIsBlack) ? blackPiecesTaken : redPiecesTaken;

  auto enemyKingsMade = (computerIsBlack) ? redKingsMade : blackKingsMade;
  auto kingsMade = (computerIsBlack) ? blackKingsMade : redKingsMade;

  auto pieceCaptureWeight = (enemyPiecesTaken - piecesLost) * _weights.qualityWeight;
  auto kingWeight = (kingsMade - enemyKingsMade) * _weights.kingingWeight;

  auto depthWeight = depthToLimit * _weights.depthWeight;

  auto raw_weight = pieceCaptureWeight + kingWeight + depthWeight;

  return raw_weight;
}

ExpandedWeights NeuralNetwork::getBoardInfo(const Board & board) {
  ExpandedWeights boardWeighting;
  boardWeighting.numberOfRedKings = board.getRedKingCount();
  boardWeighting.numberOfBlackKings = board.getBlackKingCount();
  boardWeighting.numberOfRedPieces = board.getRedPieceCount();
  boardWeighting.numberOfBlackPieces = board.getBlackPieceCount();
  return boardWeighting;
}

ExpandedWeights NeuralNetwork::getBoardInfo(const std::string & minimalBoard) {
  Board board {minimalBoard};
  ExpandedWeights boardWeighting;
  boardWeighting.numberOfRedKings = board.getRedKingCount();
  boardWeighting.numberOfBlackKings = board.getBlackKingCount();
  boardWeighting.numberOfRedPieces = board.getRedPieceCount();
  boardWeighting.numberOfBlackPieces = board.getBlackPieceCount();
  return boardWeighting;
}

bool NeuralNetwork::checkIfGameIsEnded(const std::string & currentMove) const {
  auto blackWon = currentMove.find("1") == std::string::npos && currentMove.find("3") == std::string::npos;
  if (blackWon)
    return true;

  auto redWon = currentMove.find("2") == std::string::npos && currentMove.find("4") == std::string::npos;
  if (redWon)
    return true;

  return false;
}

void NeuralNetwork::setEndCondition(const std::string & currentMove) {
  Board endBoard {currentMove};
  if (endBoard.getBlackPieceCount() == 0) {
    _gameState = RED_WON;
  }
  else if (endBoard.getRedPieceCount() == 0) {
    _gameState = BLACK_WON;
  }
  else {
    _gameState = STALEMATE;
  }
}

void NeuralNetwork::evaluateChildren(int depth) {
  auto firstMoveSet = (_startingColor == COMPUTER_BLACK) ? spawnBlack(_currentMove) : spawnRed(_currentMove);

  if (firstMoveSet.size() == 0) {
    setEndCondition(_currentMove);
    return;
  }

  _topLevelWeights = getBoardInfo(_currentMove);

  auto enemyPlayedMove = _currentMove;

  for (unsigned possibleMove = 0; possibleMove < firstMoveSet.size(); ++possibleMove) {
    NeuronLinkedList node;
    node.board = firstMoveSet[possibleMove].getBoardStateString();
    _children.push_back(std::make_shared<NeuronLinkedList>(node));

    _children[possibleMove] = recurseSpawning(depth - 1, node, _startingColor);
    _children[possibleMove]->weight = sigmoid(_children[possibleMove]->weight);

    // std::cout << "Potential Move " << possibleMove << ": " << node.board << " Weight: " << _children[possibleMove]->weight << std::endl;

    if (_children[possibleMove]->weight >= _bestMoveWeight) {
      _bestMoveWeight = _children[possibleMove]->weight;
      _currentMove = _children[possibleMove]->board;
      if (_children[possibleMove]->weight == _bestMoveWeight && _currentMove != enemyPlayedMove) {
        if (_children[possibleMove]->board != _currentMove) {
          _currentMove = (splitTie()) ? _currentMove : _children[possibleMove]->board;

          if (checkIfGameIsEnded(_currentMove))
            setEndCondition(_currentMove);
        }
      }
    }
  }
}

bool NeuralNetwork::splitTie() {
  return false;
  random_device rdev;
  mt19937 rand_gen(rdev());

  uniform_real_distribution<double> randomChoice(0, 1);
  auto chosenSide = randomChoice(rand_gen);
  if (chosenSide >= _weights.splitTieFactor) {
    return false;
  }
  else
    return true;
}

// TODO: could do alpha-beta pruning by adding a "bestBoard at this level" parameter and
// then only continuing if its >= to the child
// OR, could just say if <= depthLeft X (could assign a percentage of the way to depth) and weight is
// greater than 1 standard deviation less than bestMove, then end
std::shared_ptr<NeuronLinkedList> NeuralNetwork::recurseSpawning(int depth, NeuronLinkedList parent, int color) {
  auto nextMoveSet = (color == COMPUTER_BLACK) ? spawnRed(parent.board) : spawnBlack(parent.board);
  auto nextColor = (color == COMPUTER_BLACK) ? COMPUTER_RED : COMPUTER_BLACK;

  for (auto & nextMove : nextMoveSet) {
    NeuronLinkedList nextChild;
    nextChild.board = nextMove.getBoardStateString();

    nextChild.weight = evaluateBoard(nextMove, depth);
    parent.weight += nextChild.weight;

    parent.children.push_back(std::make_shared<NeuronLinkedList>(nextChild));
    if (depth > 0) {

      // ALPHA-BETA PRUNING - More of a heuristic since doesn't DEFINITELY guarantee this
      // move cant be the best. Also, using sigmoid this much hurts.
      // auto startAlphaBetaDepth = 0.5;
      // auto cutOffPercentage = 0.5;
      // if (depth <= int(startAlphaBetaDepth * _weights.depth) && sigmoid(parent.weight) <= cutOffPercentage * _bestMoveWeight)
      //   break;

      parent.children[parent.children.size() - 1] = recurseSpawning(depth - 1, nextChild, nextColor);
    }
  }
  parent.weight /= nextMoveSet.size();
  return std::make_shared<NeuronLinkedList>(parent);
}

double NeuralNetwork::sigmoid(double weight) {
  return tanh(weight);
}

std::vector<Board> NeuralNetwork::spawnBlack (const std::string & initBoard) {
		MoveGenerator moveGen;
		return moveGen.generateRandomMoves({initBoard});
}

// Red spawning is a little confusing. First, the board is "flipped" (i.e. the string is reversed).
// Then, all reds and blacks swap coloring. This way, moves are still generated on the bottom of
// the board for black, but these moves are actually red moves so before spawnRed returns, it
// "flips" the board back to its original state.
std::vector<Board> NeuralNetwork::spawnRed (const std::string & initBoard) {
		MoveGenerator moveGen;
    Board flippedInitBoard = flipBoardColor(initBoard);
		auto flippedBoardMoves = moveGen.generateRandomMoves(flippedInitBoard);
    return flipAllColor(flippedBoardMoves);
}


std::string NeuralNetwork::getBestMove() {
  _bestMoveWeight = -1;
  _children.clear();

  auto startMoveEval = std::chrono::system_clock::now();
  evaluateChildren(_weights.depth);
  auto endMoveEval = std::chrono::system_clock::now();
  _bpsTiming += endMoveEval - startMoveEval;

  return _currentMove;
}

double NeuralNetwork::getBestWeight() const {
  return _bestMoveWeight;
}

bool NeuralNetwork::gameIsOver() {
  if (checkIfGameIsEnded(_currentMove))
    setEndCondition(_currentMove);
  return _gameState != 0;
}

std::string NeuralNetwork::getWinner() const {
  if (_gameState == STALEMATE)
    return "Draw!";
  else
    return (_gameState == RED_WON) ? "RED WON!" : "BLACK WON!";
}

double NeuralNetwork::getEvaluationTime() const {
  return _bpsTiming.count();
}

void NeuralNetwork::clearEvaluationTime() {
  _bpsTiming = std::chrono::duration<double>::zero();
}
