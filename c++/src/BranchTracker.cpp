// BranchTracker.cpp
// 2/13/2018
// Snowball AI
// Definitions for BranchTracker.h, wrapper for neuron class that checks a given
// path and only appends it to the main _head if it is within a certain %

#include "BranchTracker.h"
#include "neuron.h"
#include "board.h"

// 2 param ctor for BranchTracker, NOTE WeightedNode is passed by reference to
// allow for updating given a branches returned values
BranchTracker::BranchTracker(const Board &startBoard, WeightedNode &branchWeight)
    : _branchWeightings(branchWeight), _startBoard(startBoard) {
  // Update Neuron starting board
  _localBranch = Neuron(this->_startBoard);
}

BranchTracker::getBestMove() {
  
}
