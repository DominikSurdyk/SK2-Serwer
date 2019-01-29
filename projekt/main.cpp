#include <cstdio>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <bits/locale_facets.tcc>
#include <string>
#include <cstring>
#include "Game.h"

using namespace std;


#define MAX_FDS 1000
#define GAMES_AMOUNT 3
struct pollfd fileDescriptorsArray[MAX_FDS];
int numberOfFileDescriptors = 1;
string clientsMessagesQueuesArray[MAX_FDS];
int clientsIdArray[MAX_FDS];
Game gamesArray[GAMES_AMOUNT];
const string MESSAGES_DELIMETER = "-";

int sendResponseToClient(int fileDescriptor, int responseCode, string message) {
    string responseCodeStr = to_string(responseCode);
    string responseStr = responseCodeStr + "-" + message + "\n";
    int responseLength = responseStr.length();
    char response[responseLength];
    strcpy(response, responseStr.c_str());
    printf("Wysyłam: %s",responseStr.c_str());
    int writeResult = (ssize_t) write(fileDescriptor, response, responseLength);
    return writeResult;
}

bool acceptNewConnection(int nSocket) {
    int newConnection = accept(nSocket, NULL, NULL);
    if (newConnection < 0) {
        perror("accept() failed");
        return false;
    }
    printf("Nowe polaczenie zaakceptowane...\n");
    fileDescriptorsArray[numberOfFileDescriptors].fd = newConnection;
    fileDescriptorsArray[numberOfFileDescriptors].events = POLLIN;
    fileDescriptorsArray[numberOfFileDescriptors].revents = 0;
    numberOfFileDescriptors++;
    return true;
}

string getGamesSeatsStatus() {
    string response = "";
    for (int i = 0; i < GAMES_AMOUNT; i++) {
        if (gamesArray[i].isPlayer1SeatOccupied()) {
            response += "1";
        } else {
            response += "0";
        }
        if (gamesArray[i].isPlayer2SeatOccupied()) {
            response += "1";
        } else {
            response += "0";
        }
    }
    return response;
}

void freeGameSeatByClientId(int clientId) {
    for (int i = 0; i < GAMES_AMOUNT; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (gamesArray[i].playerIdOnSelectedSeat(j) == clientId) {
                gamesArray[i].freePlayerSeat(j);
                printf("Usunieto gracza o ID: %d, ze stolu o nr: %d, i miejscu: %d\n", clientId, i, j);
            }
        }
    }
}

int findGameNoByClientId(int clientId) {
    int result = -1;
    for (int i = 0; i < GAMES_AMOUNT; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (gamesArray[i].playerIdOnSelectedSeat(j) == clientId) {
                result = i;
            }
        }
    }
    return result;
}

int findFileDescriptorIndexByClientId(int cliendId) {
    int result = -1;
    for (int i = 1; i < numberOfFileDescriptors; ++i) {
        if (clientsIdArray[i] == cliendId) {
            result = i;
        }
    }
    return result;
}

int main(int argc, char *argv[]) {
    int SERVER_PORT = 1234;
    int QUEUE_SIZE  = 15;
    int BUFFER_INCOMING_SIZE = 2;
    int POLL_TIMEOUT  = 1000;
    if (argc > 1){
        if (stoi(argv[1]) > 0)
            SERVER_PORT = stoi(argv[1]);
    }
    if (argc > 2){
        if (stoi(argv[2]) > 0)
        QUEUE_SIZE = stoi(argv[2]);
    }
    if (argc > 3){
        if (stoi(argv[3]) > 0)
        BUFFER_INCOMING_SIZE = stoi(argv[3]);
    }
    if (argc > 4){
        if (stoi(argv[4]) > 0)
        POLL_TIMEOUT = stoi(argv[4]);
    }

    printf("Program start\n");
    printf("Server port : %d\n", SERVER_PORT);
    printf("Rozmiar kolejki oczekujacych : %d\n", QUEUE_SIZE);
    printf("Rozmiar bufora wiadomosci : %d\n", BUFFER_INCOMING_SIZE);
    printf("Czas Poll : %d\n", POLL_TIMEOUT);
    int clientId = 1;
    bool compressArray = false;

    /*inicjalizacja kolejek wiadomości do serwera*/
    for (int k = 0; k < MAX_FDS; ++k) {
        clientsMessagesQueuesArray[k] = "";
    }
    //inicjalizacja gier
    for (int l = 0; l < GAMES_AMOUNT; ++l) {
        gamesArray[l].resetGame();
    }

    int nSocket;
    int nBind, nListen;
    int nFoo = 1;

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SERVER_PORT);


    /* Tworzenie socketu(gniazda) */
    nSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (nSocket < 0) {
        fprintf(stderr, "%s: Can't create a socket.\n", argv[0]);
        exit(1);
    }
    /*Czyszczenie gniazda ze śmieci */
    setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &nFoo, sizeof(nFoo));

    /* Wiązanie gniazda z adresem*/
    nBind = bind(nSocket, (struct sockaddr *) &addr, sizeof(struct sockaddr));
    if (nBind < 0) {
        fprintf(stderr, "%s: Can't bind a name to a socket.\n", argv[0]);
        exit(1);
    }
    /* Ustawianie rozmiaru kolejki do nasłuchiwania */
    nListen = listen(nSocket, QUEUE_SIZE);
    if (nListen < 0) {
        fprintf(stderr, "%s: Can't set queue size.\n", argv[0]);
    }

    fileDescriptorsArray[0].fd = nSocket;
    fileDescriptorsArray[0].events = POLLIN;
    /* Nawiązywanie nowych połączeń, oraz obsługa pozostałych*/
    while (true) {

        int rc;
        rc = poll(fileDescriptorsArray, numberOfFileDescriptors, POLL_TIMEOUT);
        if (rc < 0) {
            perror("poll() failed");
            break;
        }

        if (rc == 0) {
            //printf("poll() timed out. There are no neww connections.\n");
            continue;
        }

        /*akceptowanie nowych połączeń*/
        if (fileDescriptorsArray[0].revents & POLLIN) {
            acceptNewConnection(nSocket);
        }

        for (int i = 1; i < numberOfFileDescriptors; i++) {


            if (fileDescriptorsArray[i].revents & POLLIN) {
                printf("POLLIN! FileDescriptor: %d \n", i);
                bool endOfMessage = false;
                /*
                 * KODY ZAPYTAN (zakończone znakiem \n na końcu)
                 * 0 - koniec - przeciwnik się rozłączył
                 * 1 - podaj wolne miejsca i nadaj ID
                 * 2-ID-x - zajmnij wskazane miejsce x
                 * 3-G-S-xxxxx -wykonaj ruchy, G - nr gry, S - nr miejsca - xxx ruchy
                 * 4 - podaj same wolne miejsca
                 *
                 * KODY ODPOWIEDZI (zakończone znakiem \n na końcu)
                 * 1-ID-xxx -  nadano ID, xxx - 01110 itp zajętości miejsc
                 * 2-x - udało się zająć miejsce. x to 1 lub 0 ( 1 zaczynaj, 0 czekaj na ruch)
                 * 3-xxx - nie udalo sie zajac miejsca, xxx wolne miejsca
                 * 4-Status-xxx - przeciwnik wykonal ruchy. xxx-ruchy 0 - graj dalej, 1 - wygrales - 2 przegrales
                 * 5-xxx - przesyłam status miejsc
                 */

                char bufforIn[BUFFER_INCOMING_SIZE];
                int odp = 0;
                string message = "";
                odp = read(fileDescriptorsArray[i].fd, bufforIn, BUFFER_INCOMING_SIZE);
                for (int j = 0; j < odp; ++j) {
                    message += bufforIn[j];
                    if (bufforIn[j] == '\n') {
                        endOfMessage = true;
                        printf("Znak konca wiadomosci -> dodaje do kolejki");
                    }
                }
                printf("\nOdebrano : %d znakow\n", odp);
                write(1, bufforIn, odp);

                string fullMessage = "";
                if (!endOfMessage) {
                    printf("Nie odebrano calej wiadomosci\n");
                    clientsMessagesQueuesArray[i] += message;
                } else {
                    fullMessage = clientsMessagesQueuesArray[i] += message;
                    clientsMessagesQueuesArray[i] = "";
                    printf("Cala wiadomosc: %s", fullMessage.c_str());
                }

                if (endOfMessage) {
                    char messageCode = fullMessage[0];
                    if (messageCode == '0') {
                    } else if (messageCode == '1') {
                        printf("[JEDEN] -> Klient prosi o dostepnosc stolow, oraz nadanie Id!\n");
                        clientsIdArray[i] = clientId;
                        printf("Dla indeksu nr : %d w tablicy fileDescriptorow,  nadano Id: %d!\n", i, clientId);
                        string response = to_string(clientId) + "-" + getGamesSeatsStatus();
                        sendResponseToClient(fileDescriptorsArray[i].fd, 1, response);
                        clientId++;
                    } else if (messageCode == '2') {
                        printf("[DWA] -> Klient prosi o zajecie wskazanego miejsca\n");

                        //wymazanie kodu wiadomosci
                        int delimeter = fullMessage.find(MESSAGES_DELIMETER);
                        fullMessage.erase(0, delimeter + 1);

                        //wyciagniecie ID klienta
                        delimeter = fullMessage.find(MESSAGES_DELIMETER);
                        string clientId = fullMessage.substr(0, delimeter);
                        int clientIdInt = stoi(clientId);

                        //wyciagniecie miejsca ktore klient chce zajac
                        fullMessage.erase(0, delimeter + 1);
                        int endOffMesage = fullMessage.length() - 1;
                        int seat = stoi(fullMessage.substr(0, endOffMesage));

                        //sprawdzam czy miejsce jest zajete
                        int game = seat / 2;
                        int gameSeat = seat % 2;
                        bool isSeatOccupied;
                        bool isOppositeSeatOccupied;
                        if (gameSeat == 0) {
                            isSeatOccupied = gamesArray[game].isPlayer1SeatOccupied();
                            isOppositeSeatOccupied = gamesArray[game].isPlayer2SeatOccupied();
                        } else {
                            isSeatOccupied = gamesArray[game].isPlayer2SeatOccupied();
                            isOppositeSeatOccupied = gamesArray[game].isPlayer1SeatOccupied();
                        }

                        if (!isSeatOccupied) {
                            //miejsce wolne, zajmuje miejsce
                            gamesArray[game].takePlayerSeat(gameSeat, clientIdInt);
                            printf("Zajmuje miejsce nr %d w grze nr :%d. Id klienta: %d \n", gameSeat, game,
                                   clientIdInt);
                            string response;
                            if (isOppositeSeatOccupied) {
                                gamesArray[game].startGame();
                                response = "1";
                            } else {
                                response = "0";
                            }
                            sendResponseToClient(fileDescriptorsArray[i].fd, 2, response);

                        } else {
                            //miejse zajete, odsylam wiadomosc o zajetosci miejsca
                            printf("Niestety, to miejsce jest juz zajete. Odsylam nowa liste miejsc\n");
                            string response = getGamesSeatsStatus();
                            sendResponseToClient(fileDescriptorsArray[i].fd, 3, response);
                        }
                    } else if (messageCode == '3') {
                        printf("[TRZY] -> Przeslano ruchy\n");

                        //wyciagniecie kodu wiadomosci
                        int delimeter = fullMessage.find(MESSAGES_DELIMETER);
                        string opponentResponseStatus = fullMessage.substr(0, delimeter);
                        fullMessage.erase(0, delimeter + 1);
                        int opponentResponseStatusInt = stoi(opponentResponseStatus);

                        //wyciagniecie ID klienta
                        delimeter = fullMessage.find(MESSAGES_DELIMETER);
                        string clientId = fullMessage.substr(0, delimeter);

                        //wyciagniecie nr gry
                        fullMessage.erase(0, delimeter + 1);
                        delimeter = fullMessage.find(MESSAGES_DELIMETER);
                        string gameNo = fullMessage.substr(0, delimeter);
                        int gameNoInt = stoi(gameNo);

                        //wyciagniecie nr miejsca
                        fullMessage.erase(0, delimeter + 1);
                        delimeter = fullMessage.find(MESSAGES_DELIMETER);
                        string seatNo = fullMessage.substr(0, delimeter);
                        int seatNoInt = stoi(seatNo);

                        //ostatnie
                        fullMessage.erase(0, delimeter + 1);
                        string moves = fullMessage.substr(0, fullMessage.length() - 1);
                        printf("PRZETWORZONE\n");
                        printf("Status: %d\n", opponentResponseStatusInt);
                        printf("Id: %s\n", clientId.c_str());
                        printf("Nr gry: %s\n", gameNo.c_str());
                        printf("Nr miejsca: %s\n", seatNo.c_str());
                        printf("Ruchy: %s\n", moves.c_str());
                        printf("Ruchy dlugosc: %d\n", (int) moves.length());

                        bool resetGame = false;
                        //wrzucenie wyniku na serwer
                        for (unsigned int j = 0; j < moves.length(); ++j) {
                            gamesArray[gameNoInt].moveBall(moves[j]);
                        }
                        //sprawdzam stan gry
                        string responseToNextPlayer;
                        if (gamesArray[gameNoInt].isBallInGate(seatNoInt)) {
                            printf("Pilka w bramce nadsylajacego. Wygrana nastepnego gracza!\n");
                            responseToNextPlayer = "1-";
                            resetGame = true;
                        }else if (gamesArray[gameNoInt].isBallInGate((seatNoInt + 1) % 1)) {
                            printf("Pilka w bramce przeciwnika nadsylajacego. Przegrana nastepnego gracza!!\n");
                            responseToNextPlayer = "2-";
                            resetGame = true;
                        }else if (gamesArray[gameNoInt].isBallStuck()) {
                            printf("Brak ruchow. Wygrana nastepnego gracza!\n");
                            responseToNextPlayer = "1-";
                            resetGame = true;
                        } else {
                            printf("Gra toczy sie dalej\n");
                            responseToNextPlayer = "0-";
                        }
                        //wysyłam odpowiedź do przeciwnika
                        responseToNextPlayer += moves;
                        int oppositeClientId = gamesArray[gameNoInt].playerIdOnSelectedSeat((seatNoInt + 1) % 2);
                        int oppositeClientFileDescriptor = findFileDescriptorIndexByClientId(oppositeClientId);
                        printf("Wysylam odpowiedz do klienta o ID: %d\n", oppositeClientId);
                        if (oppositeClientFileDescriptor != -1) {
                            sendResponseToClient(fileDescriptorsArray[oppositeClientFileDescriptor].fd, 4,
                                                 responseToNextPlayer);
                        } else {
                            printf("Blad w znajdowaniu odpowiedniego fileDescriptora");
                        }

                        if (resetGame) {
                            gamesArray[gameNoInt].resetGame();
                            printf("Gra zakonczone. Resetuje stan gry!\n");
                        }
                    } else if (messageCode == '4') {
                        printf("[CZTERY] -> Klient skonczyl grę, i prosi o podanie wolnych miejsc");
                        string response = getGamesSeatsStatus();
                        sendResponseToClient(fileDescriptorsArray[i].fd, 5, response);
                    }
                }

                if (odp == 0) {
                    printf("Socket nr:%d jest zamkniety\n", i);
                    fileDescriptorsArray[i].fd = -1;
                    compressArray = true;
                    close(fileDescriptorsArray[i].fd);

                    /*jeśli sa dwaj gracze to znaczy ze gra jest juz rozpoczeta.
                     * Gracz ktory sie rozlacza przegrywa, a komunikat zostaje wyslany do drugiego gracza.
                     * Gra nastepnie jest resetowana
                     */
                    int gameNo = findGameNoByClientId(clientsIdArray[i]);

                    if (gamesArray[gameNo].isGameInProgress() &&
                        !gamesArray[gameNo].isBallInGate(0) &&
                        !gamesArray[gameNo].isBallInGate(1) &&
                        !gamesArray[gameNo].isBallStuck()) {

                        int playerId = gamesArray[gameNo].playerIdOnSelectedSeat(0);
                        if (clientsIdArray[i] == playerId) {
                            playerId = gamesArray[gameNo].playerIdOnSelectedSeat(1);
                        }
                        int playerFileDecriptor = findFileDescriptorIndexByClientId(playerId);
                        printf("Zamykam gre nr: %d. Gracz o ID: %d, sie rozlaczyl\n", gameNo, playerId % 2);
                        sendResponseToClient(fileDescriptorsArray[playerFileDecriptor].fd, 0, "koniec");
                        gamesArray[gameNo].resetGame();
                    }

                    //uwolnij miejsce
                    freeGameSeatByClientId(clientsIdArray[i]);
                } else if (odp == -1) {
                    printf("Error when reading input data from client\n");
                }
            }
        }

        if (compressArray) {
            compressArray = false;
            for (int i = 0; i <= numberOfFileDescriptors; i++) {
                if (fileDescriptorsArray[i].fd == -1) {
                    printf("Usuwam z tablicy fileDescriptor nr %d, wszystkie nastepne indeksy maja zmniejszone indeksy!\n",
                           i);
                    for (int j = i; j < numberOfFileDescriptors; j++) {
                        clientsIdArray[j] = clientsIdArray[j + 1];
                        clientsMessagesQueuesArray[j] = clientsMessagesQueuesArray[j + 1];
                        fileDescriptorsArray[j].fd = fileDescriptorsArray[j + 1].fd;
                    }
                    i--;
                    numberOfFileDescriptors--;
                }
            }
        }
    }

}
