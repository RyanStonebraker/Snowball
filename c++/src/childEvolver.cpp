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
#include <algorithm>
#include "NeuralNetwork.h"

#ifdef _WIN32
	#include <windows.h>
	#define MKDIR "md "
#endif
#ifndef _WIN32
	#include <stdlib.h>
  #define MKDIR "mkdir "
#endif

std::random_device rd{};
std::mt19937 engine{rd()};
std::uniform_real_distribution<double> dist{-0.1, 0.1};

std::default_random_engine generator;
std::normal_distribution<> distribution{0,(0.5/3)};

double randomGausian(){
    double randNum = distribution(engine);
    return randNum;
}

ChildEvolver::ChildEvolver(const int childrenPerGeneration, const WeightedNode &startWeights)
  : _childrenPerGeneration(childrenPerGeneration), _generations(3), _parent(startWeights),
    _children(childrenPerGeneration), _numberOfWeights(7), _mutationRate(0.2) {
      _depth = startWeights.depth;
    }

// TODO: make mutation rate a weight and have AI decide how random it wants to be
void ChildEvolver::setMutationRate(double mutationRate) {
  _mutationRate = mutationRate;
}

void ChildEvolver::setGenerationAmount(unsigned gens) {
  _generations = gens;
}

void ChildEvolver::startGeneration() {
  auto saveMutationRate = _mutationRate;

  std::cout << "Starting Generation 0:" << std::endl;
  // Generate gen0 completely random children
  setMutationRate(1);
  for (int i = 0; i < _children.size(); ++i) {
      _children[i].depth = _depth;
      mutate(_parent, _children[i]);
  }
  writeWeightsToFile(0);

  setMutationRate(saveMutationRate);

  for (auto currentGen = 1; currentGen <= _generations; ++currentGen) {
    WeightedNode blankNode;
    _bestChild = blankNode;
    std::cout << "Starting Generation " << currentGen << ":" << std::endl;

    // Slightly mutate the best 50% of children
    std::sort(_children.begin(), _children.end(), [](auto child1, auto child2) {
      return child1.fitness >= child2.fitness;
    });
    for (int i = 0; i >= _children.size() / 2; ++i) {
        selectiveMutate(_children[i]);
    }

    // Kill the last 50% of the children and replace with random.
    // spawning based off of _parent allows a controlled bias to be introduced
    setMutationRate(1);
    for (int i = floor(_children.size() / 2); i < _children.size(); ++i) {
      _children[i] = _parent;
      mutate(_parent, _children[i]);
    }
    setMutationRate(saveMutationRate);

    for (int i = 0; i < _children.size(); ++i) {
      evolve(_children[i], 5);
      std::cout << "Child " << i << " Fitness: " << _children[i].fitness / _children[i].gamesPlayed
      << " Best Child Fitness: " << _bestChild.fitness / _bestChild.gamesPlayed << " Games Played: "
      << _children[i].gamesPlayed;
			if (_children[i].fitness == _bestChild.fitness && _children[i].gamesPlayed == _bestChild.gamesPlayed && _children[i].weight == _bestChild.weight)
				std::cout << " - BEST CHILD";
			std::cout << std::endl;
    }
    writeWeightsToFile(currentGen);
  }
  // writeTestCSV(); // Only uncomment to test gaussian distribution
}

// Generate a child for every weight (only changing one weight), then play against
// parent. Cross-Breed those that beat the parent (if none beat the parent,
// the parent moves on unchanged)
void ChildEvolver::selectiveMutate(WeightedNode & parent) {
	for (unsigned i = 0; i < parent.size(); ++i) {
		WeightedNode potentialChild = parent;
		potentialChild[i] = shiftWeight(potentialChild[i]);
		auto winner = playGame(parent, potentialChild);

		// Cross Breeding - add winning trait to parent
		if (winner == Player::SECOND) {
			parent[i] = potentialChild[i];
		}
	}
}

double ChildEvolver::shiftWeight(double weight){
  return weight + randomNumber(-_mutationRate, _mutationRate);
}

void ChildEvolver::mutate(const WeightedNode &startWeights, WeightedNode &resultWeights) {
    resultWeights.kingingWeight = kingWeightPrime(startWeights);
    resultWeights.sigmaWeight = sigmaWeightPrime(startWeights, 6);
    resultWeights.weight = nodeWeightPrime(startWeights);
    // These will need to be changed at some point
    resultWeights.qualityWeight = shiftWeight(startWeights.qualityWeight);
    resultWeights.availableMovesWeight = shiftWeight(startWeights.availableMovesWeight);
    resultWeights.depthWeight = shiftWeight(startWeights.availableMovesWeight);
    resultWeights.riskFactor = shiftWeight(startWeights.riskFactor);
    resultWeights.enemyFactor = shiftWeight(startWeights.enemyFactor);
    resultWeights.splitTieFactor = shiftWeight(startWeights.splitTieFactor);
}

ChildEvolver::Player ChildEvolver::playGame(WeightedNode &player1Weights, WeightedNode &player2Weights) {
    Board board(STARTING_BOARD_STRING);

    NeuralNetwork player1;
    player1.loadStartingWeights(player1Weights);
    player1.setMoveColor(COMPUTER_RED);

    NeuralNetwork player2;
    player2.loadStartingWeights(player2Weights);
    player2.setMoveColor(COMPUTER_BLACK);

    // Play game (only allow 100 moves)
    for (int i = 0; i < 200; ++i) {
        player1.receiveMove(board.getBoardStateString());
        board = {player1.getBestMove()};
        if (board.getBlackPieceCount() == 0) {
            player1Weights.fitness += 1;
            player2Weights.fitness -= 1;
            ++player1Weights.gamesPlayed;
            ++player2Weights.gamesPlayed;
            return Player::FIRST;
        }

        // Player 2 turn
        player2.receiveMove(board.getBoardStateString());
        board = {player2.getBestMove()};
        if (board.getRedPieceCount() == 0) {
            player2Weights.fitness += 1;
            player1Weights.fitness -= 1;
            ++player1Weights.gamesPlayed;
            ++player2Weights.gamesPlayed;
            return Player::SECOND;
        }
    }

		// 'A' for effort...
		if (board.getBlackPieceCount() > board.getRedPieceCount())
			player1Weights.fitness += 0.5;
		else if (board.getRedPieceCount() > board.getBlackPieceCount())
			player2Weights.fitness += 0.5;

    player1Weights.fitness -= 1;
    player2Weights.fitness -= 1;
    ++player1Weights.gamesPlayed;
    ++player2Weights.gamesPlayed;
    return Player::NONE;
}

void ChildEvolver::writeWeightsToFile(const int generation) {
    cout << "Writing generation " << generation << endl;
    auto genFolder = "../training/gen" + to_string(generation) + "/";
    system((MKDIR + genFolder).c_str());
    auto genChildWeights = genFolder + "childWeights/";
    system((MKDIR + genChildWeights).c_str());

    for (int i = 0; i < _childrenPerGeneration; ++i) {

        // NOTE: easiest to use text format until were sure everything is working
        ofstream outFile(genChildWeights + "child_" + to_string(i) + ".txt");
        outFile << _children[i];

    }
    if (_bestChild.gamesPlayed > 0)
      writeBestMoveForGeneration(generation);
}

void ChildEvolver::writeBestMoveForGeneration(const int gen) {
  auto genFolder = "../training/gen" + to_string(gen) + "/";
  auto bestMoveOut = genFolder + "bestGen" + to_string(gen)+"Child.txt";

  ofstream bestChildWriter(bestMoveOut);
  bestChildWriter << _bestChild;
  bestChildWriter.close();
}

// This function is only useful for generating the spreadsheet required for assignment 3.
void ChildEvolver::writeTestCSV() {
    cout << "Writing test CSV..." << endl;
    ofstream outFile;
    outFile.open("../weights.csv");
    outFile << "Weight, Kinging Weight, Sigma Weight" << endl;
    for (int i = 0; i < _childrenPerGeneration; ++i) {
        outFile << _children[i].weight << "," << _children[i].kingingWeight << "," << _children[i].sigmaWeight << "," <<  i << endl;
    }
    outFile.close();
}

void ChildEvolver::evolve(WeightedNode & startWeights, const int minGamesPerChild) {
// Each child plays a minimum of [minGamesPerChild] games against a random opponent
// (potentially plays more)
      for (int i = 0; i < minGamesPerChild; ++i) {
          auto opponent = _children[unsigned(randomNumber(0, _children.size())) % _children.size()];
          playGame(startWeights, opponent);
      }
      // TODO: randomly split ties so that sometimes _bestChild = startWeights if fitness is equal
      if (_bestChild.gamesPlayed == 0)
        _bestChild = startWeights;
      else if ((startWeights.fitness / startWeights.gamesPlayed) > (_bestChild.fitness / _bestChild.gamesPlayed)) {
			  _bestChild = startWeights;
			}
}

double ChildEvolver::randomNumber(double min, double max)
{
  std::mt19937 randomNumber{rd()};
  std::uniform_real_distribution<double> betweenRange(min, max);
  double randomReal = betweenRange(randomNumber);
	// std::cout << "Random between (" << min << "," << max << "): " << randomReal << std::endl;
  return randomReal;
}


double ChildEvolver::kingWeightPrime(const WeightedNode &node){
    double oldKingWeight = node.kingingWeight;
    double kingWeight;
    kingWeight = oldKingWeight + randomNumber(-.1, .1);
    return kingWeight;
}

double ChildEvolver::sigmaWeightPrime(const WeightedNode &node, int numberOfWeights){
    double oldSigmaWeight = node.sigmaWeight;
    double sigmaWeight;
    double tau = 1/(sqrt(2*sqrt(numberOfWeights)));
    sigmaWeight = oldSigmaWeight*(exp(tau*randomGausian()));
    return sigmaWeight;
}

double ChildEvolver::nodeWeightPrime(const WeightedNode &node){
    double sigmaWeight = node.sigmaWeight;
    double oldNodeWeight = node.weight;
    double nodeWeight;
    nodeWeight = oldNodeWeight + (sigmaWeight*randomGausian());
    return nodeWeight;
}
