#ifndef WEIGHTED_NODE_H
#define WEIGHTED_NODE_H

#include <fstream>

// Weighted Node Structure meant to contain all scaling factor weights for current branch
struct WeightedNode {

  // Individual weight of each node?
  // -0.20f - 0.20f
  float weight = 0;

  // How much does the number of kings a certain branch could generate affect decision?
  // 1.00f - 3.00f
  float kingingWeight = 0;

  //Initial value for sigma for each weight(bias factor)
  //Always set to .05
  float sigmaWeight = .05;

  // How much does the number of pieces captured per branch affect decision?
  // 0.00f - 1.00f
  float qualityWeight = 0;

  // How much does the freedom of movement per branch affect decision?
  // 0.00f - 1.00f
  float availableMovesWeight = 0;

  // How much does the distance from the current move affect decision?
  // 0.00f - 1.00f
  float depthWeight = 0;

  // How much should the maximum values of certain weights in comparison to
  // other same level branches affect decisions?
  // 0.00f - 1.00f
  float riskFactor = 0;

  // How much should positive gains (in each weighting category) for the enemy
  // affect the move decision?
  // 0.00f - 1.00f
  float enemyFactor = 0;

  // What depth should this branch be calculated to?
  int depth = 0;
};

#endif
