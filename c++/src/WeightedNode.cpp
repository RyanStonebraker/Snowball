//
//  WeightedNode.cpp
//  snowball
//
//  Created by Thatcher Lane on 3/1/18.
//  Copyright © 2018 Thatcher Lane. All rights reserved.
//

#include "WeightedNode.h"
#include <stdio.h>
#include <stdexcept>

std::ostream &operator<<(std::ostream& stream, const WeightedNode& node){
    stream << node.fitness << " " << node.weight << " " << node.kingingWeight
    << " " << node.sigmaWeight << " " << node.qualityWeight << " "
    << node.availableMovesWeight << " " << node.depthWeight << " " << node.riskFactor
    << " " << node.riskThreshold << " " << node.enemyFactor << " " << node.randomMoveThreshold
    << " " << node.depth << " " << node.gamesPlayed;
    return stream;
}

std::istream &operator>>(std::istream& stream, WeightedNode& node){
    double newFitness;
    double newWeight;
    double newKingingWeight;
    double newSigmaWeight;
    double newQualityWeight;
    double newAvailableMovesWeight;
    double newDepthWeight;
    double newRiskFactor;
    double newRiskThreshold;
    double newEnemyFactor;
    double newRandomMoveThreshold;
    double newDepth;
    unsigned gamesPlayed;

    stream >> newFitness >> newWeight >> newKingingWeight >> newSigmaWeight >> newQualityWeight
    >> newAvailableMovesWeight >> newDepthWeight >> newRiskFactor >> newRiskThreshold >> newEnemyFactor
    >> newRandomMoveThreshold >> newDepth >> gamesPlayed;

    node.fitness = newFitness;
    node.weight = newWeight;
    node.kingingWeight = newKingingWeight;
    node.sigmaWeight = newSigmaWeight;
    node.qualityWeight = newQualityWeight;
    node.availableMovesWeight = newAvailableMovesWeight;
    node.depthWeight = newDepthWeight;
    node.riskFactor = newRiskFactor;
    node.riskThreshold = newRiskThreshold;
    node.enemyFactor = newEnemyFactor;
    node.randomMoveThreshold = newRandomMoveThreshold;
    node.depth = newDepth;
    node.gamesPlayed = gamesPlayed;

    return stream;
}

size_t WeightedNode::size() {
  return 8;
}

double & WeightedNode::operator[](size_t index) {
  switch(index) {
    case 0:
      return kingingWeight;
    case 1:
      return qualityWeight;
    case 2:
      return availableMovesWeight;
    case 3:
      return depthWeight;
    case 4:
      return riskFactor;
    case 5:
      return enemyFactor;
    case 6:
      return randomMoveThreshold;
    case 7:
      return riskThreshold;
    default:
      throw (std::out_of_range("double & WeightedNode::operator[](size_t index) : index must be between 0 and " + std::to_string(size())));
  }
}

bool WeightedNode::operator==(WeightedNode & rhs) {
  for (auto i = 0; i < size(); ++i) {
      if (rhs[i] != (*this)[i])
        return false;
  }
  if (rhs.gamesPlayed != (*this).gamesPlayed || rhs.weight != (*this).weight)
    return false;
  return true;
}

bool WeightedNode::operator!=(WeightedNode & rhs) {
  return !(*this == rhs);
}
