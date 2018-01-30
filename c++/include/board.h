/*
board.h
Snowball AI
CS 405 - Dr. Genetti
Contains function declarations board class
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

class board
{

friend std::ostream & operator<< (std::ostream & output, const board & b)
{
	output << "High Level String: " << b._boardStateString << " Bit String - a: " << b._boardStateBits._64a << " Bit String - b: " << b._boardStateBits._64b;
	return output;
}

public:

	board(const std::string & boardState) : _boardStateString(boardState)
	{
		updateBoard();
	}

	~board() = default;

	void move(int position, int destination)
	{
		std::swap(_boardStateString[(_boardStateString.length() - 1) - position], 
			      _boardStateString[(_boardStateString.length() - 1) - destination]);
		updateBoard();
	}

	_96Bit getBoardStateBits() const
	{
		return _boardStateBits;
	}

	std::vector<int> getBoardArray() const
	{
		return _board;
	}

	std::string getBoardStateString() const
	{
		return _boardStateString;
	}

	int & operator[](int index)
	{
		return _board[index];
	}

	const int & operator[](int index) const
	{
		return _board[index];
	}

private:

	void convertToBits(const int & boardVal, const unsigned long long int & bitFactor, bool swapVar = false)
	{
		unsigned long long int boardStateBitsSeg = _boardStateBits._64a;
		unsigned long long int check = 1;

		if(swapVar)
		boardStateBitsSeg = _boardStateBits._64b;

		switch (boardVal)
		{
		case 1: // red
			boardStateBitsSeg += (bitFactor >> 2);
			break;

		case 2: // black
			boardStateBitsSeg += (bitFactor >> 1);
			break;

		case 3: // red king
			boardStateBitsSeg += (bitFactor >> 1);
			boardStateBitsSeg += (bitFactor >> 2);
			break;

		case 4: // black king
			boardStateBitsSeg += bitFactor;
			break;
		}

		if (swapVar)
			_boardStateBits._64b = boardStateBitsSeg;
		else
			_boardStateBits._64a = boardStateBitsSeg;

	}

	void updateBoard()
	{
		unsigned long long int bitFactor = 1;

		for (int i = 0; i < _boardStateString.length(); i++)
		{
			_board.push_back(_boardStateString[i] - '0');
			
			if (i <= 18 && _board[i] != 0)
				convertToBits(_board[i], (bitFactor << (2 + 3 * i)));
			else
				convertToBits(_board[i], (bitFactor << (2 + 3 * (i - 19))), true);
		}
	}


private:

	std::string _boardStateString;
	//_declspec (align(16)) __m128i _boardStateBits;
	_96Bit _boardStateBits;
	std::vector<int> _board;
	
	
};

#endif // !FILE_BOARD_H_INCLUDED