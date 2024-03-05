/**
 * @file clientcxnmanager.c
 * 
 * @brief Client
 * 
 * @author Tom REINA-HIOT
 * @author Téo LANGUEBIEN
 * @author Valentin BRONSARD
 * @author Adrien BARRAT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "clientcxnmanager.h"
#include "../../Common/ansiColors.h"
#include "../../Common/structures.h"
#include "../../Common/common.h"
#include "../../Common/common.c"
/* 0, c'est pas son tour, et 1 c'est son tour */
int aQuiLeTour = 0;

/**
 * @fn void * threadProcess
 * @brief Thread du Client
 */
void *threadProcess(void * ptr) {
    char buffer_in[BUFFERSIZE];
    int sockfd = *((int *) ptr);
    int len;

    /* Envoi de la Structure Hello {Type 1} */
    struct Hello helloFromClient = {1, "Hello du client !"};
    char dataOfHelloFromClient[sizeof(struct Hello)];
    memcpy(dataOfHelloFromClient, &helloFromClient, sizeof(struct Hello));
    if (write(sockfd, dataOfHelloFromClient, sizeof(struct Hello)) != -1) {
        /* DEBUG *///printf(YELLOW "[ POST ] Message sortant du serveur de type %d !\n" RESET, helloFromClient.type);
    }

    while ((len = read(sockfd, buffer_in, BUFFERSIZE)) != 0) {

        /* Le router récupère le type du message et redirige vers le type de message correspondant */
        struct Router router;
        memcpy(&router, buffer_in, sizeof(struct Router));
        /* DEBUG *///printf(YELLOW "[ GET ] Message entrant du serveur de type %d !\n" RESET, router.type);

        switch (router.type) {
            case 1:
                /* Récéption Hello {Type 1} */
                struct Hello helloFromClient;
                memcpy(&helloFromClient, buffer_in, sizeof(struct Hello));
                printf(GREEN "[ HELLO ] %s\n" RESET, helloFromClient.data);
                break;
            case 3:
                /* Récéption Wait {Type 3} */
                struct Wait wait;
                memcpy(&wait, buffer_in, sizeof(struct Wait));
                printf(GREEN "[ WAIT ] %s\n" RESET, wait.data);
                break;
            case 4:
                /* Récéption Game {Type 4} */
                struct Game jeu;
                memcpy(&jeu, buffer_in, sizeof(struct Game));
                TableauJeu * tableauJeu = (TableauJeu *)malloc(sizeof(TableauJeu));
                memcpy(tableauJeu, &(jeu.tableau), sizeof(TableauJeu));
                aQuiLeTour = jeu.tour;
                afficheTableauDesCoins(tableauJeu);
                printf(GREEN "[ GAME ] %s\n" RESET, jeu.data);

                break;
            case 6:
                /* Récéption Victory {Type 6} */
                struct Victory victoryFromClient;
                memcpy(&victoryFromClient, buffer_in, sizeof(struct Victory));
                printf(GREEN "[ VICTORY ] %s\n" RESET, victoryFromClient.data);
                printf(YELLOW "[ INFO ] Pour rejouer veuillez relancer le serveur et vous reconnecter.\n" RESET);
                close(sockfd);
                printf(RED "[ BYE ] Connexion terminée !\n" RESET);
                pthread_exit(0);    
                
            case 7:
                /* Récéption Defeat {Type 7} */
                struct Defeat defeatFromClient;
                memcpy(&defeatFromClient, buffer_in, sizeof(struct Defeat));
                printf(RED "[ DEFEAT ] %s\n" RESET, defeatFromClient.data);
                printf(YELLOW "[ INFO ] Pour rejouer veuillez relancer le serveur et vous reconnecter.\n" RESET);
                close(sockfd);
                printf(RED "[ BYE ] Connexion terminée !\n" RESET);
                pthread_exit(0);
                

            case 0:
                /* Récéption Bye {Type 0} */
                struct Bye byeFromClient;
                memcpy(&byeFromClient, buffer_in, sizeof(struct Bye));
                printf(GREEN "[ BYE ] %s\n" RESET, byeFromClient.data);
                break;
            case -1:
                /* Récéption Error {Type -1} */
                struct Error error;
                memcpy(&error, buffer_in, sizeof(struct Error));
                printf(RED "[ ERROR ] %s\n" RESET, error.data);
                break;
        }
    }
    close(sockfd);
    printf(RED "[ BYE ] Connexion terminée !\n" RESET);
    pthread_exit(0);
}

/**
 * @fn int open_connection
 * @brief Ouvre la connexion du client
 */
int open_connection(char * IP, int Port) {
    int sockfd;

    struct sockaddr_in serverAddr;
    int port = Port;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(IP);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    if (connect(sockfd, (struct sockaddr *) &serverAddr, sizeof (serverAddr)) != 0) {
        printf(RED "[ ERROR ] Connexion au serveur impossible ! Vérifiez les paramètres réseau et que le serveur est fonctionnel.\n" RESET);
        exit(-1);
    };

    return sockfd;
}
