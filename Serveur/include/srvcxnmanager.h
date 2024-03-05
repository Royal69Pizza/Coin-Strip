/**
 * @file srvcxnmanager.h
 * 
 * @brief Serveur
 * 
 * @author Tom REINA-HIOT
 * @author Téo LANGUEBIEN
 * @author Valentin BRONSARD
 * @author Adrien BARRAT
 */

#ifndef SRVCXNMANAGER_H
#define SRVCXNMANAGER_H

#include "../../Common/common.h"

#define BUFFERSIZE 2048
#define MAXSIMULTANEOUSCLIENTS 2

typedef struct {
    int sockfd;
    struct sockaddr address;
    int addr_len;
    int index;
} connection_t;

typedef struct {
    connection_t *connection;
    TableauJeu *tableauDuJeu;
    int *joueur1;
    int *joueur2;
    int *aQuiLeTour;
} ThreadArgs;

void init_sockets_array();
void add(connection_t *connection, char buffer_out[]);
void del(connection_t *connection);
void *threadProcess(void *args);
int create_server_socket(char * IP, int port) ;

#endif /* SRVCXNMANAGER_H */

