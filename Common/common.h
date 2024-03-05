/**
 * @file common.h
 * 
 * @author Tom REINA-HIOT
 * @author Téo LANGUEBIEN
 */

#ifndef COMMON
#define COMMON

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define SIZE_ARRAY 13

typedef struct {
    int index;
    int array[SIZE_ARRAY];
    int player;
} TableauJeu;

void initTableauJeu(TableauJeu * tableauJeu);
void initCoinsInTableauJeu(TableauJeu * tableauJeu);
bool verifieLesCoins(TableauJeu * tableauJeu);
bool deplaceUnCoin(TableauJeu * tableauJeu, int indexDesire);
void afficheTableauDesCoins(TableauJeu * tableauJeu);
bool verifiePartieGagnee(TableauJeu * tableauJeu);
void changePlayerTo0(TableauJeu * tableauJeu);
void changePlayerTo1(TableauJeu * tableauJeu);
bool isPlayer0(TableauJeu * tableauJeu);

#endif //COMMON
