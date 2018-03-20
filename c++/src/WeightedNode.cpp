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
    << " " << node.enemyFactor << " " << node.splitTieFactor << " " << node.depth
    << " " << node.gamesPlayed;
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
    double newEnemyFactor;
    double newSplitTieFactor;
    double newDepth;
    unsigned gamesPlayed;

    stream >> newFitness >> newWeight >> newKingingWeight >> newSigmaWeight >> newQualityWeight
    >> newAvailableMovesWeight >> newDepthWeight >> newRiskFactor >> newEnemyFactor
    >> newSplitTieFactor >> newDepth >> gamesPlayed;

    node.fitness = newFitness;
    node.weight = newWeight;
    node.kingingWeight = newKingingWeight;
    node.sigmaWeight = newSigmaWeight;
    node.qualityWeight = newQualityWeight;
    node.availableMovesWeight = newAvailableMovesWeight;
    node.depthWeight = newDepthWeight;
    node.riskFactor = newRiskFactor;
    node.enemyFactor = newEnemyFactor;
    node.splitTieFactor = newSplitTieFactor;
    node.depth = newDepth;
    node.gamesPlayed = gamesPlayed;

    return stream;
}

size_t WeightedNode::size() {
  return 7;
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
      return splitTieFactor;
    default:
      throw (std::out_of_range("double & WeightedNode::operator[](size_t index) : index must be between 0 and " + std::to_string(size())));
  }
}
