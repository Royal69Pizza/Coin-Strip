/**
 * @file srvcxnmanager.c
 * 
 * @brief Serveur
 * 
 * @author Tom REINA-HIOT
 * @author Téo LANGUEBIEN
 * @author Valentin BRONSARD
 * @author Adrien BARRAT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>

#include "srvcxnmanager.h"
#include "../../Common/ansiColors.h"
#include "../../Common/structures.h"
#include "../../Common/common.h"
#include "../../Common/common.c"

connection_t * connections[MAXSIMULTANEOUSCLIENTS];

/**
 * @fn void init_sockets_array
 * @brief Initialise le tableau des sockets
 */
void init_sockets_array() {
    for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
        connections[i] = NULL;
    }
}

/**
 * @fn void add
 * @brief Ajoute une connexion. Si la dernière connexion est en trop on la déconnecte
 * @param connection_t * connection
 * @param char buffer_out[]
 */
void add(connection_t * connection, char buffer_out[]) {
    for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
        if (connections[i] == NULL) {
            connections[i] = connection;
            return;
        }
    }

    /* Envoi Error {Type -1} */
    struct Error error = {-1, "La limite de connexions est atteinte ! Vous allez être automatiquement déconnecté.\n"};
    char dataOfError[sizeof(struct Error)];
    memcpy(dataOfError, &error, sizeof(struct Error));
    if (write(connection->sockfd, dataOfError, sizeof(struct Error)) != -1) {
        printf(YELLOW "[ POST ] Message sortant du serveur de type %d envoyé à la connexion #%i !\n" RESET, error.type, connection->index);
    }
    
    printf(RED "[ WARNING ] La limite de %d connexions est atteinte ! La nouvelle connexion #%i va être déconnecté automatiquement.\n" RESET, MAXSIMULTANEOUSCLIENTS, connection->index);
    close(connection->sockfd);  
    free(connection);
    pthread_exit(0);
}

/**
 * @fn void del
 * @brief Supprime une connexion
 * @param connection_t * connection
 */
void del(connection_t *connection) {
    for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
        if (connections[i] == connection) {
            connections[i] = NULL;
            return;
        }
    }

    /* Envoi Error {Type -1} */
    struct Error error = {-1, "Une erreur c'est produite sur le serveur ! Vous allez être automatiquement déconnecté.\n"};
    char dataOfError[sizeof(struct Error)];
    memcpy(dataOfError, &error, sizeof(struct Error));
    if (write(connection->sockfd, dataOfError, sizeof(struct Error)) != -1) {
        printf(YELLOW "[ POST ] Message sortant du serveur de type %d envoyé à la connexion #%i !\n" RESET, error.type, connection->index);
    }
    
    perror(RED "[ ERROR ] La tentative d'une suppression de connexion à échoué ! Tous les clients vont être déconnectés et le serveur va s'arrêter." RESET);
    exit(-5);
}

int numberOfHello = 0;

/**
 * @fn void * threadProcess
 * @brief Thread pour gérer plusieurs connexions 
 */
void * threadProcess(void *args) {

    ThreadArgs *threadArgs = (ThreadArgs *)args;

    char buffer_in[BUFFERSIZE];
    char buffer_out[BUFFERSIZE];

    int len;

    if (!args) pthread_exit(0);
    connection_t *connection = threadArgs->connection;
    TableauJeu *tableauDuJeu = threadArgs->tableauDuJeu;
    int joueur1 = threadArgs->joueur1;
    int joueur2 = threadArgs->joueur2;

    add(connection, buffer_out);

    printf(GREEN "[ OK ] Nouvelle connexion #%i enregistrée !\n" RESET, connection->index);

    /* Envoi Hello {Type 1} */
    struct Hello helloFromServer = {1, "Hello du serveur ! Le jeu va bientôt commencer !"};
    char dataOfHelloFromServer[sizeof(struct Hello)];
    memcpy(dataOfHelloFromServer, &helloFromServer, sizeof(struct Hello));
    if (write(connection->sockfd, dataOfHelloFromServer, sizeof(struct Hello)) != -1) {
        printf(YELLOW "[ POST ] Message sortant du serveur de type %d envoyé à la connexion #%i !\n" RESET, helloFromServer.type, connection->index);
    } else {
        perror(RED "[ ERROR ] L'envoi d'un socket n'a pas fonctionné ! Tous les clients vont être déconnectés et le serveur va s'arrêter.\n" RESET);
        exit(-5);
    }

    while ((len = read(connection->sockfd, buffer_in, BUFFERSIZE)) > 0) {

        /* Le router récupère le type du message et redirige vers le type de message correspondant */
        struct Router router;
        memcpy(&router, buffer_in, sizeof(struct Router));
        printf(YELLOW "[ GET ] Message entrant du serveur de type %d !\n" RESET, router.type);

        switch (router.type) {
            case 1:
                /* Récéption Hello {Type 1} */
                struct Hello helloFromClient;
                memcpy(&helloFromClient, buffer_in, sizeof(struct Hello));
                printf(GREEN "[ HELLO ] %s\n" RESET, helloFromClient.data);

                numberOfHello = numberOfHello + 1;

                /* Si les deux clients sont connectés sinon on alerte qu'un client à fait play */
                if (numberOfHello == 2) {

                    afficheTableauDesCoins(tableauDuJeu);

                    /* Je joueur 1 commence */
                    struct Game gameJoueur1 = {4, * tableauDuJeu, "C'est à votre tour ! Choisissez un nombre entre 0 et 12 en entrant dans le terminal l'index  choisi",1};
                    char dataOfGameJoueur1[sizeof(struct Game)];
                    memcpy(dataOfGameJoueur1, &gameJoueur1, sizeof(struct Game));
                    if (write(connections[joueur1]->sockfd, dataOfGameJoueur1, sizeof(struct Game)) != -1) {
                        printf(YELLOW "[ POST ] Message sortant du serveur de type %d envoyé à la connexion #%i !\n" RESET, gameJoueur1.type, connections[joueur1]->index);
                    } else {
                        perror(RED "[ ERROR ] L'envoi d'un socket n'a pas fonctionné ! Tous les clients vont être déconnectés et le serveur va s'arrêter.\n" RESET);
                        exit(-5);
                    }

                    /* Le joueur 2 attend */
                    struct Wait waitFromServer = {3, "Le jeu à commencé mais ce n'est pas votre tour.",0};
                    char dataOfWaitFromServer[sizeof(struct Wait)];
                    memcpy(dataOfWaitFromServer, &waitFromServer, sizeof(struct Wait));
                    if (write(connections[joueur2]->sockfd, dataOfWaitFromServer, sizeof(struct Wait)) != -1) {
                        printf(YELLOW "[ POST ] Message sortant du serveur de type %d envoyé à la connexion #%i !\n" RESET, waitFromServer.type, connections[joueur2]->index);
                    } else {
                        perror(RED "[ ERROR ] L'envoi d'un socket n'a pas fonctionné ! Tous les clients vont être déconnectés et le serveur va s'arrêter.\n" RESET);
                        exit(-5);
                    }

                    changePlayerTo1(tableauDuJeu);
                }

                break;
            case 5:
                /* Récéption Choice {Type 5} */
                struct Choice choiceFromClient;
                memcpy(&choiceFromClient, buffer_in, sizeof(struct Choice));
                printf(GREEN "[ CHOICE ] %d\n" RESET, choiceFromClient.choice);
               
                bool pion = deplaceUnCoin(tableauDuJeu, choiceFromClient.choice);
                assert(verifieLesCoins(tableauDuJeu) == true);

                afficheTableauDesCoins(tableauDuJeu);

                if (!verifiePartieGagnee(tableauDuJeu)) {
                    /* Renvoie au deux clients le nouveau positionnement des coins */
                    if (pion == false) {
                        if (isPlayer0(tableauDuJeu)) {
                            changePlayerTo1(tableauDuJeu);
                        } else {
                             changePlayerTo0(tableauDuJeu);
                        }
                    }
                    
                    /* Vérifie c'est a qui de jouer */
                    if (isPlayer0(tableauDuJeu)) {
                        /* Pour le joueur 1 */
                       
                        if (pion == false) {
                            struct Game gameJoueur1 = {4, * tableauDuJeu, "La case choisie est déja occupée ! Vous pouvez rejouer.",1};
                            char dataOfGameJoueur1FromServer[sizeof(struct Game)];
                            memcpy(dataOfGameJoueur1FromServer, &gameJoueur1, sizeof(struct Game));

                            if (write(connections[joueur1]->sockfd, dataOfGameJoueur1FromServer, sizeof(struct Game)) != -1) {
                                printf(YELLOW "[ POST ] Message sortant du serveur de type %d envoyé à la connexion #%i !\n" RESET, gameJoueur1.type, connections[joueur1]->index);
                            } else {
                                perror(RED "[ ERROR ] L'envoi d'un socket n'a pas fonctionné ! Tous les clients vont être déconnectés et le serveur va s'arrêter.\n" RESET);
                                exit(-5);
                            }
                        } else {
                            struct Game  gameJoueur1 = {4, * tableauDuJeu, "C'est à votre tour ! Choisissez un nombre entre 0 et 12 en entrant dans le terminal l'index choisi",1};
                            char dataOfGameJoueur1FromServer[sizeof(struct Game)];
                            memcpy(dataOfGameJoueur1FromServer, &gameJoueur1, sizeof(struct Game));

                            if (write(connections[joueur1]->sockfd, dataOfGameJoueur1FromServer, sizeof(struct Game)) != -1) {
                                printf(YELLOW "[ POST ] Message sortant du serveur de type %d envoyé à la connexion #%i !\n" RESET, gameJoueur1.type, connections[joueur1]->index);
                            } else {
                                perror(RED "[ ERROR ] L'envoi d'un socket n'a pas fonctionné ! Tous les clients vont être déconnectés et le serveur va s'arrêter.\n" RESET);
                                exit(-5);
                            }
                        }
                        
                        /* Pour le joueur 2 */
                        struct Game gameJoueur2 = {4, * tableauDuJeu, "C'est au tour de votre adversaire !",0};
                        char dataOfGameJoueur2FromServer[sizeof(struct Game)];
                        memcpy(dataOfGameJoueur2FromServer, &gameJoueur2, sizeof(struct Game));

                        if (write(connections[joueur2]->sockfd, dataOfGameJoueur2FromServer, sizeof(struct Game)) != -1) {
                            printf(YELLOW "[ POST ] Message sortant du serveur de type %d envoyé à la connexion #%i !\n" RESET, gameJoueur2.type, connections[joueur2]->index);
                        } else {
                            perror(RED "[ ERROR ] L'envoi d'un socket n'a pas fonctionné ! Tous les clients vont être déconnectés et le serveur va s'arrêter.\n" RESET);
                            exit(-5);
                        }

                        changePlayerTo1(tableauDuJeu);
                    } else {
                        /* Pour le joueur 1 */
                        /* Renvoie le tableau changé */
                        struct Game gameJoueur1 = {4, * tableauDuJeu, "C'est au tour de votre adversaire !",0};
                        char dataOfGameJoueur1FromServer[sizeof(struct Game)];
                        memcpy(dataOfGameJoueur1FromServer, &gameJoueur1, sizeof(struct Game));

                        if (write(connections[joueur1]->sockfd, dataOfGameJoueur1FromServer, sizeof(struct Game)) != -1) {
                            printf(YELLOW "[ POST ] Message sortant du serveur de type %d envoyé à la connexion #%i !\n" RESET, gameJoueur1.type, connections[joueur1]->index);
                        } else {
                            perror(RED "[ ERROR ] L'envoi d'un socket n'a pas fonctionné ! Tous les clients vont être déconnectés et le serveur va s'arrêter.\n" RESET);
                            exit(-5);
                        }
                        
                        /* Pour le joueur 2 */
                        
                        if (pion == false) {
                            struct Game gameJoueur2 = {4, * tableauDuJeu, "La case choisie est déja occupée ! Vous pouvez rejouer.",1};
                            char dataOfGameJoueur2FromServer[sizeof(struct Game)];
                            memcpy(dataOfGameJoueur2FromServer, &gameJoueur2, sizeof(struct Game));

                            if (write(connections[joueur2]->sockfd, dataOfGameJoueur2FromServer, sizeof(struct Game)) != -1) {
                                printf(YELLOW "[ POST ] Message sortant du serveur de type %d envoyé à la connexion #%i !\n" RESET, gameJoueur2.type, connections[joueur2]->index);
                            } else {
                                perror(RED "[ ERROR ] L'envoi d'un socket n'a pas fonctionné ! Tous les clients vont être déconnectés et le serveur va s'arrêter.\n" RESET);
                                exit(-5);
                            }
                        } else {
                            struct Game gameJoueur2 = {4, * tableauDuJeu, "C'est à votre tour ! Choisissez un nombre entre 0 et 12 en entrant dans le terminal l'index choisi",1};
                            char dataOfGameJoueur2FromServer[sizeof(struct Game)];
                            memcpy(dataOfGameJoueur2FromServer, &gameJoueur2, sizeof(struct Game));

                            if (write(connections[joueur2]->sockfd, dataOfGameJoueur2FromServer, sizeof(struct Game)) != -1) {
                                printf(YELLOW "[ POST ] Message sortant du serveur de type %d envoyé à la connexion #%i !\n" RESET, gameJoueur2.type, connections[joueur2]->index);
                            } else {
                                perror(RED "[ ERROR ] L'envoi d'un socket n'a pas fonctionné ! Tous les clients vont être déconnectés et le serveur va s'arrêter.\n" RESET);
                                exit(-5);
                            }
                        }

                        changePlayerTo0(tableauDuJeu);
                    } 
                } else {
                    
                    /* Si un joueur à gagné on vérifie qui et on envoie les messages correspondants */
                    if (!isPlayer0(tableauDuJeu)) {
                        /* Envoi Victory {Type 6} */
                        struct Victory victoryFromServer = {6, "Vous avez gagné !"};
                        char dataOfVictoryFromServer[sizeof(struct Victory)];
                        memcpy(dataOfVictoryFromServer, &victoryFromServer, sizeof(struct Victory));
                        if (write(connections[joueur1]->sockfd, dataOfVictoryFromServer, sizeof(struct Victory)) != -1) {
                            printf(YELLOW "[ POST ] Message sortant du serveur de type %d envoyé à la connexion #%i !\n" RESET, victoryFromServer.type, connections[joueur1]->sockfd);
                        } else {
                            perror(RED "[ ERROR ] L'envoi d'un socket n'a pas fonctionné ! Tous les clients vont être déconnectés et le serveur va s'arrêter.\n" RESET);
                            exit(-5);
                        }

                        /* Envoi Defeat {Type 7} */
                        struct Defeat defeatFromServer = {7, "Vous avez perdu !"};
                        char dataOfDefeatFromServer[sizeof(struct Victory)];
                        memcpy(dataOfDefeatFromServer, &defeatFromServer, sizeof(struct Victory));
                        if (write(connections[joueur2]->sockfd, dataOfDefeatFromServer, sizeof(struct Victory)) != -1) {
                            printf(YELLOW "[ POST ] Message sortant du serveur de type %d envoyé à la connexion #%i !\n" RESET, defeatFromServer.type, connections[joueur2]->sockfd);
                        } else {
                            perror(RED "[ ERROR ] L'envoi d'un socket n'a pas fonctionné ! Tous les clients vont être déconnectés et le serveur va s'arrêter.\n" RESET);
                            exit(-5);
                        }
                    } else {
                        /* Envoi Victory {Type 6} */
                        struct Victory victoryFromServer = {6, "Vous avez gagné !"};
                        char dataOfVictoryFromServer[sizeof(struct Victory)];
                        memcpy(dataOfVictoryFromServer, &victoryFromServer, sizeof(struct Victory));
                        if (write(connections[joueur2]->sockfd, dataOfVictoryFromServer, sizeof(struct Victory)) != -1) {
                            printf(YELLOW "[ POST ] Message sortant du serveur de type %d envoyé à la connexion #%i !\n" RESET, victoryFromServer.type, connections[joueur2]->sockfd);
                        } else {
                            perror(RED "[ ERROR ] L'envoi d'un socket n'a pas fonctionné ! Tous les clients vont être déconnectés et le serveur va s'arrêter.\n" RESET);
                            exit(-5);
                        }

                        /* Envoi Defeat {Type 7} */
                        struct Defeat defeatFromServer = {7, "Vous avez perdu !"};
                        char dataOfDefeatFromServer[sizeof(struct Victory)];
                        memcpy(dataOfDefeatFromServer, &defeatFromServer, sizeof(struct Victory));
                        if (write(connections[joueur1]->sockfd, dataOfDefeatFromServer, sizeof(struct Victory)) != -1) {
                            printf(YELLOW "[ POST ] Message sortant du serveur de type %d envoyé à la connexion #%i !\n" RESET, defeatFromServer.type, connections[joueur1]->sockfd);
                        } else {
                            perror(RED "[ ERROR ] L'envoi d'un socket n'a pas fonctionné ! Tous les clients vont être déconnectés et le serveur va s'arrêter.\n" RESET);
                            exit(-5);
                        }
                    }
                }

                break;
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

        memset(buffer_in, '\0', BUFFERSIZE);
    }
    printf(YELLOW "[ INFO ] La connexion vers le client %i a pris fin. \n" RESET, connection->index);
    close(connection->sockfd);
    del(connection);
    free(connection);
    pthread_exit(0);
}

/**
 * @fn int create_server_socket
 * @brief Création du socket
 */
int create_server_socket(char * IP, int Port) {
    int sockfd = -1;
    struct sockaddr_in address;
    int port = Port;

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd <= 0) {
        fprintf(stderr, RED "%s: [ ERROR ] Impossible de créer le socket ! Tous les clients vont être déconnectés et le serveur va s'arrêter.\n" RESET);
        return -3;
    }

    address.sin_family = AF_INET;
    //address.sin_addr.s_addr = INADDR_ANY;
    address.sin_addr.s_addr = inet_addr(IP);
    address.sin_port = htons(port);

    int reuse = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char*) &reuse, sizeof (reuse));

    if (bind(sockfd, (struct sockaddr *) &address, sizeof (struct sockaddr_in)) < 0) {
        fprintf(stderr, RED "[ ERROR ] Impossible de lier le socket au port ! %d ! Tous les clients vont être déconnectés et le serveur va s'arrêter.\n" RESET, port);
        return -4;
    }

    return sockfd;
}
