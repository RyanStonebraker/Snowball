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
    stream << node.weight << " " << node.kingingWeight << " " << node.sigmaWeight << " " << node.qualityWeight << " " << node.availableMovesWeight << " " << node.depthWeight << " " << node.riskFactor << " " << node.enemyFactor << " " << node.depth;
    return stream;
}

std::istream &operator>>(std::istream& stream, WeightedNode& node){
    float newWeight;
    float newKingingWeight;
    float newSigmaWeight;
    float newQualityWeight;
    float newAvailableMovesWeight;
    float newDepthWeight;
    float newRiskFactor;
    float newEnemyFactor;
    float newDepth;

    stream >> newWeight >> newKingingWeight >> newSigmaWeight >> newQualityWeight >> newAvailableMovesWeight >> newDepthWeight >> newRiskFactor >> newEnemyFactor >> newDepth;


    node.weight = newWeight;
    node.kingingWeight = newKingingWeight;
    node.sigmaWeight = newSigmaWeight;
    node.qualityWeight = newQualityWeight;
    node.availableMovesWeight = newAvailableMovesWeight;
    node.depthWeight = newDepthWeight;
    node.riskFactor = newRiskFactor;
    node.enemyFactor = newEnemyFactor;
    node.depth = newDepth;
    
    return stream;
}
