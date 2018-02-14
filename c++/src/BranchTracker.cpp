// BranchTracker.cpp
// 2/13/2018
// Snowball AI
// Definitions for BranchTracker.h, wrapper for neuron class that checks a given
// path and only appends it to the main _head if it is within a certain %

#include "BranchTracker.h"
#include "neuron.h"
#include "board.h"
#include <math.h>

// 2 param ctor for BranchTracker, NOTE WeightedNode is passed by reference to
// allow for updating given a branches returned values
BranchTracker::BranchTracker(const Board &startBoard, WeightedNode &branchWeight)
    : _branchWeightings(branchWeight), _startBoard(startBoard), _childrenAmount(0) {
  // Update Neuron starting board
  _localBranch = Neuron(this->_startBoard);
}

Board BranchTracker::getBestMove(BranchTracker::Color pieceColor) {
  this->_localBranch.spawnChildren(this->_branchWeightings.depth);

  // update local info with best cumulative info
  for (unsigned int child = 0; child < this->_localBranch.childrenSize(); ++child) {
    NodeFactors firstChildFactor = _cumulativeBranchWeight(this->_localBranch[child], this->_branchWeightings.depth);
    float childWeight = _sigmoidNormalizer(raw_weighting(firstChildFactor, pieceColor));
    if (childWeight > this->_weight) {
      this->_bestMoveNode = this->_localBranch[child];

      this->_totalKings = firstChildFactor.totalKings;
      this->_totalMovesAvailable = firstChildFactor.totalMovesAvailable;
      this->_totalQuality = firstChildFactor.totalQuality;

      this->_weight = childWeight;
    }
  }

  return this->_bestMoveNode.getBoard();
}

float BranchTracker::_sigmoidNormalizer (double raw_weight) {
  return tanh(raw_weight);
}

double BranchTracker::raw_weighting(NodeFactors factors, BranchTracker::Color pieceColor) {
  double raw_total = 0;

  Color enemyColor = (pieceColor == RED_PIECE) ? BLACK_PIECE : RED_PIECE;

  double kingFactor = std::get<pieceColor>(this->_totalKings) / this->_childrenAmount * _branchWeightings.kingingWeight;
  double freedomFactor = std::get<pieceColor>(this->_totalMovesAvailable) / this->_childrenAmount * _branchWeightings.availableMovesWeight;
  double qualityFactor = std::get<pieceColor>(this->_totalQuality) / this->_childrenAmount * _branchWeightings.qualityWeight;

  double enemyKingFactor = std::get<enemyColor>(this->_totalKings) / this->_childrenAmount * _branchWeightings.kingingWeight * _branchWeightings.enemyFactor;
  double enemyFreedomFactor = std::get<enemyColor>(this->_totalMovesAvailable) / this->_childrenAmount * _branchWeightings.availableMovesWeight * _branchWeightings.enemyFactor;
  double enemyQualityFactor = std::get<enemyColor>(this->_totalQuality) / this->_childrenAmount * _branchWeightings.qualityWeight * _branchWeightings.enemyFactor;

  raw_total = kingFactor + freedomFactor + qualityFactor - enemyKingFactor - enemyFreedomFactor - enemyQualityFactor;
  return raw_total;
}

NodeFactors BranchTracker::_cumulativeBranchWeight(Neuron temp_head, unsigned int depthLimit) {
  std::tuple<int,int> tempTotalKings;
  std::tuple<int,int> tempTotalMovesAvailable;
  std::tuple<int,int> tempTotalQuality;

  NodeFactors tempFactors;

  unsigned int tempNumChildren = 0;

    // Loop through every child of temp_head
  for (unsigned int child = 0; child < temp_head.childrenSize(); ++child) {
    std::get<RED_PIECE>(tempTotalKings) += temp_head[child].getRedKings();
    std::get<BLACK_PIECE>(tempTotalKings) += temp_head[child].getBlackKings();

    std::get<RED_PIECE>(tempTotalMovesAvailable) += temp_head[child].getRedMovesAvailable();
    std::get<BLACK_PIECE>(tempTotalMovesAvailable) += temp_head[child].getBlackMovesAvailable();

    std::get<RED_PIECE>(tempTotalQuality) += temp_head[child].getRedQuality();
    std::get<BLACK_PIECE>(tempTotalQuality) += temp_head[child].getBlackQuality();

    tempFactors.totalKings = tempTotalKings;
    tempFactors.totalMovesAvailable = tempTotalMovesAvailable;
    tempFactors.totalQuality = tempTotalQuality;

    // if child has children and haven't reached our depth limit, then recursive case
    if (temp_head[child].childrenSize() > 0 && depthLimit > 0) {
      NodeFactors childFactors = _cumulativeBranchWeight(temp_head[child], depthLimit - 1);

      tempFactors.totalKings += childFactors.totalKings;
      tempFactors.totalMovesAvailable += childFactors.totalMovesAvailable;
      tempFactors.totalQuality += childFactors.totalQuality;
      tempNumChildren += tempFactors.childrenAmount;
    }

    ++tempNumChildren;
  }
  return tempFactors;
}

Neuron BranchTracker::fastForwardHead() {
  // TODO: Do error checking?
  return this->_bestMoveNode;
}
