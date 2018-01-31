/*
board.h
Snowball AI
CS 405 - Dr. Genetti
Contains function prototypes for board class
*/


#ifndef FILE_BOARD_H_INCLUDED
#define FILE_BOARD_H_INCLUDED

#include <string>
#include <iostream>
#include <emmintrin.h>
#include <algorithm>
#include <vector>

struct _96Bit{
	unsigned long long int _64a = 0;
	unsigned long long int _64b = 0;
};

class Board
{
	//stream insertion overload
	friend std::ostream & operator<< (std::ostream & output, const Board & b);

public:

	//ctor
	Board(const std::string & boardState) : _boardStateString(boardState) { updateBoard(); }

	//dctor
	~Board() = default;

	//copy ctor
	Board(const Board & other) = default;

	//copy assignment
	Board & operator=(const Board & other) = default;

	//move ctor
	Board(Board && other) = default;

	//move assignment
	Board & operator=(Board && other) = default;
	

	//retrieval functions
	_96Bit getBoardStateBits() const; 
	std::vector<int> getBoardArray() const; 
	std::string getBoardStateString() const;

	//mutator functions
	void setBoardStateString(const std::string & newState);

	//[] overloads
	int & operator[](int index);
	const int & operator[](int index) const;

private:

	//convertToBits
	//takes given board and produced a 96bit representation of it
	void convertToBits(const int & boardVal, const unsigned long long int & bitFactor, bool swapVar = false);

	//updateBoard
	//recalculates 96bit board representation based on _boardStateString
	void updateBoard();

private:

	std::string _boardStateString;
	//_declspec (align(16)) __m128i _boardStateBits;
	_96Bit _boardStateBits;
	std::vector<int> _board;

};

#endif // !FILE_BOARD_H_INCLUDED