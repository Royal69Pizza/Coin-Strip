/**
 * @file clientcxnmanager.h
 * 
 * @brief Client
 * 
 * @author Tom REINA-HIOT
 * @author Téo LANGUEBIEN
 * @author Valentin BRONSARD
 * @author Adrien BARRAT
 */

#ifndef CLIENTCXNMANAGER_H
#define CLIENTCXNMANAGER_H

#define BUFFERSIZE 2048
extern int aQuiLeTour;

void *threadProcess(void * ptr);
int open_connection(char * IP, int Port);

#endif /* CLIENTCXNMANAGER_H */