#include <cstdio>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

#include <stdlib.h>
#include <time.h>

#define SERVER_PORT 1234
#define QUEUE_SIZE 15
#define MAX_FDS 10
#define GAMES_AMOUNT 5
#define POLL_TIMEOUT 100
#define BUFFER_INCOMING_SIZE 20
#define BUFFER_OUTCOMING_SIZE 20

struct pollfd fileDescriptorsArray[MAX_FDS];
int numberOfFileDescriptors = 1;



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

int main(int argc, char *argv[]) {
    printf("Program start\n");


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
        //printf("new loop\n");
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

            } else if (fileDescriptorsArray[i].revents & POLLIN) {
                /*
                 * KODY ZAPYTAN (zakończone znakiem \n na końcu)
                 * 0 - ponow poprzendia wiadomosc
                 * 1 - podaj wolne miejsca
                 * 2xy - zajmnij wskazane miejsce (x - gra, y - miejsce)
                 * 3k - rusz się w podanym kierunku
                 * 4 - zapytaj o ruch przeciwnika
                 * 5 - opusc stolik
                 *
                 *
                 * KODY ODPOWIEDZI (zakończone znakiem \n na końcu)
                 *  poczatek 0 - udało się zrealizować
                 *  początek 1 - błąd
                 * 1 - wolne miejsca a następnie ciąg 01110 itp zajętości miejsc
                 * 2 - udało się zająć miejsce
                 * 3 - udało się ruszyć we wskazanym kierunku
                 * 4k - przeciwnik ruszył się w kierunku k. Nastepnie:0 - twoj ruch, 1 wygrales, 2 -przegrales
                 * 5 - udalo sie opuscic stolik
                 */

                //printf("POLLIN from %d \n",i);

                char bufforIn[BUFFER_INCOMING_SIZE];
                ssize_t odp = 0;
                odp = read(fileDescriptorsArray[i].fd, bufforIn, BUFFER_INCOMING_SIZE );
                if(odp == 0){
                    //printf("Socket is closed\n");
                    //gracz przegrał
                }else if(odp == -1){
                    printf("Error when reading input data from client\n");
                    //odpowiedz wiadomością z początkiem 1 o ponowne przeslanie
                }else{
                    //sprawdz czy na koncu jesz znak nowej lini
                    char messageCode = bufforIn[0];
                    if(bufforIn[odp-1] != '\n'){
                        printf("brak znaku nowej lini na koncu komunikatu\n");
                    }
                    if (messageCode == 0){
                        printf("zero");
                    }else if (messageCode == 1){
                        printf("jeden");
                    }else if (messageCode == 2){
                        printf("dwa");
                    }else if (messageCode == 3){
                        printf("trzy");
                    }else if (messageCode == 4){
                        printf("cztery");
                    }else if (messageCode == 5){
                        printf("pięć1");
                    }
                }



            }


        }
    }
}



/*
                char bufforIn[BUFFER_INCOMING_SIZE];
                int odp = 0;

                rc = read(fileDescriptorsArray[i].fd, bufforIn, BUFFER_INCOMING_SIZE );
                write(1, bufforIn, rc);
                printf("\nprzyjeto dane!\n");
                sleep(2);
                char bufforOut[BUFFER_OUTCOMING_SIZE] = {'o','d','p','\n'};
                rc = write(fileDescriptorsArray[i].fd, bufforOut, BUFFER_OUTCOMING_SIZE );
                printf("\nodeslano dane!\n");



                printf("koniec petli\n");
 */