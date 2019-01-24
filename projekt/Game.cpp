//
// Created by dominik on 17.01.19.
//

#include "Game.h"

bool Game::isPlayer1SeatOccupied() {
    return player1Index > 0;
}

bool Game::isPlayer2SeatOccupied() {
    return player2Index > 0;
}

bool Game::isGameInProgress() {
    return player1Index < 0 && player2Index < 0;
}

int Game::takePlayer1Seat(int playerIndex) {
    if (playerIndex <= 0) {
        player1Index = playerIndex;
        return 0;
    } else {
        return -1;
    }
}

int Game::takePlayer2Seat(int playerIndex) {
    if (playerIndex <= 0) {
        player2Index = playerIndex;
        return 0;
    } else {
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
    possibleMoves[3][1][7] = false;
    possibleMoves[5][1][1] = false;
    possibleMoves[3][11][5] = false;
    possibleMoves[3][11][3] = false;

    //corners
    possibleMoves[0][1][3] = true;
    possibleMoves[8][1][5] = false;
    possibleMoves[0][11][1] = false;
    possibleMoves[8][11][7] = false;

}

int Game::moveBall(int direction) {
    if (direction < 0 || direction > 7) {
        return -1;
    } else {
        //gora
        if (direction == 0) {
            possibleMoves[actualBallPositionX][actualBallPositionY][direction] = false;
            possibleMoves[actualBallPositionX][actualBallPositionY - 1][(direction + 4) % 8] = false;
            actualBallPositionY--;
        }
        //gora-prawo
        if (direction == 1) {
            possibleMoves[actualBallPositionX][actualBallPositionY][direction] = false;
            possibleMoves[actualBallPositionX + 1][actualBallPositionY - 1][(direction + 4) % 8] = false;
            actualBallPositionY--;
            actualBallPositionX++;
        }
        //prawo
        if (direction == 2) {
            possibleMoves[actualBallPositionX][actualBallPositionY][direction] = false;
            possibleMoves[actualBallPositionX + 1][actualBallPositionY][(direction + 4) % 8] = false;
            actualBallPositionX++;
        }
        //dol-prawo
        if (direction == 3) {
            possibleMoves[actualBallPositionX][actualBallPositionY][direction] = false;
            possibleMoves[actualBallPositionX + 1][actualBallPositionY + 1][(direction + 4) % 8] = false;
            actualBallPositionX++;
            actualBallPositionY++;
        }
        //dol
        if (direction == 4) {
            possibleMoves[actualBallPositionX][actualBallPositionY][direction] = false;
            possibleMoves[actualBallPositionX][actualBallPositionY + 1][(direction + 4) % 8] = false;
            actualBallPositionY++;
        }
        //dol-lewo
        if (direction == 5) {
            possibleMoves[actualBallPositionX][actualBallPositionY][direction] = false;
            possibleMoves[actualBallPositionX - 1][actualBallPositionY + 1][(direction + 4) % 8] = false;
            actualBallPositionY++;
            actualBallPositionX--;
        }
        //lewo
        if (direction == 6) {
            possibleMoves[actualBallPositionX][actualBallPositionY][direction] = false;
            possibleMoves[actualBallPositionX - 1][actualBallPositionY][(direction + 4) % 8] = false;
            actualBallPositionX--;
        }
        //lewo-gora
        if (direction == 7) {
            possibleMoves[actualBallPositionX][actualBallPositionY][direction] = false;
            possibleMoves[actualBallPositionX - 1][actualBallPositionY - 1][(direction + 4) % 8] = false;
            actualBallPositionX--;
            actualBallPositionY--;
        }
        return 1;
    }
}

bool Game::isPossibleReflectionOn() {
    bool reflection = false;
    for (int i = 0; i < 8; ++i) {
        if (!possibleMoves[actualBallPositionX][actualBallPositionY][i]) {
            reflection = true;
        }
    }
    return reflection;
}

int Game::resultOfMoves(int playerNo) {
    if (playerNo == 1) {
        if ((actualBallPositionX == 3 || actualBallPositionX == 4 || actualBallPositionX == 5) &&
            actualBallPositionY == 0) {
            return 1;
        } else {
            if (isPossibleReflectionOn()) {
                return 0;
            } else {
                return -1;
            }
        }
    } else {
        if ((actualBallPositionX == 3 || actualBallPositionX == 4 || actualBallPositionX == 5) &&
            actualBallPositionY == 12) {
            return 1;
        } else {
            if (isPossibleReflectionOn()) {
                return 0;
            } else {
                return -1;
            }
        }
    }
}