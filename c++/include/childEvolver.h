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
    ChildEvolver(const int childrenPerGeneration, const std::string &generationLoc);
    void evolve(WeightedNode &startWeights, const int minGamesPerChild);
    void startGeneration();
    void loadGeneration(std::string generationLocation);
    void setMutationRate(double mutationRate);
    void setDepth(unsigned depth);
    double kingWeightPrime(const WeightedNode &);
    double sigmaWeightPrime(const WeightedNode &, int numberOfWeights);
    double nodeWeightPrime(const WeightedNode &);
    void setGenerationAmount(unsigned gens);
private:
    enum class Player {NONE, FIRST, SECOND};
    unsigned _startGen;
    int _childrenPerGeneration;
    unsigned _generations;
    unsigned _depth;
    WeightedNode _bestChild;
    WeightedNode _parent;
    std::vector<WeightedNode> _children;
    std::vector<WeightedNode> _bestChildren;
    unsigned _numberOfWeights;
    double _mutationRate;
private:
    double shiftWeight(double weight);
    double cappedShiftWeight(double weight, double startRange, double endRange);
    double randomNumber(double Min, double Max);
    void selectiveMutate(WeightedNode & parent);
    void mutate(const WeightedNode &startWeights, WeightedNode &resultWeights);
    void evolveAll();
    void tournamentEvent();
    Player playGame(WeightedNode &player1Weights, WeightedNode &player2Weights);
    void writeWeightsToFile(const int generation);
    void writeBestMoveForGeneration(const int generation);
    void writeTestCSV();
};
#endif
