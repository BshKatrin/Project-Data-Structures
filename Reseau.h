#ifndef __RESEAU_H__
#define __RESEAU_H__

#include <stdbool.h>
#include "Chaine.h"

typedef struct noeud Noeud;

/* Liste chainee de noeuds (pour la liste des noeuds du reseau ET les listes des voisins de chaque noeud) */
typedef struct cellnoeud
{
    Noeud *nd;              /* Pointeur vers le noeud stocke */
    struct cellnoeud *suiv; /* Cellule suivante dans la liste */
} CellNoeud;

/* Noeud du reseau */
struct noeud
{
    int num;            /* Numero du noeud */
    double x, y;        /* Coordonnees du noeud*/
    CellNoeud *voisins; /* Liste des voisins du noeud */
};

/* Liste chainee de commodites */
typedef struct cellCommodite
{
    Noeud *extrA, *extrB;       /* Noeuds aux extremites de la commodite */
    struct cellCommodite *suiv; /* Cellule suivante dans la liste */
} CellCommodite;

/* Un reseau */
typedef struct
{
    int nbNoeuds;              /* Nombre de noeuds du reseau */
    int gamma;                 /* Nombre maximal de fibres par cable */
    CellNoeud *noeuds;         /* Liste des noeuds du reseau */
    CellCommodite *commodites; /* Liste des commodites a relier */
} Reseau;

/* Creer un noeud à partir d'un numero et les coordonnees (x,y) */
Noeud *creer_noeud(int num, double x, double y);
/* Creer un CellNoeud qui pointe vers le noeud donne */
CellNoeud *creer_cell_noeud(Noeud *nd);
/* Ajouter dans un reseau un CellNoeud qui pointe vers le noeud donne.
Ajout est effectue en tete. Incremente le nombre de nbNoeuds d'un reseau */
void ajouter_noeud_reseau(Reseau *R, Noeud *nd);

/* Retourne True ssi les coordonnees de 2 noeuds sont egaux */
bool noeuds_egaux(Noeud *n1, Noeud *n2);
/* Retourne True ssi les coordonnes de noeud sont correspondent aux (x,y) donnes */
bool noeuds_x_y_egaux(Noeud *n, double x, double y);

/* Ajouter la commodite dans un reseau. Ajout est effectue en tete */
void ajouter_commodite(Reseau *R, Noeud *extrA, Noeud *extrB);
/* Ajouter noeud_voisin dans une liste des voisins de noeud_cour */
void ajout_voisin(Noeud *noeud_cour, Noeud *noeud_voisin);

/* Verifier si voisins contient deja le noeud.
Retourne pointeur vers le cellnoeud trouve s'il existe, sinon NULL*/
CellNoeud *recherche_voisin(CellNoeud *liste_voisins, Noeud *n);

Noeud *rechercheCreeNoeudListe(Reseau *R, double x, double y);
Reseau *reconstitueReseauListe(Chaines *C);

/* Liberer la memoire occupee par un reseau */
void liberer_reseau(Reseau **R);

/* Ecrire un reseau (des noeuds, des liaisons, des commodites) dans un fichier donne*/
void ecrireReseau(Reseau *R, FILE *f);
/* Retourne le nombre de liaisons contenues dans un reseau.
Le comptage est effectue uniquement dans un sens choisi */
int nbLiaisons(Reseau *R);
/* Retroune le nombre de commodites contenues dans un reseau */
int nbCommodites(Reseau *R);
/* Ecrire un reseau sous le format SVG (dans un fichier .html)*/
void afficheReseauSVG(Reseau *R, char *nomInstance);
#endif
