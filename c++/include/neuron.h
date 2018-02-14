#ifndef NEURON_H_INCLUDED
#define NEURON_H_INCLUDED
#include "board.h"
#include <vector>
#include <memory>

class Neuron {

	//Equality overloads
	friend bool operator==(const Neuron & lhs, const Neuron & rhs);
	friend bool operator!=(const Neuron & lhs, const Neuron & rhs);

public:
	Neuron() = default;
	Neuron(Board &board);
	Neuron(float, float, const std::vector<std::shared_ptr<Neuron>> &, float, Board);
	Neuron(const Neuron & other);
	Neuron & operator=(const Neuron & other);
	void spawnChildren(int depth);
	void updateAverageWeights();
	size_t size() const;

	//Accessors for indirect board interfacing
	int getQuality() const;
	int getRedKingCount() const;
	int getBlackKingCount() const;
	int getBlackPieceCount() const;
	int getRedPieceCount() const;

	//Accessors for Neuron
	float getWeight() const;
	float getAverageWeight() const;
	std::vector<std::shared_ptr<Neuron>> getChildren() const;
	float getRiskFactor() const;
	Board getBoard() const;

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
