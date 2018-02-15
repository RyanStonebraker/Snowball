#ifndef NEURON_H_INCLUDED
#define NEURON_H_INCLUDED
#include "board.h"
#include <vector>
#include <memory>

class Neuron {

	//Equality overloads
	friend bool operator==(Neuron & lhs, Neuron & rhs);
	friend bool operator!=(Neuron & lhs, Neuron & rhs);

public:
	Neuron() = default;
	Neuron(Board &board);
	Neuron(float, float, std::vector<std::shared_ptr<Neuron>> &, float, Board);
	Neuron(Neuron & other);
	Neuron & operator=(Neuron & other);
	void spawnChildren(int depth);
	void updateAverageWeights();
	size_t size() const;

	int getQuality() const;
	int getRedKingCount() const;
	int getBlackKingCount() const;
	int getBlackPieceCount() const;
	int getRedPieceCount() const;

	// Accessors for Neuron
	float getWeight() const;
	float getAverageWeight() const;
	std::vector<std::shared_ptr<Neuron>> getChildren();
	float getRiskFactor() const;
	Board getBoard();

	//[] overloads
	std::shared_ptr<Neuron> & operator[](int index);

private:
	float _weight;
	float _averageWeight;
	std::vector<std::shared_ptr<Neuron>> _children;
	float _riskFactor;
	std::shared_ptr<Board> _board;
};

#endif
