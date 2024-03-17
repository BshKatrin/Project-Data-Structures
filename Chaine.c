#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "SVGLib/SVGwriter.h"
#include "Chaine.h"

CellPoint *creer_point(double x, double y)
{
    CellPoint *new = malloc(sizeof(CellPoint));
    new->suiv = NULL;
    new->x = x;
    new->y = y;
    return new;
}

CellPoint *creer_liste_points(int nb_points, char *l)
{
    if (!nb_points)
        return NULL;
    double x, y;
    char *reste;
    x = strtod(l, &reste);
    y = strtod(reste, &reste);
    CellPoint *new = creer_point(x, y);
    new->suiv = creer_liste_points(nb_points - 1, reste);
    return new;
}

// Insère en fin de liste chaînée
CellChaine *insertion(CellChaine *l, CellChaine *new)
{
    if (!l)
        return new;
    l->suiv = insertion(l->suiv, new);
    return l;
}

int comptePointsChaine(CellChaine *chaine)
{
    int cmt = 0;
    CellPoint *point_cour = chaine->points;
    while (point_cour)
    {
        cmt++;
        point_cour = point_cour->suiv;
    }
    return cmt;
}

int comptePointsTotal(Chaines *C)
{
    int cmt = 0;
    CellChaine *chaine_cour = C->chaines;
    while (chaine_cour)
    {
        cmt += comptePointsChaine(chaine_cour);
        chaine_cour = chaine_cour->suiv;
    }
    return cmt;
}

double distancePoint(CellPoint *a, CellPoint *b)
{
    double X = b->x - a->x;
    double Y = b->y - a->y;
    return sqrt(X * X + Y * Y);
}

double longueurChaine(CellChaine *c)
{
    double res = 0;
    CellPoint *courant = c->points;
    while (courant->suiv)
    {
        res += distancePoint(courant, courant->suiv);
        courant = courant->suiv;
    }
    return res;
}

double longueurTotale(Chaines *C)
{
    double res = 0;
    CellChaine *courant_chaine = C->chaines;
    while (courant_chaine)
    {
        res += longueurChaine(courant_chaine);
        courant_chaine = courant_chaine->suiv;
    }
    return res;
}

Chaines *lectureChaines(FILE *f)
{
    Chaines *res = malloc(sizeof(Chaines));
    if (!res)
    {
        printf("lectureChaine : erreur d'allocation de mémoire\n");
    }

    char buffer[1024];

    // Ajout du nb_chaine et gamma à res
    fgets(buffer, 1024, f);
    sscanf(buffer, "NbChain: %d", &(res->nbChaines));
    fgets(buffer, 1024, f);
    sscanf(buffer, "Gamma: %d", &(res->gamma));

    int nb_points;
    char l_points[1010];
    CellChaine *new = NULL;
    while (fgets(buffer, 1024, f))
    {
        // Création de Cellchaine
        new = malloc(sizeof(CellChaine));
        sscanf(buffer, "%d %d %[^\n]", (&new->numero), &nb_points, l_points);
        new->points = creer_liste_points(nb_points, l_points);
        // Ajout à la liste des chaînes
        res->chaines = insertion(res->chaines, new);
    }
    return res;
}

void ecrireChaines(Chaines *C, FILE *f)
{
    if (!C)
        return;

    fprintf(f, "NbChain: %d\nGamma: %d\n", C->nbChaines, C->gamma);
    CellChaine *chaine = C->chaines;
    CellPoint *point = NULL;
    int nb_points;
    while (chaine)
    {
        fprintf(f, "%d ", chaine->numero);
        // Compter le nb des points
        nb_points = comptePointsChaine(chaine);
        fprintf(f, "%d ", nb_points);
        // Ecrire des (x, y) des points
        point = chaine->points;
        while (point)
        {
            fprintf(f, "%.2f %.2f ", point->x, point->y);
            point = point->suiv;
        }
        // addition \n
        fputc(10, f);
        chaine = chaine->suiv;
    }
}

void afficheChainesSVG(Chaines *C, char *nomInstance)
{
    double max_x = 0, max_y = 0, min_x = 1e6, min_y = 1e6;
    CellChaine *chaine_cour;
    CellPoint *point_cour;
    double x_prec, y_prec;
    SVGwriter svg;
    chaine_cour = C->chaines;
    while (chaine_cour)
    {
        // Trouver les coordonnees max et min
        point_cour = chaine_cour->points;
        while (point_cour != NULL)
        {
            if (max_x < point_cour->x)
                max_x = point_cour->x;
            if (max_y < point_cour->y)
                max_y = point_cour->y;
            if (min_x > point_cour->x)
                min_x = point_cour->x;
            if (min_y > point_cour->y)
                min_y = point_cour->y;
            point_cour = point_cour->suiv;
        }
        chaine_cour = chaine_cour->suiv;
    }
    SVGinit(&svg, nomInstance, 500, 500);
    chaine_cour = C->chaines;
    while (chaine_cour)
    {
        point_cour = chaine_cour->points;
        SVGlineRandColor(&svg);
        // Dessiner le premier point
        SVGpoint(&svg, 500 * (point_cour->x - min_x) / (max_x - min_x), 500 * (point_cour->y - min_y) / (max_y - min_y));
        x_prec = point_cour->x;
        y_prec = point_cour->y;
        point_cour = point_cour->suiv;
        // Relier les points de la chaine
        while (point_cour)
        {
            SVGline(&svg, 500 * (x_prec - min_x) / (max_x - min_x), 500 * (y_prec - min_y) / (max_y - min_y), 500 * (point_cour->x - min_x) / (max_x - min_x), 500 * (point_cour->y - min_y) / (max_y - min_y));
            SVGpoint(&svg, 500 * (point_cour->x - min_x) / (max_x - min_x), 500 * (point_cour->y - min_y) / (max_y - min_y));
            x_prec = point_cour->x;
            y_prec = point_cour->y;
            point_cour = point_cour->suiv;
        }
        chaine_cour = chaine_cour->suiv;
    }
    SVGfinalize(&svg);
}

void liberer_liste_point(CellPoint *l)
{
    if (l->suiv)
        liberer_liste_point(l->suiv);
    free(l);
}

void liberer_chaines(Chaines **C)
{
    CellChaine *cell_cour = (*C)->chaines;
    CellChaine *tmp = NULL;
    while (cell_cour)
    {
        liberer_liste_point(cell_cour->points);
        tmp = cell_cour->suiv;
        free(cell_cour);
        cell_cour = tmp;
    }
    free(*C);
    *C = NULL;
}