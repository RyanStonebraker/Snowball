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

class board
{

friend std::ostream & operator<< (std::ostream & output, const board & b)
{
	output << "High Level String: " << b._boardStateString << " Bit String: " << b._boardStateBits;
	return output;
}

public:

	board(const std::string & boardState) : _boardStateString(boardState) , _boardStateBits(0)
	{
		unsigned long long int bitFactor = 1;

		for (int i = 0; i < boardState.length(); i++)
		{
			_board[i] = boardState[i] - '0';
			convertToBits(_board[i], (bitFactor << (2 + 3 * i)));
		}
	}

	~board() 
	{
		delete[] _board;
	}

	void convertToBits(const int & boardVal, const unsigned int & bitFactor)
	{
		switch (boardVal)
		{
		case 0: // empty
			
		break;

		case 1: // red
			_boardStateBits += (bitFactor >> 2);
		break;

		case 2: // black
			_boardStateBits += (bitFactor >> 1);
		break;

		case 3: // red king
			_boardStateBits += (bitFactor >> 1);
			_boardStateBits += (bitFactor >> 2);
		break;

		case 4: // black king
			_boardStateBits += bitFactor;
		break;
		}
	}


private:
	std::string _boardStateString;
	unsigned long long int _boardStateBits;
	int _board[32];
	
	
};

#endif // !FILE_BOARD_H_INCLUDED