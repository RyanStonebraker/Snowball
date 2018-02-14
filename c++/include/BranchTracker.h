#ifndef BRANCH_TRACKER_H
#define BRANCH_TRACKER_H

#include "board.h"
#include "WeightedNode.h"

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
  BranchTracker(const Board &, WeightedNode);

  BranchTracker(const BranchTracker &);
  BranchTracker(BranchTracker &&);
  BranchTracker operator=(const BranchTracker &);
  BranchTracker operator=(BranchTracker &&);

  ~BranchTracker() = default;

  double getKingingChange();
  double getQuality();

  // appends current board to private variables
  double getCurrentBoardWeight();

  // recursive/iterative function to go through all children down to a depth of
  // _branchWeightings.depth
  // NeuralNode cumulativeBranchWeight();

private:
  WeightedNode _branchWeightings;

  // The below private vars could be replaced by a neuron type
  std::tuple<int,int> _totalKings;
  std::tuple<int,int> _totalMovesAvailable;
  std::tuple<int,int> _totalQuality;

  std::tuple<int,int> _maxKings;
  std::tuple<int,int> _maxMoves;
  std::tuple<int,int> _maxQuality;

  // NOTE: Replace list w/custom neuron network and type should be a neuron
  // instead of board
  std::list<Board> _localBranch;
};

#endif
