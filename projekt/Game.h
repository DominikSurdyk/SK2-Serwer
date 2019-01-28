//
// Created by dominik on 17.01.19.
//

#ifndef PROJEKT_GAME_H
#define PROJEKT_GAME_H


class Game {
private:
    /** jest to mapa pokazujaca wszystkie mozliwe polaczenia
     * 0 - x
     * 1 - y
     * 2 - kierunki w ktorych mozna wykonac ruch z danego pkt. 1(true) 0ZNACZA MOZLIWY RUCH, 0(false) OZNACZA BRAK RUCHU
     *
     * OZNACZENIA KIERUNKOW
     * 0 - GORA
     * 1 - UKOS GORA-PRAWO
     * 2 - PRAWO
     * 3 - UKOSC DOL-PRAWO
     * 4 - DOL
     * 5 - UKOS DOL-LEWO
     * 6 - LEWO
     * 7 - UKOS LEWO-GORA**/
    bool possibleMoves[9][13][8];
    int actualBallPositionX = 4;
    int actualBallPositionY = 6;
    int player1Id = -1;
    int player2Id = -1;

public:
    bool isPlayer1SeatOccupied();

    bool isPlayer2SeatOccupied();



    bool isGameInProgress();

    int takePlayer1Seat(int playerIndex);

    int takePlayer2Seat(int playerIndex);

    int takePlayerSeat(int seatNo, int playerIndex);

    void freePlayerSeat(int seatNo);

    int playerIdOnSelectedSeat(int seatNo);

    void preparePossibleMovesToGame();

    int moveBall(char direction);

    bool isPossibleReflectionOn();

    bool isBallStuck();

    bool isBallInGate(int gateNo);

    bool isPlayerWin(int playerNo);

    bool isPlayerLoose(int playerNo);

};


#endif //PROJEKT_GAME_H
