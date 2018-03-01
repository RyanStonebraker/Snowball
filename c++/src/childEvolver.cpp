//
//  childEvolver.cpp
//  snowball
//
//  Created by Thatcher Lane on 2/28/18.
//  Copyright © 2018 Thatcher Lane. All rights reserved.
//

#include "childEvolver.h"
#include "WeightedNode.h"
#include <math.h>

childEvolver::childEvolver(){

}

float randomNumber(float Min, float Max)
{
    return ((float(rand()) / float(RAND_MAX)) * (Max - Min)) + Min;
}

float childEvolver::kingWeightPrime(WeightedNode &node){
    float oldKingWeight = node.kingingWeight;
    float kingWeight;
    kingWeight = oldKingWeight + randomNumber(-.1, .1);
    return kingWeight;
}

float childEvolver::sigmaWeightPrime(WeightedNode &node, int numberOfWeights){
    float oldSigmaWeight = node.sigmaWeight;
    float sigmaWeight;
    double tau = 1/(sqrt(2*sqrt(numberOfWeights)));
    sigmaWeight = oldSigmaWeight*(exp(tau*tanh(oldSigmaWeight)));
    return sigmaWeight;
}

float childEvolver::nodeWeightPrime(WeightedNode &node){
    float sigmaWeight = node.sigmaWeight;
    float oldNodeWeight;
    float nodeWeight;
    nodeWeight = oldNodeWeight + (sigmaWeight+tanh(oldNodeWeight));
    return nodeWeight;
}
