// BranchTracker.h
// 2/13/2018
// Snowball AI
// Wrapper for neuron class that checks a given path and only appends it to the
// main _head if it is within a certain %

#ifndef BRANCH_TRACKER_H
#define BRANCH_TRACKER_H

#include "board.h"
#include "WeightedNode.h"
#include "neuron.h"

// *** NOTE: REPLACE LIST WITH CUSTOM NEURON NETWORK IMPLEMENTATION ***
#include <list>
#include <tuple>

struct NodeFactors {
  std::tuple<size_t,size_t> totalKings;
  std::tuple<size_t,size_t> totalPieceCount;
  std::tuple<size_t,size_t> totalQuality;
  unsigned int childrenAmount;
};

// A class that recursive/iteratively goes through every possible future move
// to a depth specified in the WeightedNode and weights various components to
// come up with a cumulative score of branch. The lowest percentage of same level
// branches will not be appended to the global neuron network.
class BranchTracker {
public:
  enum Color { RED_PIECE, BLACK_PIECE };

  BranchTracker() = delete;
  BranchTracker(const Board &, WeightedNode&);

  // TODO: Fill these in if they become necessary
  // BranchTracker(const BranchTracker &);
  // BranchTracker(BranchTracker &&);
  // BranchTracker operator=(const BranchTracker &);
  // BranchTracker operator=(BranchTracker &&);

  ~BranchTracker() = default;

  // double getKingingChange();
  // double getQuality();

  // gets the best move by calling a recursive traversal function.
  // takes an enumerated Color type (RED_PIECE/BLACK_PIECE)
  Board getBestMove(Color);

  Neuron fastForwardHead();

  // appends current board to private variables
  // double getCurrentBoardWeight();

// private helper functions
private:
  // recursive/iterative function to go through all children down to a depth of
  // _branchWeightings.depth
  NodeFactors _cumulativeBranchWeight(Neuron, unsigned);

  float _sigmoidNormalizer(double);

  // Gets the weight of the branch for given color before sigmoid function
  double raw_weighting(NodeFactors, const Color);

// private members
private:
  WeightedNode _branchWeightings;

  // The below private vars could be put in the neuron type
  std::tuple<int,int> _totalKings;
  std::tuple<int,int> _totalPieceCount;
  std::tuple<int,int> _totalQuality;

  // TODO: Do something with max counts and risk factor later...
  // std::tuple<int,int> _maxKings;
  // std::tuple<int,int> _maxMoves;
  // std::tuple<int,int> _maxQuality;

  unsigned int _childrenAmount;

  float _weight;

  // temp neuron to explore a path before appending to the global _head Neuron
  Neuron _localBranch;

  Neuron _bestMoveNode;

  Board _startBoard;
};

#endif
