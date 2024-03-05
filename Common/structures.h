/**
 * @file structures.h
 * 
 * @brief Structures de communication entre serveur et client
 * 
 * @author Tom REINA-HIOT
 * @author Téo LANGUEBIEN
 * @author Valentin BRONSARD
 * @author Adrien BARRAT
 */

#ifndef STRUCTURES
#define STRUCTURES

#include "common.h"

/**
 * Structure Router {Type x}
*/
struct Router {
    int type;
    char data[256];
};

/**
 * Structure Hello {Type 1}
*/
struct Hello {
    int type;
    char data[256];
};

/**
 * Structure Bye {Type 0}
*/
struct Bye {
    int type;
    char data[256];
};

/**
 * Structure Wait {Type 3}
*/
struct Wait {
    int type;
    char data[256];
};

/**
 * Structure Game {Type 4}
*/
struct Game {
    int type;
    TableauJeu tableau;
    char data[256];
    int tour;
};

/**
 * Structure Choice {Type 5}
*/
struct Choice {
    int type;
    int choice;
};

/**
 * Structure Victory {Type 6}
*/
struct Victory {
    int type;
    char data[256];
};

/**
 * Structure Defeat {Type 7}
*/
struct Defeat {
    int type;
    char data[256];
};

/**
 * Structure Error {Type -1}
*/
struct Error {
    int type;
    char data[256];
};

#endif /* STRUCTURES */
