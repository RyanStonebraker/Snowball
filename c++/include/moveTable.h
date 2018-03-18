//
//  moveTable.h
//  Snowball AI
//
//  Created by Thatcher Lane on 1/26/18.
//  Copyright © 2018 Thatcher Lane. All rights reserved.
//

#include <cstdlib>
#include <iostream>
#include <string>
using std::string;

#ifndef moveTable_h
#define moveTable_h

#include <stdio.h>

class MoveTable {

private:
    static const int tableSize = 32;

    struct move{
        string position;
        int moveOne;
        int moveTwo;
        move* next;
    };

    move* hashTable[tableSize];

public:
    MoveTable();
    static int hash(std::string key);
    void addMove(std::string position, int moveOne, int moveTwo);
    int numberOfItemsInIndex(int index);
    void printTable();
    void printItemsInIndex(int index);
    std::pair<int, int> findMove(string position);
    void removeItem(string position);

    void updateTables();
    void generateShadowState(string currentBoardState, MoveTable redTable, MoveTable blackTable);
    string findShadowState(string position);


};

#endif /* moveTable_h */
