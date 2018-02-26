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

void Neuron::spawnChildrenBlackFirst(int depth) {
	auto firstMoveSet = spawnBlack(*_board);

	#pragma omp parallel for
	for (unsigned possMove = 0; possMove < firstMoveSet.size(); ++possMove) {
		this->_children.push_back(make_shared<Neuron>(firstMoveSet[possMove]));

		this->_children[possMove] = recurseSpawning(depth, this->_children[possMove]);
	}

}

void Neuron::spawnChildrenRedFirst(int depth) {
	auto firstMoveSet = spawnRedOnce(*_board);

	#pragma omp parallel for
	for (unsigned possMove = 0; possMove < firstMoveSet.size(); ++possMove) {
		this->_children.push_back(make_shared<Neuron>(firstMoveSet[possMove]));

		this->_children[possMove] = recurseSpawning(depth, this->_children[possMove]);
	}

}

std::shared_ptr<Neuron> Neuron::recurseSpawning(int depth, std::shared_ptr<Neuron> parentNeuron) {
	auto rMoves = spawnRed(*parentNeuron->_board);
	--depth;

	#pragma omp parallel for
	for (auto & rMove : rMoves) {
		parentNeuron->_children.push_back(make_shared<Neuron>(rMove));
		if (depth > 0)
			parentNeuron->_children[parentNeuron->_children.size() - 1] = recurseSpawning(depth - 1, parentNeuron->_children[parentNeuron->_children.size() - 1]);
	}

	return parentNeuron;
}

vector<Board> Neuron::spawnBlack (Board & initBoard) {
		MoveGenerator moveGen;
		return moveGen.generateRandomMoves(initBoard);
}

vector<Board> Neuron::spawnRedOnce (Board & initBoard) {
		MoveGenerator moveGen;
		return _flipColor(moveGen.generateRandomMoves(initBoard));
}

vector<Board> Neuron::spawnRed (Board & rBoard) {
	MoveGenerator moveGen;
	auto rMoves = _flipColor(spawnBlack(rBoard));

	vector <Board> bMoves;
	#pragma omp parallel for
	for (auto & firstMove : rMoves) {
		auto childBoard = moveGen.generateRandomMoves(firstMove);
		for (auto & child : childBoard) {
			bMoves.push_back(child);
		}
	}

	return _flipColor(bMoves);
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
