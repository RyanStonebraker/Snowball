#ifndef BRANCH_TRACKER_H
#define BRANCH_TRACKER_H

#include "board.h"
#include "WeightedNode.h"

// *** NOTE: REPLACE LIST WITH CUSTOM NEURON NETWORK IMPLEMENTATION ***
#include <list>

// A class that recursive/iteratively goes through every possible future move
// to a depth specified in the WeightedNode and weights various components to
// come up with a cumulative score of branch. The lowest percentage of same level
// branches will not be appended to the global neuron network.
class BranchTracker {
public:
  BranchTracker() = delete;
  BranchTracker(const Board &, WeightedNode);

  BranchTracker(const BranchTracker &);
  BranchTracker(BranchTracker &&);
  BranchTracker operator=(const BranchTracker &);
  BranchTracker operator=(BranchTracker &&);

  ~BranchTracker() = default;



  // NeuralNode cumulativeBranchWeight();

private:


  WeightedNode _branchWeightings;

  // NOTE: Replace list w/custom neuron network and type should be a neuron
  // instead of board
  std::list<Board> _localBranch;
};

#endif
