#include "neuron.h"
#include "moveGenerator.h"
#include "constants.h"
#include <vector>
#include <memory>
using std::shared_ptr;
using std::make_shared;
using std::weak_ptr;
#include <iostream>
using std::vector;
#include <string>
using std::string;


vector<Board> _flipColor(vector<Board> validMoves)
{
	//std::move
	std::transform(validMoves.begin(), validMoves.end(), validMoves.begin(), [](Board & b) {

		string flippedBoard = "";
		string boardString = b.getBoardStateString();
		int quality = b.getQuality();

		std::reverse(boardString.begin(), boardString.end());

		b = { boardString, quality };

		for (auto & n : b.getBoardStateString())
		{
			int piece = n - '0';

			if (piece == BLACK)
				flippedBoard += '1';
			else if (piece == RED)
				flippedBoard += '2';
			else if (piece == BLACK_KING)
				flippedBoard += '3';
			else if (piece == RED_KING)
				flippedBoard += '4';
			else
				flippedBoard += '0';
		}

		Board temp(flippedBoard, quality);
		return temp;

	});

	return validMoves;
}

Neuron::Neuron(Board &board) {
	_board = make_shared<Board>(board);
}

Neuron::Neuron(float weight, float averageWeight, std::vector<std::shared_ptr<Neuron>> & children, float riskFactor, Board board)
	: _weight(weight), _averageWeight(averageWeight), _children(children), _riskFactor(riskFactor)
{
	_board = make_shared<Board>(board);
}

Neuron::Neuron(Neuron & other)
{
	_weight = other.getWeight();
	_averageWeight = other.getAverageWeight();
	_children = other.getChildren();
	_riskFactor = other.getRiskFactor();
	_board = other._board;
}

Neuron & Neuron::operator=(Neuron & other)
{
	_weight = other.getWeight();
	_averageWeight = other.getAverageWeight();
	_children = other.getChildren();
	_riskFactor = other.getRiskFactor();
	_board = other._board;

	return *this;
}

unsigned int evals = 0;
bool flipOccured = false;

void Neuron::spawnChildren(int depth, bool flipColorFlag) {
	// std::cout << "Spawning children at level " << depth << "\n";
	// std::cout << "Boards generated: " << evals << "\n";

	if (flipOccured)
		*_board = _flipColor({ *_board })[0];

	MoveGenerator moveGenerator;
	auto validMoves = moveGenerator.generateRandomMoves(*_board);
	if (!flipColorFlag)
	{
		for (auto board : validMoves) {
			evals++;
			_children.push_back(make_shared<Neuron>(board));
		}
		depth--; 
		if (depth > 0) {
			for (auto child : _children) {
				flipOccured = false;
				child->spawnChildren(depth, !flipColorFlag);
			}
		}
	}
	else
	{
		auto flippedValidMoves = _flipColor(validMoves); //flip the board

		for (auto flipBoard : flippedValidMoves) {
			evals++;
			_children.push_back(make_shared<Neuron>(flipBoard)); //push flipped boards back as children
		}

		if (depth > 0) {
			for (auto flipChild : _children) {
				flipOccured = true;
				flipChild->spawnChildren(depth, !flipColorFlag);
			}
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

std::vector<std::shared_ptr<Neuron>> Neuron::getChildren()
{
	return _children;
}

float Neuron::getRiskFactor() const
{
	return _riskFactor;
}

Board Neuron::getBoard()
{
	return *_board;
}

size_t Neuron::size() const
{
	return _children.size();
}

std::shared_ptr<Neuron> & Neuron::operator[](int index)
{
	return _children[index];
}

bool operator==(Neuron & lhs, Neuron & rhs)
{
	return ((lhs.getWeight() == rhs.getWeight()) &&
		(lhs.getAverageWeight() == rhs.getAverageWeight()) &&
		(lhs.getChildren() == rhs.getChildren()) &&
		(lhs.getRiskFactor() == rhs.getRiskFactor()) &&
		(lhs.getBoard() == rhs.getBoard()));
}

bool operator!=(Neuron & lhs, Neuron & rhs)
{
	return !(lhs == rhs);
}
