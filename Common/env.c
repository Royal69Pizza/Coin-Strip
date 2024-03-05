/**
 * @file env.c
 * 
 * @brief Gestion de la configuration des variables d'environnement
 * 
 * @author Tom REINA-HIOT
 * @author Téo LANGUEBIEN
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../Common/ansiColors.h"

#define MAX_LINE_LENGTH 256

/**
 * @fn char * getEnvVar
 * @brief Récupère des valeurs dans un fichier de configuration
 */
char * getEnvVar(char *envFileName, char *envVariableName) {
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char *value = NULL;

    file = fopen(envFileName, "r");
    
    if (file == NULL) {
        perror(RED "[ ERROR ] Erreur lors de l'ouverture du fichier !" RESET);
        return NULL;
    }

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, envVariableName) != NULL) {
            value = strdup(strchr(line, '=') + 1);
            value[strcspn(value, "\n")] = 0;
            break;
        }
    }

    fclose(file);

    return value;
}