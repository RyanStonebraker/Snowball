#ifndef NEURON_H_INCLUDED
#define NEURON_H_INCLUDED
#include "board.h"
#include <vector>
#include <memory>

class Neuron {
public:
	Neuron(Board &board);
	void spawnChildren(int depth);
	void updateAverageWeights();

private:
	float _weight;
	float _averageWeight;
	std::vector<std::shared_ptr<Neuron>> _children;
	float _riskFactor;
	std::unique_ptr<Board> _board;
};

#endif