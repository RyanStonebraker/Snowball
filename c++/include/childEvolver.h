//
//  childEvolver.hpp
//  snowball
//
//  Created by Thatcher Lane on 2/28/18.
//  Copyright © 2018 Thatcher Lane. All rights reserved.
//

#ifndef childEvolver_h
#define childEvolver_h

#include "WeightedNode.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

class ChildEvolver{
private:
    int _childrenPerGeneration;
    enum class Player {NONE, FIRST, SECOND};
    std::vector<WeightedNode> _children;

    void mutate(const WeightedNode &startWeights, WeightedNode &resultWeights);
    Player playGame(WeightedNode &player1Weights, WeightedNode &player2Weights);
    void writeWeightsToFile(const int generation);
    void writeTestCSV();
public:
    ChildEvolver(const int childrenPerGeneration, const WeightedNode &startWeights);
    void evolve(const WeightedNode &startWeights, const int depth, const int generations, const int minGamesPerChild);
    float kingWeightPrime(const WeightedNode &);
    float sigmaWeightPrime(const WeightedNode &, int numberOfWeights);
    float nodeWeightPrime(const WeightedNode &);

};
#endif /* childEvolver_h */
