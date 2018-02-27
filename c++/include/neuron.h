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
	void spawnChildrenBlackFirst(int depth);
	void spawnChildrenRedFirst(int depth);
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

    void setWeight(float weight);
    void setAverageWeight(float averageWeight);
    void setRiskFactor(float riskFactor);

	//[] overloads
	std::shared_ptr<Neuron> & operator[](int index);
    friend std::ostream & operator<<(std::ostream&, const Neuron&);
    friend std::istream& operator>>(std::istream &, Neuron&);

    //transforms for new class
    float randomNumber(float Min, float Max);
    float kingWeightPrime(float oldKingWeight);
    float sigmaWeightPrime(float oldSigmaWeight, int numberOfWeights);
    float nodeWeightPrime(float oldNodeWeight, float sigmaWeight);
    Neuron& evolveChild(Neuron&);

private:
	std::vector<Board> spawnBlack (Board & initBoard);
	std::vector<Board> spawnRed (Board & rBoard);
	std::vector<Board> spawnRedOnce (Board & rBoard);
	std::shared_ptr<Neuron> recurseSpawning(int depth, std::shared_ptr<Neuron> parentNeuron);

	float _weight;
	float _averageWeight;
	std::vector<std::shared_ptr<Neuron>> _children;
	float _riskFactor;
	std::shared_ptr<Board> _board;
};

#endif
