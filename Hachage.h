#ifndef __HACHAGE_H__
#define __HACHAGE_H__

#include "Reseau.h"

typedef struct
{
  int nbElement; // pas necessaire ici
  int tailleMax;
  CellNoeud **T;
} TableHachage;

/* N'EST PAS DEMANDER. Pour les tests. SUPPRIMER apres */
void afficher_tab_hash(TableHachage *H);

/* Générer un clef à partir de (x, y) de point */
double clef(double x, double y);

/* Générer l'indice de la case dans une table de hachage à partir d'un clef */
int hash(double clef, int tab_taille);

/* Creer et initialiser une table de hachage de la taille donnée */
TableHachage *creer_tab(unsigned int tailleMax);

/* Inserer un element dans une table de hachage.
Résolution des collisions par chaînage */
void inserer(TableHachage *H, Noeud *nd);

/* Retourne un point correspondant à (x,y) dans la table de hachage.
Si tel point n'existe pas, créer un noeud et l’ajouter dans H
ainsi que dans la liste des noeuds du réseau de R */
Noeud *rechercheCreeNoeudHachage(Reseau *R, TableHachage *H, double x, double y);

/* Reconstruit le reseau à partir de la liste des chaines C
et en utilisant une table de hachage de taille M.
A la fin desalloue la memoire occupee par une table de hachage utilisee. */
Reseau *reconstitueReseauHachage(Chaines *C, int M);

/* Liberer la memoire occupe par une table de hachage */
void liberer_tab(TableHachage *H);
#endif