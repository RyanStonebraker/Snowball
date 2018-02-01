#include "catch.hpp"
#include "board.h"
#include <vector>

TEST_CASE("string in == string out") {
	Board board("11111111111100000000222222222222");
	REQUIRE(board.getBoardStateString() == "11111111111100000000222222222222");
}

TEST_CASE("board change") {
	Board board("11111111111100000000222222222222");
	board.setBoardStateString("10203401020002004430001230004123");
	REQUIRE(board.getBoardStateString() == "10203401020002004430001230004123");		
}

TEST_CASE("convert to array") {
	Board board("10203401020002004430001230004123");
	auto arr = board.getBoardArray();
	std::vector<int> v = {1,0,2,0,3,4,0,1,0,2,0,0,0,2,0,0,4,4,3,0,0,0,1,2,3,0,0,0,4,1,2,3};
	REQUIRE(arr == v);
}

TEST_CASE("[] operator") {
	Board board("30000000000000000000000000000004");
	REQUIRE(board[0] == 3);
	REQUIRE(board[31] == 4);
}