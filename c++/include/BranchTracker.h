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

// A class that recursive/iteratively goes through every possible future move
// to a depth specified in the WeightedNode and weights various components to
// come up with a cumulative score of branch. The lowest percentage of same level
// branches will not be appended to the global neuron network.
class BranchTracker {
public:
  enum Color { RED, BLACK };

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

  Board getBestMove();

  Neuron fastForwardHead();

  // appends current board to private variables
  // double getCurrentBoardWeight();

  // recursive/iterative function to go through all children down to a depth of
  // _branchWeightings.depth
  double cumulativeBranchWeight();

// private helper functions
private:

// private members
private:
  WeightedNode _branchWeightings;

  // The below private vars could be put in the neuron type
  std::tuple<int,int> _totalKings;
  std::tuple<int,int> _totalMovesAvailable;
  std::tuple<int,int> _totalQuality;

  std::tuple<int,int> _maxKings;
  std::tuple<int,int> _maxMoves;
  std::tuple<int,int> _maxQuality;

  // temp neuron to explore a path before appending to the global _head Neuron
  Neuron _localBranch;

  Board _startBoard;
};

#endif
