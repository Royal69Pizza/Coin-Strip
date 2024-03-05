/**
 * @file main.c
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
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <ctype.h>

#include "clientcxnmanager.h"
#include "../../Common/env.c"
#include "../../Common/ansiColors.h"
#include "../../Common/structures.h"

/**
 * @fn int main()
 * @brief Programme principal du Client
 */
int main(int argc, char const *argv[])
{
    printf(GREEN "[ Xx_ôOô_xX ] Bienvenue joueur anonyme ! [ Xx_ôOô_xX ]\n\n" RESET);

    /* Configuration de la connexion au serveur */
    char * envFileName = ".env";
    char * addressVariableName = "IP_ADDRESS";
    char * IP = getEnvVar(envFileName, addressVariableName);
    char * portVariableName = "PORT";
    char * charsPortValue = getEnvVar(envFileName, portVariableName);
    int port = atoi(charsPortValue);

    if (IP && port) {
        /* DEBUG *///printf(YELLOW "[ INFO ] Adresse IP récupérée : %s\n" RESET, IP);
        /* DEBUG *///printf(YELLOW "[ INFO ] port récupéré : %d\n" RESET, port);

        int sockfd;
        int status = 0;
        char msg[100];
        pthread_t thread;

        sockfd = open_connection(IP, port);

        pthread_create(&thread, 0, threadProcess, &sockfd);
        pthread_detach(thread);
        
        do {
            fgets(msg, 100, stdin);

            /* On fait la différence entre les messages et les mouvements des joueurs */
            if (!isdigit(msg[0])) {
                /* On met aussi \n car fgets ajoute ce caractère */
                if (strcmp(msg, "hello\n") == 0) {
                    /* Envoi Hello {Type 1} */
                    struct Hello helloFromClient = {1, "Hello d'un joueur anonyme !"};
                    char dataOfHelloFromClient[sizeof(struct Hello)];
                    memcpy(dataOfHelloFromClient, &helloFromClient, sizeof(struct Hello));
                    if (write(sockfd, dataOfHelloFromClient, sizeof(struct Hello)) != -1) {
                        /* DEBUG *///printf(YELLOW "[ POST ] Message sortant du serveur de type %d !\n" RESET, helloFromClient.type);
                    }
                }
                if (strcmp(msg, "bye\n") == 0) {
                    
                    /* Envoi Exit {Type 0} */
                    struct Bye bye = {0, "Bye d'un joueur anonyme !"};
                    char dataOfByeFromClient[sizeof(struct Bye)];
                    memcpy(dataOfByeFromClient, &bye, sizeof(struct Bye));
                    if (write(sockfd, dataOfByeFromClient, sizeof(struct Bye)) != -1) {
                        /* DEBUG *///printf(YELLOW "[ INFO ] Un message de type %d à été envoyé au serveur !\n" RESET, bye.type);
                        printf(RED "[ BYE ] Vous allez être déconnecté du serveur !\n" RESET);
                        status = -1;
                    }
                }
                
            } else {
                if (sizeof(atoi(msg)) == sizeof(int) )
                {
                    if (aQuiLeTour == 1)
                    {
                        int choixEnInt = atoi(msg);

                        /* Vérification de l'index */
                        if (choixEnInt >= 0 && choixEnInt <= 11) {
                        /* Envoi Choice {Type 5} */
                        
                            struct Choice choice = {5, choixEnInt};
                            char dataOfChoiceFromClient[sizeof(struct Choice)];
                            memcpy(dataOfChoiceFromClient, &choice, sizeof(struct Choice));
                            if (write(sockfd, dataOfChoiceFromClient, sizeof(struct Choice)) != -1) {
                            /* DEBUG *///printf(YELLOW "[ INFO ] Un message de type %d à été envoyé au serveur !\n" RESET, choice.type);
                            }
                        } else {
                            printf(YELLOW "[ CHOICE ] L'index séléctionné n'est pas valide ! Vous pouvez de nouveau essayer.\n" RESET);
                        }
                    }
                    
                    
                }
            }
        } while (status != -1);

        return (EXIT_SUCCESS);

    } else {
        perror(RED "[ ERROR ] Une variable d'environnement est introuvable ! Tous les clients vont être déconnectés et le serveur va s'arrêter." RESET);
        exit(-5);
    }
}
