//
// Created by dominik on 17.01.19.
//

#include <cstdio>
#include "Game.h"

bool Game::isPlayer1SeatOccupied() {
    return player1Id > 0;
}

bool Game::isPlayer2SeatOccupied() {
    return player2Id > 0;
}

bool Game::isGameInProgress() {
    return gameStarted;
}

void Game::takePlayerSeat(int seatNo, int playerId) {
    if (seatNo == 0) {
        if (player1Id <= 0) {
            player1Id = playerId;
        }
    } else {
        if (player2Id <= 0) {
            player2Id = playerId;
        }
    }
}

void Game::freePlayerSeat(int seatNo) {
    if (seatNo == 0) {
        player1Id = -1;
    } else {
        player2Id = -1;
    }
}

int Game::playerIdOnSelectedSeat(int seatNo) {
    if (seatNo == 0) {
        return player1Id;
    } else {
        return player2Id;
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
    for (int l = 1; l <= 7; ++l) {
        for (int i = 2; i <= 10; ++i) {
            for (int j = 0; j < 8; ++j) {
                possibleMoves[l][i][j] = true;
            }
        }
    }

    //vertical borders
    for (int m = 2; m <= 10; ++m) {
        //set left border
        possibleMoves[0][m][1] = true;
        possibleMoves[0][m][2] = true;
        possibleMoves[0][m][3] = true;

        //set rigt border
        possibleMoves[8][m][5] = true;
        possibleMoves[8][m][6] = true;
        possibleMoves[8][m][7] = true;
    }

    //horizontal borders
    for (int n = 1; n <= 2; ++n) {
        //left-up
        possibleMoves[n][1][3] = true;
        possibleMoves[n][1][4] = true;
        possibleMoves[n][1][5] = true;
        //right-up
        possibleMoves[n + 5][1][3] = true;
        possibleMoves[n + 5][1][4] = true;
        possibleMoves[n + 5][1][5] = true;
        //left-down
        possibleMoves[n][11][0] = true;
        possibleMoves[n][11][1] = true;
        possibleMoves[n][11][7] = true;

        //right-down
        possibleMoves[n + 5][11][0] = true;
        possibleMoves[n + 5][11][1] = true;
        possibleMoves[n + 5][11][7] = true;
    }

    //gates
    for (int i1 = 3; i1 <= 5; ++i1) {
        for (int i = 0; i < 8; ++i) {
            possibleMoves[i1][1][i] = true;
            possibleMoves[i1][11][i] = true;
        }

    }
    //gates-line-up-left
    possibleMoves[3][1][0] = false;
    possibleMoves[3][1][7] = false;
    possibleMoves[3][1][6] = false;

    //gates-line-up-right
    possibleMoves[5][1][0] = false;
    possibleMoves[5][1][1] = false;
    possibleMoves[5][1][2] = false;

    //gates-line-down-left
    possibleMoves[3][11][4] = false;
    possibleMoves[3][11][5] = false;
    possibleMoves[3][11][6] = false;

    //gates-line-down-right
    possibleMoves[5][11][2] = false;
    possibleMoves[5][11][3] = false;
    possibleMoves[5][11][4] = false;

    //corners
    possibleMoves[0][1][3] = true;
    possibleMoves[8][1][5] = true;
    possibleMoves[0][11][1] = true;
    possibleMoves[8][11][7] = true;

}

int Game::moveBall(char directionType) {
    unsigned char direction = (unsigned char) directionType;
    printf("ruch pilki: %c\n", directionType);
    //gora
    if (direction == '0') {
        possibleMoves[currentBallPositionX][currentBallPositionY][direction] = false;
        possibleMoves[currentBallPositionX][currentBallPositionY - 1][(direction + 4) % 8] = false;
        currentBallPositionY--;
        return 1;
    }
    //gora-prawo
    if (direction == '1') {
        possibleMoves[currentBallPositionX][currentBallPositionY][direction] = false;
        possibleMoves[currentBallPositionX + 1][currentBallPositionY - 1][(direction + 4) % 8] = false;
        currentBallPositionY--;
        currentBallPositionX++;
        return 1;
    }
    //prawo
    if (direction == '2') {
        possibleMoves[currentBallPositionX][currentBallPositionY][direction] = false;
        possibleMoves[currentBallPositionX + 1][currentBallPositionY][(direction + 4) % 8] = false;
        currentBallPositionX++;
        return 1;
    }
    //dol-prawo
    if (direction == '3') {
        possibleMoves[currentBallPositionX][currentBallPositionY][direction] = false;
        possibleMoves[currentBallPositionX + 1][currentBallPositionY + 1][(direction + 4) % 8] = false;
        currentBallPositionX++;
        currentBallPositionY++;
        return 1;
    }
    //dol
    if (direction == '4') {
        possibleMoves[currentBallPositionX][currentBallPositionY][direction] = false;
        possibleMoves[currentBallPositionX][currentBallPositionY + 1][(direction + 4) % 8] = false;
        currentBallPositionY++;
        return 1;
    }
    //dol-lewo
    if (direction == '5') {
        possibleMoves[currentBallPositionX][currentBallPositionY][direction] = false;
        possibleMoves[currentBallPositionX - 1][currentBallPositionY + 1][(direction + 4) % 8] = false;
        currentBallPositionY++;
        currentBallPositionX--;
        return 1;
    }
    //lewo
    if (direction == '6') {
        possibleMoves[currentBallPositionX][currentBallPositionY][direction] = false;
        possibleMoves[currentBallPositionX - 1][currentBallPositionY][(direction + 4) % 8] = false;
        currentBallPositionX--;
        return 1;
    }
    //lewo-gora
    if (direction == '7') {
        possibleMoves[currentBallPositionX][currentBallPositionY][direction] = false;
        possibleMoves[currentBallPositionX - 1][currentBallPositionY - 1][(direction + 4) % 8] = false;
        currentBallPositionX--;
        currentBallPositionY--;
        return 1;
    }
    return -1;

}

bool Game::isBallStuck() {
    int count = 0;
    for (int i = 0; i < 8; i++) {
        if (!possibleMoves[currentBallPositionX][currentBallPositionY][i]) {
            count++;
        }
    }
    return count == 8;
}


bool Game::isBallInGate(int gateNo) {
    if (gateNo == 0) {
        if ((currentBallPositionX == 3 || currentBallPositionX == 4 || currentBallPositionX == 5) &&
            currentBallPositionY == 0) {
            return true;
        } else {
            return false;
        }
    } else if ((currentBallPositionX == 3 || currentBallPositionX == 4 || currentBallPositionX == 5) &&
               currentBallPositionY == 12) {
        return true;
    } else {
        return false;
    }
}

void Game::startGame() {
    gameStarted = true;
}

void Game::resetGame() {
    gameStarted = false;
    player1Id = -1;
    player2Id = -1;
    preparePossibleMovesToGame();
    currentBallPositionX = 4;
    currentBallPositionY = 6;
}