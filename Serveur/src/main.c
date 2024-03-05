/**
 * @file main.c
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
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

#include "srvcxnmanager.h"
#include "../../Common/env.c"
#include "../../Common/ansiColors.h"
#include "../../Common/structures.h"

/* Nombre maximal de clients */
#define MAXCLIENTS 2

/**
 * @fn int main()
 * @brief Programme principal du Serveur
 */
int main(int argc, char** argv) {
    printf(GREEN "[ Xx_ôOô_xX ] Bienvenue sur la console du serveur ! [ Xx_ôOô_xX ]\n\n" RESET);

    /* Configuration de la connexion au serveur */
    char * envFileName = ".env";
    char * addressVariableName = "IP_ADDRESS";
    char * IP = getEnvVar(envFileName, addressVariableName);
    char * portVariableName = "PORT";
    char * charsPortValue = getEnvVar(envFileName, portVariableName);
    int port = atoi(charsPortValue);

    if (IP && port) {
        printf(YELLOW "[ INFO ] Adresse IP récupérée : %s\n" RESET, IP);
        printf(YELLOW "[ INFO ] port récupéré : %d\n" RESET, port);

        int sockfd = -1;
        int index = 1;
        connection_t *connection;
        pthread_t thread;

        /* Cette structure permet dans un socket d'envoyer les valeurs communes aux clients, pour voir les modifications en direct */
        ThreadArgs *threadArgs;

        init_sockets_array();
        sockfd = create_server_socket(IP, port);

        if (listen(sockfd, 50) < 0) {
            fprintf(stderr, RED "%s: [ ERROR ] Impossible d'écouter sur le port ! Tous les clients vont être déconnectés et le serveur va s'arrêter.\n" RESET, argv[0]);
            return -5;
        }

        printf(YELLOW "[ INFO ] Seveur prêt en attente de connexions ...\n" RESET);

        /* On définit ici le jeu pour que tous les clients aient la même partie */
        TableauJeu * tableauDuJeu = malloc(sizeof(TableauJeu));
        initTableauJeu(tableauDuJeu);
        initCoinsInTableauJeu(tableauDuJeu);
        assert(verifieLesCoins(tableauDuJeu) == true);

        /* Choix aléatoire du joueur qui va débuter */
        srand(time(NULL));
        int *joueur1, *joueur2;
        joueur1 = rand() % 2;
        if (joueur1 == 0) {
            joueur2 = 1;
        } else {
            joueur2 = 0;
        }
        
        while (true) {
            connection = (connection_t *) malloc(sizeof (connection_t));
            threadArgs = (ThreadArgs *) malloc(sizeof(ThreadArgs));
            threadArgs->connection = connection;
            threadArgs->tableauDuJeu = tableauDuJeu;
            threadArgs->joueur1 = joueur1;
            threadArgs->joueur2 = joueur2;

            connection->sockfd = accept(sockfd, &connection->address, &connection->addr_len);
            connection->index = index++;
            
            if (connection->sockfd <= 0) {
                free(connection);
            } else {
                pthread_create(&thread, 0, threadProcess, (void *) threadArgs);
                pthread_detach(thread);
            }
        }

        free(tableauDuJeu);
        free(threadArgs);
        
        return (EXIT_SUCCESS);

    } else {
        perror(RED "[ ERROR ] Une variable d'environnement est introuvable ! Tous les clients vont être déconnectés et le serveur va s'arrêter." RESET);
        exit(-5);
    }
}