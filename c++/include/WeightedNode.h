#ifndef WEIGHTED_NODE_H
#define WEIGHTED_NODE_H

#include <fstream>

// Weighted Node Structure meant to contain all scaling factor weights for current branch
struct WeightedNode {
  friend std::ostream &operator<<(std::ostream& stream, const WeightedNode& node);
  friend std::istream &operator>>(std::istream& stream, WeightedNode& node);
  double & operator[](size_t index);
  size_t size();

  // How does this child neural network stack up against others in its generation?
  double fitness = 0;

  // Individual weight of each node?
  // -0.20f - 0.20f
  double weight = 0;

  // How much does the number of kings a certain branch could generate affect decision?
  // 1.00f - 3.00f
  double kingingWeight = 0;

  //Initial value for sigma for each weight(bias factor)
  //Always set to .05
  double sigmaWeight = .05;

  // How much does the number of pieces captured per branch affect decision?
  // 0.00f - 1.00f
  double qualityWeight = 0;

  // How much does the freedom of movement per branch affect decision?
  // 0.00f - 1.00f
  double availableMovesWeight = 0;

  // How much does the distance from the current move affect decision?
  // 0.00f - 1.00f
  double depthWeight = 0;

  // How much should the maximum values of certain weights in comparison to
  // other same level branches affect decisions?
  // 0.00f - 1.00f
  double riskFactor = 0;

  // How much should positive gains (in each weighting category) for the enemy
  // affect the move decision?
  // 0.00f - 1.00f
  double enemyFactor = 0;

  // When two moves have the same weight, should left moves be prefered over right moves?
  // Ex. < 0.5 = left preference, > 0.5 = right preference
  double splitTieFactor = 0.5;

  // What depth should this branch be calculated to?
  int depth = 0;

  // Keep track of the amount of games this neural network has been trained on
  unsigned gamesPlayed = 0;
};

#endif
