//
//  childEvolver.cpp
//  snowball
//
//  Created by Thatcher Lane on 2/28/18.
//  Copyright © 2018 Thatcher Lane. All rights reserved.
//

#include "childEvolver.h"
#include "WeightedNode.h"
#include "board.h"
#include "constants.h"
#include "BranchTracker.h"
#include "util.h"
#include <math.h>
#include <vector>
using std::vector;
#include <fstream>
using std::ofstream;
#include <string>
using std::to_string;
#include <iostream>
using std::cout;
using std::endl;
#include <random>

ChildEvolver::ChildEvolver(const int childrenPerGeneration, const WeightedNode &startWeights){
    _childrenPerGeneration = childrenPerGeneration;
    _children = vector<WeightedNode>(childrenPerGeneration);
    _children[0] = startWeights;

    for (int i = 1; i < _childrenPerGeneration; ++i) {
        WeightedNode newChild;
        mutate(startWeights, newChild);
        _children[i] = newChild;
    }

    writeWeightsToFile(0);
    writeTestCSV(); // Only uncomment to test gaussian distribution

}

void ChildEvolver::mutate(const WeightedNode &startWeights, WeightedNode &resultWeights) {
    resultWeights.kingingWeight = kingWeightPrime(startWeights);
    resultWeights.sigmaWeight = sigmaWeightPrime(startWeights, 6);
    resultWeights.weight = nodeWeightPrime(startWeights);
    // These will need to be changed at some point
    resultWeights.qualityWeight = startWeights.qualityWeight;
    resultWeights.availableMovesWeight = startWeights.availableMovesWeight;
    resultWeights.depthWeight = startWeights.availableMovesWeight;
    resultWeights.riskFactor = startWeights.riskFactor;
    resultWeights.enemyFactor = startWeights.enemyFactor;
}

ChildEvolver::Player ChildEvolver::playGame(WeightedNode &player1Weights, WeightedNode &player2Weights) {
    Board board(STARTING_BOARD_STRING);
    Player winner = Player::NONE;

    // Play game (only allow 70 moves)
    for (int i = 0; i < 35; ++i) {
        // Player 1 is always red, player 2 is always black. We might want to change this, not sure if it matters?
        BranchTracker branchTracker1(board, player1Weights);
        board = branchTracker1.getBestMove(BranchTracker::Color::RED_PIECE);
        // Did player 1 win?
        if (board.getBlackPieceCount() == 0) {
            winner = Player::FIRST;
            break;
        }

        // Player 2 turn
        BranchTracker branchTracker2(board, player2Weights);
        board = branchTracker2.getBestMove(BranchTracker::Color::BLACK_PIECE);
        // Did player 2 win?
        if (board.getRedPieceCount() == 0) {
            winner = Player::SECOND;
            break;
        }
    }
    return winner;
}

void ChildEvolver::writeWeightsToFile(const int generation) {
    cout << "Writing generation " << generation << endl;
    // This function desperately needs to be improved to make its own directories, this is an extremely minimal implementation
    for (int i = 0; i < _childrenPerGeneration; ++i) {
        // cout << "- Writing child " << i << endl;
        ofstream outFile;
        outFile.open("../training/gen" + to_string(generation) + "/childWeights/" + to_string(i), std::ios::binary);
        outFile.write((char *)&_children[i], sizeof(WeightedNode));
        outFile.close();
    }
}

// This function is only useful for generating the spreadsheet required for assignment 3.
void ChildEvolver::writeTestCSV() {
    cout << "Writing test CSV..." << endl;
    ofstream outFile;
    outFile.open("../weights.csv");
    outFile << "Weight, Kinging Weight, Sigma Weight" << endl;
    for (int i = 0; i < _childrenPerGeneration; ++i) {
        outFile << _children[i].weight << "," << _children[i].kingingWeight << "," << _children[i].sigmaWeight << endl;
    }
    outFile.close();
}

void ChildEvolver::evolve(const WeightedNode & startWeights, const int depth, const int generations, const int minGamesPerChild) {
// Each child plays [minGamesPerChild] games against a random opponent
    vector<int> wins;
    for (int i = 0; i < _childrenPerGeneration; ++i) {
        for (int i = 0; i < minGamesPerChild; ++i) {
            auto opponent = _children[Util::randInt(_childrenPerGeneration - 1)];
            auto result = playGame(_children[i], opponent);
            // NEEDS TO BE FINISHED FOR ASSIGNMENT 4
        }
    }
}

float randomNumber(float Min, float Max)
{
    std::random_device rd{};
    std::mt19937 engine{rd()};
    std::uniform_real_distribution<double> dist{-0.1, 0.1};

    double randomReal = dist(engine);
    return randomReal;
}

float randomGausian(){
    std::default_random_engine generator;
    std::normal_distribution<double> distribution(.5,.25);
    float randNum = distribution(generator);
    return randNum;
}

float ChildEvolver::kingWeightPrime(const WeightedNode &node){
    float oldKingWeight = node.kingingWeight;
    float kingWeight;
    kingWeight = oldKingWeight + randomNumber(-.1, .1);
    return kingWeight;
}

float childEvolver::sigmaWeightPrime(WeightedNode &node, int numberOfWeights){
    float oldSigmaWeight = node.sigmaWeight;
    float sigmaWeight;
    double tau = 1/(sqrt(2*sqrt(numberOfWeights)));
    sigmaWeight = oldSigmaWeight*(exp(tau*randomGausian()));
    return sigmaWeight;
}

float childEvolver::nodeWeightPrime(WeightedNode &node){
    float sigmaWeight = node.sigmaWeight;
    float oldNodeWeight = node.weight;
    float nodeWeight;
    nodeWeight = oldNodeWeight + (sigmaWeight*randomGausian());
    return nodeWeight;
}
