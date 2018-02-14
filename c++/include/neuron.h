#ifndef NEURON_H_INCLUDED
#define NEURON_H_INCLUDED
#include "board.h"
#include <vector>
#include <memory>

class Neuron {
public:
	Neuron() = default;
	Neuron(Board &board);
	void spawnChildren(int depth);
	void updateAverageWeights();
	int getQuality() const;
	int getRedKingCount() const;
	int getBlackKingCount() const;
	int getBlackPieceCount() const;
	int getRedPieceCount() const;
	size_t size() const;

	//[] overloads
	Neuron & operator[](int index);
	const Neuron & operator[](int index) const;

private:
	float _weight;
	float _averageWeight;
	std::vector<std::shared_ptr<Neuron>> _children;
	float _riskFactor;
	std::unique_ptr<Board> _board;
};

#endif
