//
// Created by dominik on 17.01.19.
//

#ifndef PROJEKT_GAME_H
#define PROJEKT_GAME_H


class Game {
private:
    /**
     * OZNACZENIA KIERUNKOW
     * 0 - GORA
     * 1 - UKOS GORA-PRAWO
     * 2 - PRAWO
     * 3 - UKOSC DOL-PRAWO
     * 4 - DOL
     * 5 - UKOS DOL-LEWO
     * 6 - LEWO
     * 7 - UKOS LEWO-GORA**/
    bool allowedMoves[9][13][8];
    int currentBallPositionX = 4;
    int currentBallPositionY = 6;
    int player1Id = -1;
    int player2Id = -1;
    bool gameStarted = false;

public:
    bool isPlayer1SeatOccupied();

    bool isPlayer2SeatOccupied();

    bool isGameInProgress();

    void takePlayerSeat(int seatNo, int playerIndex);

    void freePlayerSeat(int seatNo);

    int playerIdOnSelectedSeat(int seatNo);

    void preparePossibleMovesToGame();

    int moveBall(char direction);

    bool isBallStuck();

    bool isBallInGate(int gateNo);

    void startGame();

    void resetGame();

};


#endif //PROJEKT_GAME_H
