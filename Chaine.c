#include <stdio.h>
#include <stdlib.h>

#include "Chaine.h"

CellPoint *creer_point(double x, double y)
{
    CellPoint *new = malloc(sizeof(CellPoint));
    new->suiv = NULL;
    new->x = x;
    new->y = y;
    return new;
}

CellPoint *liste_points(int nb_points, char *l)
{
    if (!nb_points)
        return NULL;
    double x, y;
    char *reste;
    x = strtod(l, &reste);
    y = strtod(reste, &reste);
    CellPoint *new = creer_point(x, y);
    new->suiv = liste_points(nb_points - 1, reste);
    return new;
}

// Insère en fin de liste chaînée
CellChaine *insertion(CellChaine *l, CellChaine *new)
{
    if (!l)
        return new;
    else
        l->suiv = insertion(l->suiv, new);
    return l;
}

void liberer_liste_point(CellPoint *l)
{
    if (l->suiv)
        liberer_liste_point(l->suiv);
    free(l);
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
        new->points = liste_points(nb_points, l_points);
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
        nb_points = 0;
        point = chaine->points;
        while (point)
        {
            nb_points++;
            point = point->suiv;
        }
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

// int main()
// {
//     char *nom_fic = "00014_burma.cha";
//     FILE *f = fopen(nom_fic, "r");
//     FILE *f_tmp = fopen("tmp.txt", "w");

//     Chaines *test = lectureChaine(f);
//     ecrireChaines(test, f_tmp);
//     CellPoint *courant = test->chaines->points;

//     fclose(f_tmp);
//     fclose(f);
//     return 0;
// }