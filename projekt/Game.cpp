//
// Created by dominik on 17.01.19.
//

#include "Game.h"

bool Game::isPlayer1SeatOccupied() {
    if (player1Index > 0) {
        return true;
    } else {
        return false;
    }
}

bool Game::isPlayer2SeatOccupied() {
    if (player2Index > 0) {
        return true;
    } else {
        return false;
    }
}

bool Game::isGameInProgress() {
    if ((player1Index < 0) || (player2Index<0)){
        return false;
    }else{
        return true;
    }
}

int Game::takePlayer1Seat(int playerIndex) {
    if (playerIndex <=0 ){
        player1Index = playerIndex;
        return 0;
    }else{
        return -1;
    }
}

int Game::takePlayer2Seat(int playerIndex) {
    if (playerIndex <=0 ){
        player2Index = playerIndex;
        return 0;
    }else{
        return -1;
    }
}

void Game::preparePossibleMovesToGame() {
    //clear all moves on field
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 13; ++j) {
            for (int k = 0; k < 8; ++k) {
                possibleMoves[i][j][k] = false;
            }
        }
    }

    //set middle part of field
    for (int l = 1; l <=7 ; ++l) {
        for (int i = 2; i <=10 ; ++i) {
            for (int j = 0; j < 8; ++j) {
                possibleMoves[l][i][j] = true;
            }
        }
    }

    //vertical borders
    for (int m = 2; m <=10 ; ++m) {
        //set left border
        possibleMoves[0][m][1]= true;
        possibleMoves[0][m][2] = true;
        possibleMoves[0][m][3] = true;

        //set rigt border
        possibleMoves[8][m][5]= true;
        possibleMoves[8][m][6]= true;
        possibleMoves[8][m][7]= true;
    }

    //horizontal borders
    for (int n = 1; n <=2 ; ++n) {
        //left-up
        possibleMoves[n][1][3]= true;
        possibleMoves[n][1][4]= true;
        possibleMoves[n][1][5]= true;
        //right-up
        possibleMoves[n+5][1][3]= true;
        possibleMoves[n+5][1][4]= true;
        possibleMoves[n+5][1][5]= true;
        //left-down
        possibleMoves[n][11][0]= true;
        possibleMoves[n][11][1]= true;
        possibleMoves[n][11][7]= true;

        //right-down
        possibleMoves[n+5][11][0]= true;
        possibleMoves[n+5][11][1]= true;
        possibleMoves[n+5][11][7]= true;
    }

    //gates
    for (int i1 = 3; i1 <= 5; ++i1) {
        for (int i = 0; i < 8; ++i) {
            possibleMoves[i1][1][i]= true;
            possibleMoves[i1][11][i]= true;
        }

    }
    possibleMoves[3][1][7] = false;
    possibleMoves[5][1][1] = false;
    possibleMoves[3][11][5] = false;
    possibleMoves[3][11][3] = false;


}