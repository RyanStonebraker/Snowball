#include "catch.hpp"
#include "WeightedNode.h"
#include "IOHandler.h"
#include <fstream>
#include "BranchTracker.h"
#include <vector>
#include "board.h"
#include <chrono>
using std::chrono::system_clock;
using std::chrono::duration;
#include "moveGenerator.h"

TEST_CASE ("Basic Gameplay") {
  WeightedNode currentWeights;
  currentWeights.kingingWeight = 0.5;
  currentWeights.qualityWeight = 0.5;
  currentWeights.availableMovesWeight = 0.5;
  currentWeights.riskFactor = 0.5;
  currentWeights.enemyFactor = 0.5;
  currentWeights.depth = 2;

  SECTION ("AI goes second as black") {
    SECTION ("Basic IO") {
    IOHandler ioHandler(true);
    std::ofstream firstHandshakeWriter (ioHandler.getFirstHandshakeLocation().c_str(), std::ofstream::trunc);
    firstHandshakeWriter << "1";
    firstHandshakeWriter.flush();

    ioHandler.startGame(ioHandler.setupGame());

    std::ifstream checkHandshake(ioHandler.getSecondHandshakeLocation());
    int handshakeCheck = -1;
    checkHandshake >> handshakeCheck;

    SECTION ("Valid Handshake") {
      REQUIRE(handshakeCheck == 1);
    }

    std::ofstream writeZeroTurn (ioHandler.getCurrentGameDirectory() + "turn0.txt");

    writeZeroTurn << "11111111111100000000222222222222";
    writeZeroTurn.flush();

    std::ofstream writeFirstTurn (ioHandler.getCurrentGameDirectory() + "turn1.txt");

    std::string first_board = "11111111110110000000222222222222";
    writeFirstTurn << first_board;
    writeFirstTurn.flush();

    ioHandler.updateFileName();
    Board initBoard(ioHandler.readBoardState());

    }

    std::string first_board = "11111111110110000000222222222222";
    Board initBoard(first_board);

    REQUIRE(initBoard.getBoardStateString() == first_board);

    BranchTracker potentialBranch(initBoard, currentWeights);
    Board nextMove = potentialBranch.getBestMove(BranchTracker::Color::BLACK_PIECE);

    std::vector <Board> validMoves {nextMove};
    std::string boardToWrite = validMoves[0].getBoardStateString();

    SECTION ("Move Generated") {
      REQUIRE(validMoves.size() == 1);

      SECTION ("Valid Move Generated") {
        SECTION ("Wrote all pieces") {
          REQUIRE (boardToWrite.size() == first_board.size());
        }

        SECTION ("Didn't move red") {
          REQUIRE (boardToWrite.substr(0, 13) == first_board.substr(0, 13));
        }

        SECTION ("Moved white") {
          REQUIRE(boardToWrite.substr(13, 32) != first_board.substr(13,32));
        }

        SECTION ("Valid Piece Count") {
          REQUIRE(validMoves[0].getRedPieceCount() == 12);
          REQUIRE(validMoves[0].getBlackPieceCount() == 12);
          REQUIRE(validMoves[0].getRedKingCount() == 0);
          REQUIRE(validMoves[0].getBlackKingCount() == 0);
        }
      }
    }

    SECTION ("Timing") {
      SECTION ("Within time limit") {
        WeightedNode timedWeights;
        timedWeights.kingingWeight = 0.5;
        timedWeights.qualityWeight = 0.5;
        timedWeights.availableMovesWeight = 0.5;
        timedWeights.riskFactor = 0.5;
        timedWeights.enemyFactor = 0.5;
        SECTION ("DEPTH OF 2") {
          timedWeights.depth = 2;

          MoveGenerator::totalMoveGens = 0;

          auto startWeightTwo = system_clock::now();

          BranchTracker potentialBranch(initBoard, timedWeights);
          Board nextMove = potentialBranch.getBestMove(BranchTracker::Color::BLACK_PIECE);

          auto endWeightTwo = system_clock::now();
          duration<double> elapsedMoveGenTime = endWeightTwo - startWeightTwo;

          REQUIRE (elapsedMoveGenTime.count() < 10);

          SECTION ("Boards Per Second >= 100,000") {
            double bps = MoveGenerator::totalMoveGens/elapsedMoveGenTime.count();
            REQUIRE (bps >= 100000.0);
          }
        }

        SECTION ("DEPTH OF 3") {
          timedWeights.depth = 3;

          MoveGenerator::totalMoveGens = 0;

          auto startWeightTwo = system_clock::now();

          BranchTracker potentialBranch(initBoard, timedWeights);
          Board nextMove = potentialBranch.getBestMove(BranchTracker::Color::BLACK_PIECE);

          auto endWeightTwo = system_clock::now();
          duration<double> elapsedMoveGenTime = endWeightTwo - startWeightTwo;

          REQUIRE (elapsedMoveGenTime.count() < 10);

          SECTION ("Boards Per Second >= 100,000") {
            REQUIRE (MoveGenerator::totalMoveGens/elapsedMoveGenTime.count() >= 100000);
          }
        }

        SECTION ("DEPTH OF 5") {
          timedWeights.depth = 5;

          MoveGenerator::totalMoveGens = 0;

          auto startWeightTwo = system_clock::now();

          BranchTracker potentialBranch(initBoard, timedWeights);
          Board nextMove = potentialBranch.getBestMove(BranchTracker::Color::BLACK_PIECE);

          auto endWeightTwo = system_clock::now();
          duration<double> elapsedMoveGenTime = endWeightTwo - startWeightTwo;

          REQUIRE (elapsedMoveGenTime.count() < 10);

          SECTION ("Boards Per Second >= 100,000") {
            REQUIRE (MoveGenerator::totalMoveGens/elapsedMoveGenTime.count() >= 100000);
          }
        }
      }
    }
  }

  SECTION ("AI goes first as red") {
    IOHandler ioHandler(true);
    std::ofstream firstHandshakeWriter (ioHandler.getFirstHandshakeLocation());
    firstHandshakeWriter << 0;
    firstHandshakeWriter.flush();

    ioHandler.startGame(ioHandler.setupGame());

    std::ifstream checkHandshake(ioHandler.getSecondHandshakeLocation());
    int handshakeCheck = -1;
    checkHandshake >> handshakeCheck;

    SECTION ("Valid Handshake") {
      REQUIRE(handshakeCheck == 1);
    }
  }
}
