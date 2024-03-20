#ifndef __ARBRE_QUAT_H__
#define __ARBRE_QUAT_H__

#include "Reseau.h"

/* Arbre quaternaire contenant les noeuds du reseau */
typedef struct arbreQuat
{
    double xc, yc;        /* Coordonnees du centre de la cellule */
    double coteX;         /* Longueur de la cellule */
    double coteY;         /* Hauteur de la cellule */
    Noeud *noeud;         /* Pointeur vers le noeud du reseau */
    struct arbreQuat *so; /* Sous-arbre sud-ouest, pour x < xc et y < yc */
    struct arbreQuat *se; /* Sous-arbre sud-est, pour x >= xc et y < yc */
    struct arbreQuat *no; /* Sous-arbre nord-ouest, pour x < xc et y >= yc */
    struct arbreQuat *ne; /* Sous-arbre nord-est, pour x >= xc et y >= yc */
} ArbreQuat;

/* Détermine les coordonnées minimales et maximales des points constituant les différentes chaˆınes du réseau.
Stockage est effectue directement dans les variables passées en paramètre */
void chaineCoordMinMax(Chaines *C, double *xmin, double *ymin, double *xmax, double *ymax);

/* Créer une cellule de l’arbre quaternaire, de centre (xc,yc), de longueur coteX et de hauteur coteY.
Initialiser le noeud du réseau, les arbres nord-ouest, nord-est, sud-ouest et sud-est à NULL. */
ArbreQuat *creerArbreQuat(double xc, double yc, double coteX, double coteY);

/* Inserer un noeud dans un arbre quaternaire*/
void insererNoeudArbre(Noeud *n, ArbreQuat **a, ArbreQuat *parent);

/* Retourne un Noeud du réseau R correspondant au point de coordonnées (x,y) dans l’arbre quaternaire.
Si le noeud n'existe pas dans la réseau, créer le noeud avec (x, y), l'ajouter dans l'arbre et le réseau */
Noeud *rechercheCreeNoeudArbre(Reseau *R, ArbreQuat **a, ArbreQuat *parent, double x, double y);

/* Reconstruit le reseau à partir de la liste des chaines C
et en utilisant un arbre quarternaire.
A la fin desalloue la memoire occupee par un arbre utilisé. */
Reseau *reconstitueReseauArbre(Chaines *C);

/* Liberer la mémoire occupée par un arbre */
void liberer_arbre(ArbreQuat *A);

/* Affichage d'un arbre construit. N'est pas demandée. Juste pour les tests.
Ne supprime pas les noeuds vers qui pointent des feilles */
void afficher_arbre(ArbreQuat *A, int level);
#endif
