//
//  WeightedNode.cpp
//  snowball
//
//  Created by Thatcher Lane on 3/1/18.
//  Copyright © 2018 Thatcher Lane. All rights reserved.
//

#include <stdio.h>
#include "WeightedNode.h"

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
