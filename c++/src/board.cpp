/*
board.cpp
Snowball AI
CS 405 - Dr. Genetti
Contains function definitions for board class
*/

#include "board.h"

std::ostream & operator<< (std::ostream & output, const Board & b)
{
	output << "High Level String: " << b._boardStateString << " Bit String - a: " << b._boardStateBits._64a << " Bit String - b: " << b._boardStateBits._64b;
	return output;
}

_96Bit Board::getBoardStateBits() const
{
	return _boardStateBits;
}

std::vector<int> Board::getBoardArray() const
{
	return _board;
}

std::string Board::getBoardStateString() const
{
	return _boardStateString;
}

void Board::setBoardStateString(const std::string & newState)
{
	_boardStateString = newState;
	updateBoard();
}

int & Board::operator[](int index)
{
	return _board[index];
}

const int & Board::operator[](int index) const
{
	return _board[index];
}

void Board::convertToBits(const int & boardVal, const unsigned long long int & bitFactor, bool swapVar)
{
	unsigned long long int boardStateBitsSeg = _boardStateBits._64a;
	// unsigned long long int check = 1;

	if (swapVar)
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

void Board::updateBoard()
{
	unsigned long long int bitFactor = 1;

	for (unsigned int i = 0; i < _boardStateString.length(); i++)
	{
		_board.push_back(_boardStateString[i] - '0');

		if (i <= 18 && _board[i] != 0)
			convertToBits(_board[i], (bitFactor << (2 + 3 * i)));
		else
			convertToBits(_board[i], (bitFactor << (2 + 3 * (i - 19))), true);
	}
}
