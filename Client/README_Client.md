# Projet-C-CoinStrip-Client

## [](#auteur)Auteurs

[@Auteur](/author  "Pavel") Tom REINA-HIOT | [@Auteur](/author  "Pavel") Téo LANGUEBIEN | [@Auteur](/author  "Pavel") Adrien BARRAT | [@Auteur](/author  "Pavel") Valentin BRONSARD

## [](#comment-executer-client)Comment exécuter le client

Modifier le fichier ".env" du client en renseignant l'adresse IP de la machine qui fera office de client :
```
IP_ADDRESS=x.x.x.x
PORT=7799
```

Ouvrir un terminal Bash dans le répertoire racine du projet Client.

Taper `make clean` pour supprimer tous les anciens fichiers d'exécutions.

Taper `make` afin de compiler le projet, puis `make run` pour l'exécuter.

**ATTENTION :** Le serveur doit impérativement être lancé avant les clients

## [](#coinstrip-regles)Règles du CoinStrip

Le CoinStrip est un jeu qui se joue à **deux** sur un tableau (horizontal) de **13 cases** numérotées de 0 à 12.

Sur ce tableau, à chaque début de partie **5 pièces** sont disposées aléatoirement dans ces cases.

Les joueurs joueront à tour de rôle et devront choisir une **case vide** où la pièce la plus à droite de cette case sera déplacée.

Le dernier qui parvient à déplacer une pièce et donc de **remplir les 5 premières cases** de pièces remporte la partie.

## [](#comment-jouer)Comment jouer

Une fois que les deux clients sont connectés, le serveur pourra lancer le jeu. Un joueur sera choisi aléatoirement par le serveur pour commencer.

Le joueur qui commence recevra le tableau avec les pièces ainsi qu'un message lui indiquant de jouer et devra donc saisir un numéro d'index valable (entre 0 et 11).

Pendant ce temps, l'autre joueur verra un message d'attente avant de recevoir à son tour le tableau lorsque le premier joueur à jouer.

La partie suivra son cours, à chaque tour le serveur vérifie s'il y a un gagnant ou non, si oui, chaque joueur verra son résultat affiché et la partie prendra fin.

## [](#generer-documentation)Générer la Documentation

Taper `make documentation` pour générer la documentation dans le dossier 'documentation'.

Pour recréer le Doxyfile pour la configuration de la documentation, taper `doxygen -g Doxyfile`.

## [](#prerequis-vs-code)Prérequis VS Code

Extensions :
- C/C++ Extension Pack
- C/C++ Project Generator
- CMake Tools
- Makefile Creator
- Makefile Tools