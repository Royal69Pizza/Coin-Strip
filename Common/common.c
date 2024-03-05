/**
 * @file common.c
 * 
 * @brief Fonctions communes au Serveur et Client
 * 
 * @author Tom REINA-HIOT
 * @author Téo LANGUEBIEN
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include "common.h"

/**
 * @fn void initTableauJeu
 * @brief Initialise le tableau pour le jeu
*/
void initTableauJeu(TableauJeu * tableauJeu) {
    tableauJeu->index = 0;
    for (int i = 0; i < SIZE_ARRAY; ++i) {
        tableauJeu->array[i] = 0;
        tableauJeu->index += 1;
    }

    tableauJeu->player = 0;
}

/**
 * @fn void initCoinsInTableauJeu
 * @brief Initialise les coins dans le tableau du jeu
*/
void initCoinsInTableauJeu(TableauJeu * tableauJeu) {
    srand(time(NULL));

    for (int i = 0; i < 5; ++i) {
        int index;

        do {
            index = rand() % tableauJeu->index;
        } while (tableauJeu->array[index] != 0);

        tableauJeu->array[index] = 1;
    }
}

/**
 * @fn bool verifieLesCoins
 * @brief Vérifie qu'il y a bien toujours 5 coins en jeu
*/
bool verifieLesCoins(TableauJeu * tableauJeu) {
    int sumOne = 0;
    for (int i = 0; i < tableauJeu->index; ++i) {
        if(tableauJeu->array[i] == 1){
            sumOne++;
        }
    }
    return (sumOne == 5 ? true : false);
}

/**
 * @fn void deplaceUnCoin
 * @brief Déplace un coin
*/
bool deplaceUnCoin(TableauJeu * tableauJeu, int indexDesire) {
    if (tableauJeu->array[indexDesire] == 0) {
        for (int i = indexDesire + 1; i < SIZE_ARRAY; i++) {
            if (tableauJeu->array[i] == 1) {
                tableauJeu->array[i] = 0;
                tableauJeu->array[indexDesire] = 1;
                break;
            }
        }
    } else {
        return false;
    }

    tableauJeu->index = SIZE_ARRAY;
    return true;
}

/**
 * @fn void afficheTableauDesCoins
 * @brief Affcihe le tableau des coins
*/
void afficheTableauDesCoins(TableauJeu * tableauJeu) {
    printf("Grille des Coins -> [");
    for (int i = 0; i < tableauJeu->index; ++i) {
        printf(" %i ",tableauJeu->array[i]);
    }
    printf("]\n");
}

/**
 * @fn void afficheTableauDesCoins
 * @brief Vérifie si la partie est terminée
*/
bool verifiePartieGagnee(TableauJeu * tableauJeu) {
    int sum = 0;
    for (int i = 0; i < 5; ++i) {
        sum += tableauJeu->array[i];
    }
    return (sum == 5 ? true : false);
}

/**
 * @fn void changePlayerTo1
 * @brief Donne le tour au player 1
*/
void changePlayerTo1(TableauJeu * tableauJeu) {
    tableauJeu->player = 1;
}

/**
 * @fn void changePlayerTo1
 * @brief Donne le tour au player 1
*/
void changePlayerTo0(TableauJeu * tableauJeu) {
    tableauJeu->player = 0;
}

/**
 * @fn bool isPlayer0
 * @brief Vérifie si c'est le player 0 (true) sinon c'est le player 1 (false)
*/
bool isPlayer0(TableauJeu * tableauJeu) {
    if (tableauJeu->player == 0) {
        return true;
    } else {
        return false;
    }
}

/**
 * @fn bool isIndexCorrect
 * @brief Vérifie si l'index choisi est correct [0 - 11]
*/
bool isIndexCorrect(int index) {
    if (index >= 0 && index <= 11) {
        return true;
    } else {
        return false;
    }
}