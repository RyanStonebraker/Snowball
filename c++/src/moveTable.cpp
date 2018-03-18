//
//  moveTable.cpp
//  Snowball AI
//
//  Created by Thatcher Lane on 1/26/18.
//  Copyright © 2018 Thatcher Lane. All rights reserved.
//

#include "moveTable.h"
#include "board.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
using std::string;

MoveTable::MoveTable() {
    for(int i = 0; i < tableSize; i++){
        hashTable[i] = new move;
        hashTable[i]->position = "empty";
        hashTable[i]->moveOne = -1;
        hashTable[i]->moveTwo = -1;
        hashTable[i]->next = NULL;
    }
}

int MoveTable::hash(std::string key)
{
    int hash = 0;
    int index;

    index = key.length();

    for(int i = 0; i < key.length(); i++) {
        hash += (int)key[i];
    }

    index = hash % tableSize;

    return index;
}

void MoveTable::addMove(string position, int moveOne, int moveTwo){
    int index = hash(position);

    if(hashTable[index]->position == "empty"){
        hashTable[index]->position = position;
        hashTable[index]->moveOne = moveOne;
        hashTable[index]->moveTwo = moveTwo;
    }
    else{
        move* Ptr = hashTable[index];
        move* n = new move;
        n->position = position;
        n->moveOne = moveOne;
        n->moveTwo = moveTwo;
        n->next = NULL;
        while (Ptr->next != NULL) {
            Ptr = Ptr->next;
        }
        Ptr->next = n;
    }
}

int MoveTable::numberOfItemsInIndex(int index){
    int count = 0;

    if(hashTable[index]->position == "empty"){
        return count;
    }
    else{
        count++;
        move* Ptr = hashTable[index];
        while (Ptr->next != NULL) {
            count++;
            Ptr = Ptr->next;
        }
    }
    return count;
}

void MoveTable::printTable(){
    int number;
    for (int i = 0 ; i < tableSize; i++) {
        number = numberOfItemsInIndex(i);
        std::cout << "index: " << i << std::endl;
        std::cout << hashTable[i]->position << std::endl;
        std::cout << hashTable[i]->moveOne << std::endl;
        std::cout << hashTable[i]->moveTwo << std::endl;
        std::cout << "# of items = " << number << std::endl << std::endl;
    }
}

void MoveTable::printItemsInIndex(int index){
    move* Ptr = hashTable[index];

    if(Ptr->position == "empty"){
        std::cout << "index " << index << "is empty" << std::endl;
    }
    else{
        std::cout << "index: " << index << " contains the following moves\n\n";
        while (Ptr != NULL) {
            std::cout << "Position: "<< Ptr->position << std::endl;
            std::cout << Ptr->moveOne << std::endl;
            std::cout << Ptr->moveTwo << std::endl << std::endl;
            Ptr = Ptr->next;
        }
    }
}

std::pair<int, int> MoveTable::findMove(string position){
    std::pair<int, int> movePair;
    int index = hash(position);
    bool foundName = false;
    int moveOne;
    int moveTwo;

    move* Ptr = hashTable[index];
    while (Ptr != NULL) {
        if(Ptr->position == position){
            foundName = true;
            moveOne = Ptr->moveOne;
            moveTwo = Ptr->moveTwo;
        }
        Ptr = Ptr->next;
    }

    if (foundName == true) {
        movePair.first = moveOne;
        movePair.second = moveTwo;
    }

    else{
        std::cout << "Position " << position << " was not found in the move table\n";
    }

    return movePair;
}

void MoveTable::removeItem(string position){
    int index = hash(position);

    move* delPtr;
    move* p1;
    move* p2;

    //case 0 - bucket is empty
    if(hashTable[index]->position=="empty" && hashTable[index]->moveOne==-1 && hashTable[index]->moveTwo==-1)
    {
        std::cout << "Position " << position << " was not found in the move table \n";
    }

    //case 1 - only 1 item contained in the bucket and that item has matching position
    else if(hashTable[index]->position == position && hashTable[index]->next == NULL)
    {
        hashTable[index]->position="empty";
        hashTable[index]->moveOne=-1;
        hashTable[index]->moveOne=-1;
    }

    //case 2 - match is located in the first item in the bucket but there are more items in the bucket
    else if (hashTable[index]->position == position)
    {
        delPtr = hashTable[index];
        hashTable[index] = hashTable[index]->next;
        delete delPtr;

        std::cout << position << " was removed from the table\n";
    }

    //case 3 - bucket contains item but first item is not a match
    else
    {
        p1 = hashTable[index]->next;
        p2 = hashTable[index];

        while (p1 != NULL && p1->position != position) {
            p2 = p1;
            p1 = p1->next;
        }

        // case 3.1 - no match
        if (p1 == NULL)
        {
            std::cout << "Position " << position << " was not found in the move table \n";
        }

        // case 3.2 - match is found
        else
        {
            delPtr = p1;
            p1 = p1->next;
            p2->next = p1;

            delete delPtr;

            std::cout << position << " was removed from the table\n";
        }
    }
}






void MoveTable::generateShadowState(string currentBoardState, MoveTable blackTable, MoveTable redTable){
    std::ofstream shadowState;
    shadowState.open("shadowstate.txt", std::fstream::app);

    for (int i=0; i<currentBoardState.length(); i++) {
        char piece = currentBoardState[i];
        std::pair<int, int> movesPair;

        switch (piece) {
            //blank space
            case '0':
                shadowState << piece << " " << -1 << std::endl;
                break;

            //black piece
            case '1':
                movesPair = blackTable.findMove(std::to_string(i+1));
                if (movesPair.first == -1 && movesPair.second == -1 ) {
                    shadowState << piece << " " << -1 << std::endl;
                }
                else if (movesPair.first == -1) {
                    shadowState << piece << " " << movesPair.second << std::endl;
                }
                else if(movesPair.second == -1) {
                    shadowState << piece << " " << movesPair.first << std::endl;
                }
                else {
                    shadowState << piece << " " << movesPair.first << movesPair.second << std::endl;
                }
                break;

            //red piece
            case '2':
                movesPair = redTable.findMove(std::to_string(i+1));
                if (movesPair.first == -1 && movesPair.second == -1 ) {
                    shadowState << piece << " " << -1 << std::endl;
                }
                else if (movesPair.first == -1) {
                    shadowState << piece << " " << movesPair.second << std::endl;
                }
                else if(movesPair.second == -1) {
                    shadowState << piece << " " << movesPair.first << std::endl;
                }
                else {
                    shadowState << piece << " " << movesPair.first << movesPair.second << std::endl;
                }
                break;

            // Black King
            case '3':
                movesPair = blackTable.findMove(std::to_string(i+1));
                if (movesPair.first == -1 && movesPair.second == -1 ) {
                    shadowState << piece << " ";
                }
                else if (movesPair.first == -1) {
                    shadowState << piece << " " << movesPair.second;
                }
                else if(movesPair.second == -1) {
                    shadowState << piece << " " << movesPair.first;
                }
                else {
                    shadowState << piece << " " << movesPair.first << movesPair.second;
                }

                movesPair = redTable.findMove(std::to_string(i+1));
                if (movesPair.first == -1 && movesPair.second == -1 ) {
                    shadowState << std::endl;
                }
                else if (movesPair.first == -1) {
                    shadowState << movesPair.second << std::endl;
                }
                else if(movesPair.second == -1) {
                    shadowState << movesPair.first << std::endl;
                }
                else {
                    shadowState << movesPair.first << movesPair.second << std::endl;
                }
                break;

            //Red King
            case '4':
                movesPair = blackTable.findMove(std::to_string(i+1));
                if (movesPair.first == -1 && movesPair.second == -1 ) {
                    shadowState << piece << " ";
                }
                else if (movesPair.first == -1) {
                    shadowState << piece << " " << movesPair.second;
                }
                else if(movesPair.second == -1) {
                    shadowState << piece << " " << movesPair.first;
                }
                else {
                    shadowState << piece << " " << movesPair.first << movesPair.second;
                }

                movesPair = redTable.findMove(std::to_string(i+1));
                if (movesPair.first == -1 && movesPair.second == -1 ) {
                    shadowState << std::endl;
                }
                else if (movesPair.first == -1) {
                    shadowState << movesPair.second << std::endl;
                }
                else if(movesPair.second == -1) {
                    shadowState << movesPair.first << std::endl;
                }
                else {
                    shadowState << movesPair.first << movesPair.second << std::endl;
                }
                break;


            default:
                break;
        }
    }
}

string MoveTable::findShadowState(string position){
    return position;
}

void MoveTable::updateTables(){


}
