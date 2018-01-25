/*
main.cpp
Snowball AI
CS 405 - Dr. Genetti
All main functions that pertain to generating valid moves will be
called from here to keep things organized.
*/

#include "backend.h"
#include "board.h"
#include <iostream>

int main()
{
	board b(readBoardState());

	std::cout << b << std::endl;

	std::cin.get();
	return 0;
}