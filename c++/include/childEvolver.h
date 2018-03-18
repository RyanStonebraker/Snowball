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
public:
    ChildEvolver(const int childrenPerGeneration, const WeightedNode &startWeights);
    void evolve(WeightedNode &startWeights, const int minGamesPerChild);
    void startGeneration();
    void setMutationRate(double mutationRate);
    void setDepth(unsigned depth);
    double kingWeightPrime(const WeightedNode &);
    double sigmaWeightPrime(const WeightedNode &, int numberOfWeights);
    double nodeWeightPrime(const WeightedNode &);
    void setGenerationAmount(unsigned gens);
private:
    enum class Player {NONE, FIRST, SECOND};
    int _childrenPerGeneration;
    unsigned _generations;
    unsigned _depth;
    WeightedNode _bestChild;
    WeightedNode _parent;
    std::vector<WeightedNode> _children;
    double _mutationRate;
private:
    double shiftWeight(double weight);
    double randomNumber(double Min, double Max);
    void mutate(const WeightedNode &startWeights, WeightedNode &resultWeights);
    Player playGame(WeightedNode &player1Weights, WeightedNode &player2Weights);
    void writeWeightsToFile(const int generation);
    void writeBestMoveForGeneration(const int generation);
    void writeTestCSV();
};
#endif
