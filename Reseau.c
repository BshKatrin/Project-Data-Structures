#include <stdlib.h>
#include <stdio.h>

#include "SVGLib/SVGwriter.h"

#include "Reseau.h"

int nbCommodites(Reseau *R)
{
    if (!R)
        return 0;
    int cmt = 0;
    CellCommodite *noeud_cour = R->commodites;
    while (noeud_cour)
        cmt++;
    return cmt;
}

int nbLiaisons(Reseau *R)
{
    if (!R)
    {
        return 0;
    }
    int cmt = 0;
    CellNoeud *cell_cour = R->noeuds;
    CellNoeud *voisin_cour = NULL;
    Noeud *noeud = NULL;

    while (cell_cour)
    {
        noeud = cell_cour->nd;
        voisin_cour = noeud->voisins;
        while (voisin_cour)
        {
            cmt++;
            voisin_cour = voisin_cour->suiv;
        }
        cell_cour = cell_cour->suiv;
    }
    return cmt;
}

void ecrireReseau(Reseau *R, FILE *f)
{
    if (!R)
        return;

    fprintf(f, "NbNoeuds: %d\n", R->nbNoeuds);
    fprintf(f, "NbLiaisons: %d\n", nbLiaisons(R));
    fprintf(f, "NbCommodites: %d\n", nbCommodites(R));
    fprintf(f, "Gamma: %d\n", R->gamma);

    // Ajouter newline
    fputc(10, f);

    CellNoeud *noeud_cour = R->noeuds;
    Noeud *noeud = NULL;
    // Ecrire les points
    while (noeud_cour)
    {
        noeud = noeud_cour->nd;
        fprintf(f, "v %d %.6f %.6f\n", noeud->num, noeud->x, noeud->y);
        noeud_cour = noeud_cour->suiv;
    }
    fputc(10, f);

    // Ecrire des liaisons
    noeud_cour = R->noeuds;
    CellNoeud *voisin = NULL;
    while (noeud_cour)
    {
        noeud = noeud_cour->nd;
        voisin = noeud->voisins;
        while (voisin)
        {
            fprintf(f, "l %d %d\n", voisin->nd->num, noeud->num);
            voisin = voisin->suiv;
        }

        noeud_cour = noeud_cour->suiv;
    }
    fputc(10, f);

    // Ecrire des commodites
    CellCommodite *commodite = R->commodites;
    while (commodite)
    {
        fprintf(f, "k %d %d\n", commodite->extrA->num, commodite->extrB->num);
        commodite = commodite->suiv;
    }
}

void afficheReseauSVG(Reseau *R, char *nomInstance)
{
    CellNoeud *noeud_cour, *voisin;
    SVGwriter svg;
    double max_x = 0, max_y = 0, min_x = 1e6, min_y = 1e6;

    noeud_cour = R->noeuds;
    while (noeud_cour != NULL)
    {
        if (max_x < noeud_cour->nd->x)
            max_x = noeud_cour->nd->x;
        if (max_y < noeud_cour->nd->y)
            max_y = noeud_cour->nd->y;
        if (min_x > noeud_cour->nd->x)
            min_x = noeud_cour->nd->x;
        if (min_y > noeud_cour->nd->y)
            min_y = noeud_cour->nd->y;
        noeud_cour = noeud_cour->suiv;
    }
    SVGinit(&svg, nomInstance, 500, 500);
    noeud_cour = R->noeuds;
    while (noeud_cour != NULL)
    {
        SVGpoint(&svg, 500 * (noeud_cour->nd->x - min_x) / (max_x - min_x), 500 * (noeud_cour->nd->y - min_y) / (max_y - min_y));
        voisin = noeud_cour->nd->voisins;
        while (voisin != NULL)
        {
            if (voisin->nd->num < noeud_cour->nd->num)
                SVGline(&svg, 500 * (voisin->nd->x - min_x) / (max_x - min_x), 500 * (voisin->nd->y - min_y) / (max_y - min_y), 500 * (noeud_cour->nd->x - min_x) / (max_x - min_x), 500 * (noeud_cour->nd->y - min_y) / (max_y - min_y));
            voisin = voisin->suiv;
        }
        noeud_cour = noeud_cour->suiv;
    }
    SVGfinalize(&svg);
}
