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