#ifndef WEIGHTED_NODE_H
#define WEIGHTED_NODE_H

#include <fstream>

// Weighted Node Structure meant to contain all scaling factor weights for current branch
struct WeightedNode {
  friend std::ostream &operator<<(std::ostream& stream, const WeightedNode& node);
  friend std::istream &operator>>(std::istream& stream, WeightedNode& node);
  double & operator[](size_t index);
  bool operator==(WeightedNode & rhs);
  bool operator!=(WeightedNode & rhs);
  size_t size();

  // How does this child neural network stack up against others in its generation? Metaweight.
  double fitness = 0;

  // Individual weight of each node?                                              Metaweight.
  // -0.20f - 0.20f
  double weight = 0;

  // How much does the number of kings a certain branch could generate affect decision?
  // 1.00f - 3.00f
  double kingingWeight = 0;

  //Initial value for sigma for each weight(bias factor)                          Non-used Weight.
  //Always set to .05
  double sigmaWeight = .05;

  // How much does the number of pieces captured per branch affect decision?
  // 0.00f - 1.00f
  double qualityWeight = 0;

  // How much does the freedom of movement per branch affect decision?            Non-used weight.
  // 0.00f - 1.00f
  double availableMovesWeight = 0;

  // How much does the distance from the current move affect decision?
  // 0.00f - 1.00f
  double depthWeight = 0;

  // How much should the maximum values of certain weights in comparison to
  // other same level branches affect decisions?                                  Non-used weight.
  // 0.00f - 1.00f
  double riskFactor = 0;

  // How much percentage should the player be beating the enemy by before it is
  // willing to take a risky move?
  double riskThreshold = 0;

  // How much should positive gains (in each weighting category) for the enemy
  // affect the move decision?                                                    Non-used weight.
  // 0.00f - 1.00f
  double enemyFactor = 0;

  // What threshold for similar best moves should a random top move be chosen in?
  // 0.00f - 1.00f
  double randomMoveThreshold = 0;

  // What depth should this branch be calculated to?                              Metaweight
  int depth = 0;

  // Keep track of the amount of games this neural network has been trained on.   Metaweight.
  unsigned gamesPlayed = 0;
};

#endif
