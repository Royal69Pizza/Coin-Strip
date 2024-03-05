/**
 * @file env.h
 * 
 * @brief Gestion de la configuration des variables d'environnement
 * 
 * @author Tom REINA-HIOT
 * @author Téo LANGUEBIEN
 */

#ifndef ENV_VAR
#define ENV_VAR

#define MAX_LINE_LENGTH 256

char * getEnvVar(char * envFileName, char * envVariableName);

#endif /* ENV_VAR */

