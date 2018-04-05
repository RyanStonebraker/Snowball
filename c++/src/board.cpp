/*
board.cpp
Snowball AI
CS 405 - Dr. Genetti
Contains function definitions for board class
*/

#include "board.h"
#include "constants.h"

std::ostream & operator<< (std::ostream & output, const Board & b)
{
	output << "High Level String: " << b._boardStateString << " Bit String - a: " << b._boardStateBits._64a << " Bit String - b: " << b._boardStateBits._64b;
	return output;
}

bool operator==(const Board & lhs, const Board & rhs)
{
	return lhs.getBoardStateString() == rhs.getBoardStateString();
}

bool operator!=(const Board & lhs, const Board & rhs)
{
	return !(lhs == rhs);
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

int Board::getQuality() const
{
	return _quality;
}

int Board::getRedKingCount() const
{
	return _redKingCount;
}

int Board::getBlackKingCount() const
{
	return _blackKingCount;
}

int Board::getBlackPieceCount() const
{
	return _blackPieceCount;
}

int Board::getRedPieceCount() const
{
	return _redPieceCount;
}

int Board::getMovesAvailable() const
{
	return 0;
}

// NOTE: This function just needs to be implemented, should work
// size_t Board::getRedOpenSpace() const {
// 	size_t openSpaces = 0;
// 	for (auto position = 0; position < _boardStateString.size(); ++position) {
// 		if (_boardStateString[position] == '0') {
// 			auto enemyAtBottomRight = (position+4)%4 == position % 4 && position+4 == '2' && position+4 == '4';
// 			auto enemyAtBottomLeft = (position+3)%4 < position % 4 && position+3 == '2' && position+3 == '4';
// 			auto enemyAtTopRight = (position-4)%4 == position % 4 && position-4 == '2' && position-4 == '4';
// 			auto enemyAtTopLeft = (position-5)%4 < position % 4 && position-5 == '2' && position-5 == '4';
//
// 			if (!enemyAtBottomRight && !enemyAtBottomLeft && !enemyAtTopLeft && !enemyAtTopRight)
// 				++openSpaces;
// 		}
// 	}
// 	return openSpaces;
// }

// size_t Board::getWhiteOpenSpace() const {
// 	size_t openSpaces = 0;
// 	for (auto i = 0; i < _boardStateString.size(); ++i) {
// 		if (_boardStateString[i] == '0') {
// 			int topLeft = ((i / 4) % 2) == 0 ? i - 5 : i - 4;
// 			int topRight = ((i / 4) % 2) == 0 ? i - 4 : i - 3;
// 			int bottomLeft = ((i / 4) % 2) == 0 ? i + 3 : i + 4;
// 			int bottomRight = ((i / 4) % 2) == 0 ? i + 4 : i + 5;
//
//
// 		}
// 	}
// 	return openSpaces;
// }


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
	case RED: // red
		boardStateBitsSeg += (bitFactor >> 2);
		break;

	case BLACK: // black
		boardStateBitsSeg += (bitFactor >> 1);
		break;

	case RED_KING: // red king
		boardStateBitsSeg += (bitFactor >> 1);
		boardStateBitsSeg += (bitFactor >> 2);
		break;

	case BLACK_KING: // black king
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

	_redKingCount = 0;
	_redPieceCount = 0;
	_blackKingCount = 0;
	_blackPieceCount = 0;

	for (unsigned int i = 0; i < _boardStateString.length(); i++)
	{
		_board.push_back(_boardStateString[i] - '0');

		if (i <= 18 && _board[i] != 0)
			convertToBits(_board[i], (bitFactor << (2 + 3 * i)));
		else
			convertToBits(_board[i], (bitFactor << (2 + 3 * (i - 19))), true);

		if (_board[i] == RED)
			_redPieceCount++;
		else if (_board[i] == RED_KING)
		{
			_redPieceCount++;
			_redKingCount++;
		}
		else if (_board[i] == BLACK)
			_blackPieceCount++;
		else if (_board[i] == BLACK_KING)
		{
			_blackPieceCount++;
			_blackKingCount++;
		}
	}
}
