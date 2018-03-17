// NeuralNetwork.h
// Header for the container class for all Snowball functionality

#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include "WeightedNode.h"
#include <string>
#include <vector>
#include <memory>
#include "constants.h"
#include "board.h"

struct ExpandedWeights {
  int numberOfRedKings;
  int numberOfBlackKings;

  int numberOfRedPieces;
  int numberOfBlackPieces;

  // int numberOfPreferredFormations;
};

struct NeuronLinkedList {
  std::string board;
  double weight;
  std::vector<std::shared_ptr<NeuronLinkedList>> children;
};

class NeuralNetwork {
public:
  static WeightedNode generateRandomWeights();
public:
  NeuralNetwork() : _startingColor(COMPUTER_BLACK), _bestMoveWeight(-1), _currentMove(STARTING_BOARD_STRING) {}
public:
  void loadStartingWeightsFromFile(const std::string & readLocation);
  void loadStartingWeights(WeightedNode & startWeights); // use std::move to move weighted node to NN
  void writeWeightsToFile(const std::string & writeLocation);
  void setMoveColor(int startingPlayer);
  void receiveMove(const std::string & currentMove);
  std::string getBestMove();
  double getBestWeight() const;
private:
  bool splitTie();
  double sigmoid(double weight);
  double evaluateBoard(const Board & currentState, int depthFromCurrent);
  ExpandedWeights getBoardInfo(const std::string & board);
  ExpandedWeights getBoardInfo(const Board & board);
  std::vector<Board> spawnBlack (const std::string & initBoard);
  std::vector<Board> spawnRed (const std::string & initBoard);
  void evaluateChildren(int depth);
  std::shared_ptr<NeuronLinkedList> recurseSpawning(int depth, NeuronLinkedList parent, int color);
  std::string flipBoardColor(const std::string & board);
  std::vector<Board> flipAllColor(std::vector<Board> validMoves);
private:
  int _startingColor;
  WeightedNode _weights;

  double _bestMoveWeight;
  std::string _currentMove;
  ExpandedWeights _topLevelWeights;

  std::vector<std::shared_ptr<NeuronLinkedList>> _children;
};

#endif
