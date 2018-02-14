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

size_t Neuron::size() const
{
	return _children.size();
}

std::shared_ptr<Neuron> & Neuron::operator[](int index)
{
	return _children[index];
}

const std::shared_ptr<Neuron> & Neuron::operator[](int index) const
{
	return _children[index];
}