#ifndef WEIGHTED_NODE_H
#define WEIGHTED_NODE_H

// Weighted Node Structure meant to contain all scaling factor weights for current branch
struct WeightedNode {
  // How much does the number of kings a certain branch could generate affect decision?
  // 0.00f - 1.00f
  float kingingWeight;

  // How much does the number of pieces captured per branch affect decision?
  // 0.00f - 1.00f
  float qualityWeight;

  // How much does the freedom of movement per branch affect decision?
  // 0.00f - 1.00f
  float availableMovesWeight;

  // How much does the distance from the current move affect decision?
  // 0.00f - 1.00f
  float depthWeight;

  // How much should the maximum values of certain weights in comparison to
  // other same level branches affect decisions?
  // 0.00f - 1.00f
  float riskFactor;

  // How much should positive gains (in each weighting category) for the enemy
  // affect the move decision?
  // 0.00f - 1.00f
  float enemyFactor;

  // What depth should this branch be calculated to?
  int depth;
};

#endif