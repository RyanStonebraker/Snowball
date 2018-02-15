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

Board BranchTracker::getBestMove(BranchTracker::Color pieceColor) {
  this->_localBranch.spawnChildren(this->_branchWeightings.depth);

  // This returns the first valid move!

  // update local info with best cumulative info
  for (unsigned int child = 0; child < this->_localBranch.size(); ++child) {
    std::cout << "Starting Child " << child << std::endl;
    NodeFactors firstChildFactor = _cumulativeBranchWeight(*this->_localBranch[child], this->_branchWeightings.depth);

    std::cout << "\nChild " << child << " Factor: \n" << firstChildFactor;

    double childWeight = _sigmoidNormalizer(raw_weighting(firstChildFactor, pieceColor));

    // std::cout << "Immediate Childs Quality: " << (*this->_localBranch[child])[0]->getQuality() << "\n";

    std::cout << "Normalized Weight: " << childWeight << "\n" << std::endl;
    if (childWeight > this->_weight) {
      this->_bestMoveNode = *this->_localBranch[child];

      this->_totalKings = firstChildFactor.totalKings;
      this->_totalPieceCount = firstChildFactor.totalPieceCount;
      this->_totalQuality = firstChildFactor.totalQuality;

      this->_weight = childWeight;
    }
  }

  // return (*this->_localBranch[0]).getBoard();

  return this->_bestMoveNode.getBoard();
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


// START NEW ******

NodeFactors BranchTracker::_cumulativeBranchWeight(Neuron & temp_head, unsigned int depthLimit) {
  std::tuple<int,int> tempTotalKings;
  std::tuple<int,int> tempTotalPieceCount;
  std::tuple<int,int> tempTotalQuality;

  NodeFactors tempFactors;

  unsigned int tempNumChildren = 0;

  // Loop through children
  for (unsigned child = 0; child < temp_head.size(); ++child) {
    std::get<RED_PIECE>(tempTotalKings) += temp_head[child]->getRedKingCount();
    std::get<BLACK_PIECE>(tempTotalKings) += temp_head[child]->getBlackKingCount();

    std::get<RED_PIECE>(tempTotalPieceCount) += temp_head[child]->getRedPieceCount();
    std::get<BLACK_PIECE>(tempTotalPieceCount) += temp_head[child]->getBlackPieceCount();

    std::get<RED_PIECE>(tempTotalQuality) -= temp_head[child]->getQuality();

    // TODO: Make more versatile for Red/Black, this might not work to get accurate black piece quality
    std::get<BLACK_PIECE>(tempTotalQuality) += temp_head[child]->getQuality();
  }
  tempNumChildren += temp_head.size();

  tempFactors.totalKings = tempTotalKings;
  tempFactors.totalPieceCount = tempTotalPieceCount;
  tempFactors.totalQuality = tempTotalQuality;
  tempFactors.childrenAmount = tempNumChildren;

  return tempFactors;
}

// END NEW ********


// NodeFactors BranchTracker::_cumulativeBranchWeight(Neuron & temp_head, unsigned int depthLimit) {
//   std::tuple<int,int> tempTotalKings;
//   std::tuple<int,int> tempTotalPieceCount;
//   std::tuple<int,int> tempTotalQuality;
//
//   NodeFactors tempFactors;
//
//   unsigned int tempNumChildren = 0;
//
//     // Loop through every child of temp_head
//   for (unsigned int child = 0; child < temp_head.size(); ++child) {
    // std::get<RED_PIECE>(tempTotalKings) += temp_head[child]->getRedKingCount();
    // std::get<BLACK_PIECE>(tempTotalKings) += temp_head[child]->getBlackKingCount();
    //
    // std::get<RED_PIECE>(tempTotalPieceCount) += temp_head[child]->getRedPieceCount();
    // std::get<BLACK_PIECE>(tempTotalPieceCount) += temp_head[child]->getBlackPieceCount();
    //
    // std::get<RED_PIECE>(tempTotalQuality) += temp_head[child]->getQuality();
    //
    // // TODO: Make more versatile for Red/Black, this might not work to get accurate black piece quality
    // std::get<BLACK_PIECE>(tempTotalQuality) -= temp_head[child]->getQuality();
    //
    // tempFactors.totalKings = tempTotalKings;
    // tempFactors.totalPieceCount = tempTotalPieceCount;
    // tempFactors.totalQuality = tempTotalQuality;
//
//     // if child has children and haven't reached our depth limit, then recursive case
//     if (temp_head[child]->size() > 0 && depthLimit > 1) {
//       NodeFactors childFactors = _cumulativeBranchWeight(*temp_head[child], depthLimit - 1);
//
//       std::get<RED_PIECE>(tempFactors.totalKings) += std::get<RED_PIECE>(childFactors.totalKings);
//       std::get<BLACK_PIECE>(tempFactors.totalKings) += std::get<BLACK_PIECE>(childFactors.totalKings);
//
//       std::get<RED_PIECE>(tempFactors.totalPieceCount) += std::get<RED_PIECE>(childFactors.totalPieceCount);
//       std::get<BLACK_PIECE>(tempFactors.totalPieceCount) += std::get<BLACK_PIECE>(childFactors.totalPieceCount);
//
//       std::get<RED_PIECE>(tempFactors.totalKings) += std::get<RED_PIECE>(childFactors.totalKings);
//       std::get<BLACK_PIECE>(tempFactors.totalKings) += std::get<BLACK_PIECE>(childFactors.totalKings);
//
//       std::get<RED_PIECE>(tempFactors.totalQuality) += std::get<RED_PIECE>(childFactors.totalQuality);
//       std::get<BLACK_PIECE>(tempFactors.totalQuality) += std::get<BLACK_PIECE>(childFactors.totalQuality);
//
//       tempNumChildren += tempFactors.childrenAmount;
//     }
//
//     ++tempNumChildren;
//   }
//   return tempFactors;
// }

Neuron BranchTracker::fastForwardHead() {
  // TODO: Do error checking?
  return this->_bestMoveNode;
}
