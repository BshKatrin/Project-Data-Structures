#ifndef __CHAINE_H__
#define __CHAINE_H__
#include <stdio.h>

/* Liste chainee de points */
typedef struct cellPoint
{
  double x, y;            /* Coordonnees du point */
  struct cellPoint *suiv; /* Cellule suivante dans la liste */
} CellPoint;

/* Celllule d une liste (chainee) de chaines */
typedef struct cellChaine
{
  int numero;              /* Numero de la chaine */
  CellPoint *points;       /* Liste des points de la chaine */
  struct cellChaine *suiv; /* Cellule suivante dans la liste */
} CellChaine;

/* L'ensemble des chaines */
typedef struct
{
  int gamma;           /* Nombre maximal de fibres par cable */
  int nbChaines;       /* Nombre de chaines */
  CellChaine *chaines; /* La liste chainee des chaines */
} Chaines;

/* Creer un point avec les coordonnees (x, y) */
CellPoint *creer_point(double x, double y);
/* Creer une liste des points à partir d'une chaine de characters */
CellPoint *creer_liste_points(int nb_points, char *l);
/* Inserer une chaine dans une liste des chaines */
CellChaine *insertion(CellChaine *l, CellChaine *new);

/* Creer Chaines selon le contenu d'un fichier */
Chaines *lectureChaines(FILE *f);
/* Ecrire le contenu de Chaines dans le fichier */
void ecrireChaines(Chaines *C, FILE *f);
/* Creer le fichier SVG (le fichier .html) à partir d'un struct Chaines*/
void afficheChainesSVG(Chaines *C, char *nomInstance);

/* Compter le nombre des points dans une chaine */
int comptePointsChaine(CellChaine *chaine);
/* Compter le nombre des points dans toutes les chaines.
Si le point apparait plusieurs fois, il est compte plusieurs fois */
int comptePointsTotal(Chaines *C);

/* Calculer la distance entre deux points */
double distancePoint(CellPoint *a, CellPoint *b);
/* Compter la longueur d'une chaine */
double longueurChaine(CellChaine *c);
/* Compter la longueur de toutes les chaines*/
double longueurTotale(Chaines *C);

/* Liberer la memoire occupee par une liste des points*/
void liberer_liste_point(CellPoint *l);
/* Liberer la memoire occupee par chaines */
void liberer_chaines(Chaines **C);

/* Generer plusieures chaines, chacune contient nbPointsChaine. Les points sont générés
aléatoirement entre (0, 0) et (xmax, ymax) */
Chaines *generationAleatoire(int nbChaines, int nbPointsChaine, int xmax, int ymax);
#endif
