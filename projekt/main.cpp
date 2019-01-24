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


#define SERVER_PORT 1234
#define QUEUE_SIZE 15
#define MAX_FDS 1000
#define GAMES_AMOUNT 3
#define POLL_TIMEOUT 1500
#define BUFFER_INCOMING_SIZE 20

struct pollfd fileDescriptorsArray[MAX_FDS];
int numberOfFileDescriptors = 1;
string messagesQueue[MAX_FDS];
Game gamesArray[GAMES_AMOUNT];
const string MESSAGES_DELIMETER = "-";

int sendResponseToClient(int fileDescriptor, int responseCode, string message) {
    string responseCodeStr = to_string(responseCode);
    string responseStr = responseCodeStr + "-" + message + "\n";
    int responseLength = responseStr.length();
    char response[responseLength];
    strcpy(response, responseStr.c_str());
    int writeResult = write(fileDescriptor, response, responseLength);
}

bool acceptNewConnection(int nSocket) {
    int newConnection = accept(nSocket, NULL, NULL);
    if (newConnection < 0) {
        perror("accept() failed");
        return false;
    }

    printf("New connection accepted...\n");

    fileDescriptorsArray[numberOfFileDescriptors].fd = newConnection;
    fileDescriptorsArray[numberOfFileDescriptors].events = POLLIN;
    fileDescriptorsArray[numberOfFileDescriptors].revents = 0;
    numberOfFileDescriptors++;
}

string getGamesSeatsStatus() {
    string response = "";
    for (int i = 0; i < GAMES_AMOUNT; i++) {
        if (gamesArray->isPlayer1SeatOccupied()) {
            response += "1";
        } else {
            response += "0";
        }
        if (gamesArray->isPlayer2SeatOccupied()) {
            response += "1";
        } else {
            response += "0";
        }
    }
    return response;
}

int main(int argc, char *argv[]) {
    printf("Program start\n");
    /*inicjalizacja kolejek wiadomości do serwera*/
    for (int k = 0; k < MAX_FDS; ++k) {
        messagesQueue[k] = "";
    }
    //nadawanie połaczeniom id
    int clientId = 1;
    bool compressArray = false;

    int on = 1;

    int current_size = 0, listen_sd = -1;

    int new_sd = -1;

    int nSocket, nClientSocket;
    int nBind, nListen;
    int nFoo = 1;
    socklen_t nTmp;
    struct sockaddr_in stAddr, stClientAddr;


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
        printf("+");
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
            bool closeConnection = false;

            /*Sprawdzanie czy wystąpiły błędy */
            if (fileDescriptorsArray[i].revents & POLLERR) {
                printf("socket error, closing connection...\n");
                closeConnection = true;

            }
            if (fileDescriptorsArray[i].revents & POLLIN) {
                printf("POLLIN: %d", i);
                bool endOfMessage = false;
                /*
                 * KODY ZAPYTAN (zakończone znakiem \n na końcu)
                 * 0 - ponow poprzendia wiadomosc
                 * 1 - podaj wolne miejsca
                 * 2-ID-x - zajmnij wskazane miejsce x
                 * 3k - rusz się w podanym kierunku
                 * 4 - zapytaj o ruch przeciwnika
                 * 5 - opusc stolik
                 *
                 *
                 * KODY ODPOWIEDZI (zakończone znakiem \n na końcu
                 * 1 - wolne miejsca a następnie ciąg 01110 itp zajętości miejsc
                 * 2-x - udało się zająć miejsce. x to 1 lub 0 ( 1 zaczynaj, 0 czekaj na ruch)
                 * 3-xxx - nie udalo sie zajac miejsca, x wolne miejsca
                 * 4 - udało się ruszyć we wskazanym kierunku
                 * 5k - przeciwnik ruszył się w kierunku k. Nastepnie:0 - twoj ruch, 1 wygrales, 2 -przegrales
                 * 6 - udalo sie opuscic stolik
                 */


                char bufforIn[BUFFER_INCOMING_SIZE];
                int odp = 0;
                int odpAll = 0;
                string message = "";
                odp = read(fileDescriptorsArray[i].fd, bufforIn, BUFFER_INCOMING_SIZE);
                //write(1, bufforIn, odp);
                printf("---------\n");
                for (int j = 0; j < odp; ++j) {
                    message += bufforIn[j];
                    if (bufforIn[j] == '\n') {
                        endOfMessage = true;
                        printf("Znak końca wiadomości! -> dodaję do kolejki\n");
                    }
                }
                odpAll += odp;
                //printf("Końcowa wiadomość sklejona: %s\n", message.c_str());
                printf("\nOdebrano : %d znakow\n", odpAll);
                write(1, bufforIn, odp);

                string fullMessage = "";
                if (!endOfMessage) {
                    printf("Nie odebrano całej wiadomości\n");
                    messagesQueue[i] += message;
                } else {
                    fullMessage = messagesQueue[i] += message;
                    messagesQueue[i] = "";
                    printf("Cała wiadomość: %s", fullMessage.c_str());
                }

                if (endOfMessage) {
                    char messageCode = fullMessage[0];
                    if (messageCode == '0') {
                    } else if (messageCode == '1') {
                        printf("jeden -> Klient prosi o dostepnosc stolow, oraz nadanie ID!\n");
                        string response = to_string(clientId) + "-" + getGamesSeatsStatus();
                        sendResponseToClient(fileDescriptorsArray[i].fd, 1, response);
                        clientId++;
                    } else if (messageCode == '2') {
                        printf("dwa -> Klient prosi o zajecie wskazanego miejsca\n");

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
                            string response;
                            if (isOppositeSeatOccupied) {
                                response = "1";
                            } else {
                                response = "0";
                            }
                            sendResponseToClient(fileDescriptorsArray[i].fd, 2, response);

                        } else {
                            //miejse zajete, odsylam wiadomosc o zajetosci miejsca
                            string response = getGamesSeatsStatus();
                            sendResponseToClient(fileDescriptorsArray[i].fd, 3, response);
                        }
                    } else if (messageCode == '3') {
                        printf("trzy");
                    } else if (messageCode == '4') {
                        printf("cztery");
                    } else if (messageCode == '5') {
                        printf("pięć1");
                    }
                }


                if (odp == 0) {
                    printf("Socket nr:%d is closed\n", i);
                    fileDescriptorsArray[i].fd = -1;
                    compressArray = true;
                    close(fileDescriptorsArray[i].fd);
                    //gracz przegrał
                } else if (odp == -1) {
                    printf("Error when reading input data from client\n");
                    //odpowiedz wiadomością z początkiem 1 o ponowne przeslanie
                }

            }


        }

        if (compressArray) {
            compressArray = false;
            for (int i = 0; i <= numberOfFileDescriptors; i++) {
                if (fileDescriptorsArray[i].fd == -1) {
                    printf("Usuwam z tablicy fileDescriptor nr %d, wszystkie nastepne indeksy maja zmniejszone indeksy",
                           i);
                    for (int j = i; j < numberOfFileDescriptors; j++) {
                        messagesQueue[j] = messagesQueue[j + 1];
                        fileDescriptorsArray[j].fd = fileDescriptorsArray[j + 1].fd;
                    }
                    i--;
                    numberOfFileDescriptors--;
                }
            }
        }
    }

}