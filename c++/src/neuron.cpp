#include "neuron.h"
#include "moveGenerator.h"
#include <vector>
#include <memory>
using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;
using std::make_unique;
using std::weak_ptr;
#include <iostream>

Neuron::Neuron(Board &board) {
	_board = make_unique<Board>(board);
}

Neuron::Neuron(float weight, float averageWeight, std::vector<std::shared_ptr<Neuron>> children, float riskFactor, Board board) 
	: _weight(weight), _averageWeight(averageWeight), _children(children), _riskFactor(riskFactor)
{
	_board = make_unique<Board>(board);
}

Neuron::Neuron(const Neuron & other)
{
	_weight = other.getWeight();
	_averageWeight = other.getAverageWeight();
	_children = other.getChildren();
	_riskFactor = other.getRiskFactor();
	_board = make_unique<Board>(other.getBoard());
}

Neuron & Neuron::operator=(const Neuron & other)
{
	Neuron temp = { other.getWeight(), 
					other.getAverageWeight(), 
					other.getChildren(),
					other.getRiskFactor(), 
					other.getBoard() };

	return temp;
}

unsigned int evals = 0;

void Neuron::spawnChildren(int depth) {
	std::cout << "Spawning children at level " << depth << "\n";
	std::cout << "Boards generated: " << evals << "\n";
	MoveGenerator moveGenerator;
	auto validMoves = moveGenerator.generateRandomMoves(*_board.get());
	for (auto board : validMoves) {
		evals++;
		_children.push_back(make_shared<Neuron>(board));
	}
	depth--;
	if (depth > 0) {
		for (auto child : _children) {
			child->spawnChildren(depth);
		}
	}
}

//Accessors for indirect board interfacing

int Neuron::getQuality() const
{
	return _board->getQuality();
}

int Neuron::getRedKingCount() const
{
	return _board->getRedKingCount();
}

int Neuron::getBlackKingCount() const
{
	return _board->getBlackKingCount();
}

int Neuron::getBlackPieceCount() const
{
	return _board->getBlackPieceCount();
}

int Neuron::getRedPieceCount() const
{
	return _board->getRedPieceCount();
}

//Accessors for Neuron

float Neuron::getWeight() const
{
	return _weight;
}

float Neuron::getAverageWeight() const
{
	return _averageWeight;
}

std::vector<std::shared_ptr<Neuron>> Neuron::getChildren() const
{
	return _children;
}

float Neuron::getRiskFactor() const
{
	return _riskFactor;
}

Board Neuron::getBoard() const
{
	return *_board;
}

size_t Neuron::size() const
{
	return _children.size();
}

Neuron & Neuron::operator[](int index)
{
	return *_children[index];
}

const Neuron & Neuron::operator[](int index) const
{
	return *_children[index];
}

bool operator==(const Neuron & lhs, const Neuron & rhs)
{
	return ((lhs.getWeight() == rhs.getWeight()) &&
		(lhs.getAverageWeight() == rhs.getAverageWeight()) &&
		(lhs.getChildren() == rhs.getChildren()) &&
		(lhs.getRiskFactor() == rhs.getRiskFactor()) &&
		(lhs.getBoard() == rhs.getBoard()));
}

bool operator!=(const Neuron & lhs, const Neuron & rhs)
{
	return !(lhs == rhs);
}
