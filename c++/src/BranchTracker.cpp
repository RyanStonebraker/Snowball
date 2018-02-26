// BranchTracker.cpp
// 2/13/2018
// Snowball AI
// Definitions for BranchTracker.h, wrapper for neuron class that checks a given
// path and only appends it to the main _head if it is within a certain %

#include "BranchTracker.h"
#include "neuron.h"
#include "board.h"
#include <math.h>
#include <iostream>
#include <unistd.h>

// DEBUG: Output for debugging
std::ostream & operator<< (std::ostream & os, const NodeFactors & nf) {
  os << "Red Total Kings: " << std::get<0>(nf.totalKings) << " Black Total Kings: " << std::get<1>(nf.totalKings) << "\n";
  os << "Red Total Piece Count: " << std::get<0>(nf.totalPieceCount) << " Black Total Piece Count: " << std::get<1>(nf.totalPieceCount) << "\n";
  os << "Red Total Quality: " << std::get<0>(nf.totalQuality) << " Black Total Quality: " << std::get<1>(nf.totalQuality) << "\n";
  os << "Children Amount: " << nf.childrenAmount << std::endl;

  return os;
}

// 2 param ctor for BranchTracker, NOTE WeightedNode is passed by reference to
// allow for updating given a branches returned values
BranchTracker::BranchTracker(Board &startBoard, WeightedNode &branchWeight)
    : _branchWeightings(branchWeight), _childrenAmount(0), _startBoard(startBoard) {
  // Update Neuron starting board
  Neuron currLevel(startBoard);
  _localBranch = currLevel;

}

Board BranchTracker::getStartBoard() const {
  return _startBoard;
}

unsigned BranchTracker::getTotalChildren() const {
  return _totalChildren;
}

Board BranchTracker::getBestMove(BranchTracker::Color pieceColor) {
  if (pieceColor == BLACK_PIECE)
    this->_localBranch.spawnChildrenBlackFirst(this->_branchWeightings.depth);
  else
    this->_localBranch.spawnChildrenRedFirst(this->_branchWeightings.depth);

  // This returns the first valid move!


  int bestChild = -1;
  double bestWeight = -1;

  // DEBUG: for debugging purposes
  NodeFactors bestFactor;

  // update local info with best cumulative info
  for (unsigned int child = 0; child < this->_localBranch.size(); ++child) {
    // std::cout << "\nStarting Child " << child << std::endl;
    NodeFactors firstChildFactor = _cumulativeBranchWeight(*this->_localBranch[child], this->_branchWeightings.depth);

    this->_totalChildren += firstChildFactor.childrenAmount;

    // std::cout << "\nChild " << child << " Factor: \n" << firstChildFactor;

    double childWeight = _sigmoidNormalizer(raw_weighting(firstChildFactor, pieceColor));

    // std::cout << "Normalized Weight: " << childWeight << "\n" << std::endl;
    if (childWeight > bestWeight) {
      bestFactor = firstChildFactor;
      this->_bestMoveNode = *this->_localBranch[child];

      this->_totalKings = firstChildFactor.totalKings;
      this->_totalPieceCount = firstChildFactor.totalPieceCount;
      this->_totalQuality = firstChildFactor.totalQuality;

      this->_weight = childWeight;
      bestWeight = childWeight;
      bestChild = child;
    }
    this->_childrenAmount += firstChildFactor.childrenAmount;

    // std::cout << "ENDING CHILD " << child << std::endl;
    // std::cout << "ENDING BOARD: " << (*this->_localBranch[child]).getBoard() << std::endl;
  }


  if (bestChild >= 0) {
    // std::cout << "\n*** BEST BOARD FACTOR: " << bestFactor << std::endl;
    return (*this->_localBranch[bestChild]).getBoard();
  }
  else {
    if (this->_localBranch.size() > 0)
      return (*this->_localBranch[0]).getBoard();
    // std::cout << "SIZE OF ENDING LOCAL BRANCH: " << this->_localBranch.size() << std::endl;
    // std::cout << "NO MOVES LEFT TO MAKE, GAME OVER!" << std::endl;

    _exit(0);
  }
}

double BranchTracker::_sigmoidNormalizer (double raw_weight) {
  return tanh(raw_weight);
}

double BranchTracker::raw_weighting(const NodeFactors & factors, const Color pieceColor) {
  double raw_total = 0;

  double kingFactor = 0;
  double freedomFactor = 0;
  double qualityFactor = 0;
  double enemyKingFactor = 0;
  double enemyFreedomFactor = 0;
  double enemyQualityFactor = 0;

  // Prevents dividing by 0 if no children
  if (factors.childrenAmount == 0) {
    return 0;
  }

  if (pieceColor == RED_PIECE) {
    kingFactor = (std::get<RED_PIECE>(factors.totalKings)) / factors.childrenAmount * _branchWeightings.kingingWeight;
    freedomFactor = std::get<RED_PIECE>(factors.totalPieceCount) / factors.childrenAmount * _branchWeightings.availableMovesWeight;
    qualityFactor = std::get<RED_PIECE>(factors.totalQuality) / factors.childrenAmount * _branchWeightings.qualityWeight;

    enemyKingFactor = std::get<BLACK_PIECE>(factors.totalKings) / factors.childrenAmount * _branchWeightings.kingingWeight * _branchWeightings.enemyFactor;
    enemyFreedomFactor = std::get<BLACK_PIECE>(factors.totalPieceCount) / factors.childrenAmount * _branchWeightings.availableMovesWeight * _branchWeightings.enemyFactor;
    enemyQualityFactor = std::get<BLACK_PIECE>(factors.totalQuality) / factors.childrenAmount * _branchWeightings.qualityWeight * _branchWeightings.enemyFactor;
  }
  else {
    kingFactor = (std::get<BLACK_PIECE>(factors.totalKings)) / factors.childrenAmount * _branchWeightings.kingingWeight;
    freedomFactor = std::get<BLACK_PIECE>(factors.totalPieceCount) / factors.childrenAmount * _branchWeightings.availableMovesWeight;
    qualityFactor = std::get<BLACK_PIECE>(factors.totalQuality) / factors.childrenAmount * _branchWeightings.qualityWeight;

    enemyKingFactor = std::get<RED_PIECE>(factors.totalKings) / factors.childrenAmount * _branchWeightings.kingingWeight * _branchWeightings.enemyFactor;
    enemyFreedomFactor = std::get<RED_PIECE>(factors.totalPieceCount) / factors.childrenAmount * _branchWeightings.availableMovesWeight * _branchWeightings.enemyFactor;
    enemyQualityFactor = std::get<RED_PIECE>(factors.totalQuality) / factors.childrenAmount * _branchWeightings.qualityWeight * _branchWeightings.enemyFactor;
  }

  raw_total = kingFactor + freedomFactor + qualityFactor - enemyKingFactor - enemyFreedomFactor - enemyQualityFactor;
  return raw_total;
}

NodeFactors BranchTracker::_cumulativeBranchWeight(Neuron & temp_head, unsigned int depthLimit) {
  std::tuple<int,int> tempTotalKings;
  std::tuple<int,int> tempTotalPieceCount;
  std::tuple<int,int> tempTotalQuality;

  NodeFactors tempFactors;
  tempFactors.totalKings = {0, 0};
  tempFactors.totalPieceCount = {0, 0};
  tempFactors.totalQuality = {0, 0};
  tempFactors.childrenAmount = 0;

  unsigned int tempNumChildren = 0;

  // greater than 1 because this is already a child of the first move
  // Loop through children
  // #pragma omp parallel for
  // for (unsigned child = 0; child < temp_head.size(); ++child) {
    _recursivelyAddWeight(tempFactors, temp_head, depthLimit);
  // }

  if (tempNumChildren <= 0) {
    return tempFactors;
  }

  return tempFactors;
}

void BranchTracker::_recursivelyAddWeight (NodeFactors & tempFactors, Neuron temp_head, unsigned depthLimit) {
  // #pragma omp parallel for
  for (unsigned child = 0; child < temp_head.size(); ++child) {
    std::get<RED_PIECE>(tempFactors.totalKings) += temp_head[child]->getRedKingCount();
    std::get<BLACK_PIECE>(tempFactors.totalKings) += temp_head[child]->getBlackKingCount();

    std::get<RED_PIECE>(tempFactors.totalPieceCount) += temp_head[child]->getRedPieceCount();
    std::get<BLACK_PIECE>(tempFactors.totalPieceCount) += temp_head[child]->getBlackPieceCount();

    std::get<RED_PIECE>(tempFactors.totalQuality) -= temp_head[child]->getQuality();

    // TODO: Make more versatile for Red/Black, this might not work to get accurate black piece quality
    std::get<BLACK_PIECE>(tempFactors.totalQuality) += temp_head[child]->getQuality();
    if (depthLimit > 1 && temp_head.size() > 0)
      _recursivelyAddWeight(tempFactors, *temp_head[child], depthLimit - 1);
  }
  tempFactors.childrenAmount += temp_head.size();
}

Neuron BranchTracker::fastForwardHead() {
  // TODO: Do error checking?
  return this->_bestMoveNode;
}
