#ifndef NEURON_H_INCLUDED
#define NEURON_H_INCLUDED
#include "board.h"
#include <vector>
#include <memory>

class Neuron {
public:
	Neuron() = default;
	Neuron(Neuron & rhs) :  _weight(rhs._weight), _averageWeight(rhs._averageWeight),
												 _children(rhs._children),_riskFactor(rhs._riskFactor), _board(rhs._board){}
	Neuron(Board &board);
	void spawnChildren(int depth);
	void updateAverageWeights();
	int getQuality() const;
	int getRedKingCount() const;
	int getBlackKingCount() const;
	int getBlackPieceCount() const;
	int getRedPieceCount() const;
	Board getBoard();
	size_t size() const;

	//[] overloads
	std::shared_ptr<Neuron> & operator[](int index);
	const std::shared_ptr<Neuron> & operator[](int index) const;

private:
	float _weight;
	float _averageWeight;
	std::vector<std::shared_ptr<Neuron>> _children;
	float _riskFactor;
	std::shared_ptr<Board> _board;
};

#endif
