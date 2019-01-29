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
                allowedMoves[i][j][k] = false;
            }
        }
    }

    //set middle part of field
    for (int l = 1; l <= 7; ++l) {
        for (int i = 2; i <= 10; ++i) {
            for (int j = 0; j < 8; ++j) {
                allowedMoves[l][i][j] = true;
            }
        }
    }

    //vertical borders
    for (int m = 2; m <= 10; ++m) {
        //set left border
        allowedMoves[0][m][1] = true;
        allowedMoves[0][m][2] = true;
        allowedMoves[0][m][3] = true;

        //set rigt border
        allowedMoves[8][m][5] = true;
        allowedMoves[8][m][6] = true;
        allowedMoves[8][m][7] = true;
    }

    //horizontal borders
    for (int n = 1; n <= 2; ++n) {
        //left-up
        allowedMoves[n][1][3] = true;
        allowedMoves[n][1][4] = true;
        allowedMoves[n][1][5] = true;
        //right-up
        allowedMoves[n + 5][1][3] = true;
        allowedMoves[n + 5][1][4] = true;
        allowedMoves[n + 5][1][5] = true;
        //left-down
        allowedMoves[n][11][0] = true;
        allowedMoves[n][11][1] = true;
        allowedMoves[n][11][7] = true;

        //right-down
        allowedMoves[n + 5][11][0] = true;
        allowedMoves[n + 5][11][1] = true;
        allowedMoves[n + 5][11][7] = true;
    }

    //gates
    for (int i1 = 3; i1 <= 5; ++i1) {
        for (int i = 0; i < 8; ++i) {
            allowedMoves[i1][1][i] = true;
            allowedMoves[i1][11][i] = true;
        }

    }
    //gates-line-up-left
    allowedMoves[3][1][0] = false;
    allowedMoves[3][1][7] = false;
    allowedMoves[3][1][6] = false;

    //gates-line-up-right
    allowedMoves[5][1][0] = false;
    allowedMoves[5][1][1] = false;
    allowedMoves[5][1][2] = false;

    //gates-line-down-left
    allowedMoves[3][11][4] = false;
    allowedMoves[3][11][5] = false;
    allowedMoves[3][11][6] = false;

    //gates-line-down-right
    allowedMoves[5][11][2] = false;
    allowedMoves[5][11][3] = false;
    allowedMoves[5][11][4] = false;

    //corners
    allowedMoves[0][1][3] = true;
    allowedMoves[8][1][5] = true;
    allowedMoves[0][11][1] = true;
    allowedMoves[8][11][7] = true;

}

int Game::moveBall(char directionType) {
    unsigned char direction = (unsigned char) directionType;
    printf("ruch pilki: %c\n", directionType);
    //gora
    if (direction == '0') {
        allowedMoves[currentBallPositionX][currentBallPositionY][direction] = false;
        allowedMoves[currentBallPositionX][currentBallPositionY - 1][(direction + 4) % 8] = false;
        currentBallPositionY--;
        return 1;
    }
    //gora-prawo
    if (direction == '1') {
        allowedMoves[currentBallPositionX][currentBallPositionY][direction] = false;
        allowedMoves[currentBallPositionX + 1][currentBallPositionY - 1][(direction + 4) % 8] = false;
        currentBallPositionY--;
        currentBallPositionX++;
        return 1;
    }
    //prawo
    if (direction == '2') {
        allowedMoves[currentBallPositionX][currentBallPositionY][direction] = false;
        allowedMoves[currentBallPositionX + 1][currentBallPositionY][(direction + 4) % 8] = false;
        currentBallPositionX++;
        return 1;
    }
    //dol-prawo
    if (direction == '3') {
        allowedMoves[currentBallPositionX][currentBallPositionY][direction] = false;
        allowedMoves[currentBallPositionX + 1][currentBallPositionY + 1][(direction + 4) % 8] = false;
        currentBallPositionX++;
        currentBallPositionY++;
        return 1;
    }
    //dol
    if (direction == '4') {
        allowedMoves[currentBallPositionX][currentBallPositionY][direction] = false;
        allowedMoves[currentBallPositionX][currentBallPositionY + 1][(direction + 4) % 8] = false;
        currentBallPositionY++;
        return 1;
    }
    //dol-lewo
    if (direction == '5') {
        allowedMoves[currentBallPositionX][currentBallPositionY][direction] = false;
        allowedMoves[currentBallPositionX - 1][currentBallPositionY + 1][(direction + 4) % 8] = false;
        currentBallPositionY++;
        currentBallPositionX--;
        return 1;
    }
    //lewo
    if (direction == '6') {
        allowedMoves[currentBallPositionX][currentBallPositionY][direction] = false;
        allowedMoves[currentBallPositionX - 1][currentBallPositionY][(direction + 4) % 8] = false;
        currentBallPositionX--;
        return 1;
    }
    //lewo-gora
    if (direction == '7') {
        allowedMoves[currentBallPositionX][currentBallPositionY][direction] = false;
        allowedMoves[currentBallPositionX - 1][currentBallPositionY - 1][(direction + 4) % 8] = false;
        currentBallPositionX--;
        currentBallPositionY--;
        return 1;
    }
    return -1;

}

bool Game::isBallStuck() {
    int count = 0;
    for (int i = 0; i < 8; i++) {
        if (!allowedMoves[currentBallPositionX][currentBallPositionY][i]) {
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