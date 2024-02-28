#include "Chaine.h"
#include <stdio.h>
#include <stdlib.h>

CellPoint *creerpoint(double x, double y)
{
    CellPoint *new = malloc(sizeof(CellPoint));
    new->suiv = NULL;
    new->x = x;
    new->y = y;
    return new;
}

CellPoint *liste_points(int len, char *l)
{
    if (!len)
        return NULL;
    double x, y;
    x = strtod(l, &l);
    y = strtod(l, &l);
    CellPoint *new = creerpoint(x, y);
    new->suiv = liste_points(len - 1, l);
    return new;
}

CellChaine *insertion(CellChaine *l, CellChaine *new)
{
    // Insère en fin de liste chaînée
    if (!l)
        return new;
    else
        l->suiv = insertion(l->suiv, new);
    return l;
}

Chaines *lectureChaine(FILE *f)
{
    Chaines *res = malloc(sizeof(Chaines));
    char buffer[1024];

    // Ajout du nb_chaine et gamma à res
    fgets(buffer, 1024, f);
    sscanf(buffer, "NbChain: %d", &(res->nbChaines));
    fgets(buffer, 1024, f);
    sscanf(buffer, "Gamma: %d", &(res->gamma));

    while (fgets(buffer, 1024, f))
    {
        char l_points[1024];
        int nb_points;
        // Création de Cellchaine
        CellChaine *new = malloc(sizeof(CellChaine));
        sscanf(buffer, "%d %d %s", (&new->numero), &nb_points, l_points);
        new->points = liste_points(nb_points, l_points);

        // Ajout à la liste des chaînes
        res->chaines = insertion(res->chaines, new);
    }
    return res;
}

void liberer_liste_point(CellPoint *l)
{
    if (l->suiv)
        liberer_liste_point(l->suiv);
    free(l);
}

void ecrireChaines(Chaines *C, FILE *f)
{
    if (f == NULL)
    {
        printf("Erreur d'ecriture d'un fichier\n");
        return;
    }
    if (C == NULL)
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

int main()
{
    char *nom_fic = "00014_burma.cha";
    FILE *f = fopen(nom_fic, "r");
    char *nom_tmp = "tmp.cha";
    FILE *tmp = fopen(nom_tmp, "w");
    Chaines *test = lectureChaine(f);
    ecrireChaines(test, tmp);
    fclose(f);
    fclose(tmp);
    return 0;
}