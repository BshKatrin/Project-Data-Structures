#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "SVGLib/SVGwriter.h"
#include "Chaine.h"
#include "Reseau.h"
// #include "Hachage.h"
#define EPS 1e-3 // precision pour comparaison de l'egalite entre double

int nbCommodites(Reseau *R)
{
    if (!R)
        return 0;
    int cmt = 0;
    CellCommodite *noeud_cour = R->commodites;
    while (noeud_cour)
    {
        cmt++;
        noeud_cour = noeud_cour->suiv;
    }

    return cmt;
}

int nbLiaisons(Reseau *R)
{
    if (!R)
    {
        return 0;
    }
    int cmt = 0;
    CellNoeud *noeud_cour = R->noeuds;
    CellNoeud *voisin_cour = NULL;
    Noeud *noeud = NULL;

    while (noeud_cour)
    {
        noeud = noeud_cour->nd;
        voisin_cour = noeud->voisins;
        while (voisin_cour)
        {
            // Compter des liaisons sans repetitions, i.e. uniquement dans un sens
            if (noeud_cour->nd->num > voisin_cour->nd->num)
                cmt++;
            voisin_cour = voisin_cour->suiv;
        }
        noeud_cour = noeud_cour->suiv;
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
    // Ecrire des liaisons, eviter les repetitions
    noeud_cour = R->noeuds;

    CellNoeud *voisin = NULL;
    while (noeud_cour)
    {
        noeud = noeud_cour->nd;
        voisin = noeud->voisins;
        while (voisin)
        {
            // Ecrire uniquement dans un seul sens, pour eviter des repetitions
            if (noeud->num > voisin->nd->num)
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

// Retourne true ssi les noeuds sont egaux
bool noeuds_egaux(Noeud *n1, Noeud *n2)
{
    return (fabs(n1->x - n2->x) < EPS) && (fabs(n1->y - n2->y) < EPS);
}

bool noeuds_x_y_egaux(Noeud *n, double x, double y)
{
    return (fabs(n->x - x) < EPS) && (fabs(n->y - y) < EPS);
}

Noeud *creer_noeud(int num, double x, double y)
{
    Noeud *noeud_nouv = malloc(sizeof(Noeud));
    noeud_nouv->x = x;
    noeud_nouv->y = y;
    noeud_nouv->num = num;
    noeud_nouv->voisins = NULL;
    return noeud_nouv;
}

// Creer cellnoeud qui point vers noeud donne
CellNoeud *creer_cell_noeud(Noeud *nd)
{
    CellNoeud *cell_nouv = malloc(sizeof(CellNoeud));
    cell_nouv->nd = nd;
    cell_nouv->suiv = NULL;
    return cell_nouv;
}

void ajouter_noeud_reseau(Reseau *R, Noeud *nd)
{
    printf("Ajout dans reseau : %d %.2f %.2f\n", nd->num, nd->x, nd->y);

    CellNoeud *cell_nouv = creer_cell_noeud(nd);
    cell_nouv->suiv = R->noeuds;
    R->noeuds = cell_nouv;
    (R->nbNoeuds)++;
}

Noeud *rechercheCreeNoeudListe(Reseau *R, double x, double y)
{
    if (!R)
        return NULL;
    // Vérification de la présence du noeud
    CellNoeud *cell_cour = R->noeuds;
    while (cell_cour)
    {
        if ((fabs(cell_cour->nd->x - x) < EPS) && (fabs(cell_cour->nd->y - y) < EPS))
            return cell_cour->nd;
        cell_cour = cell_cour->suiv;
    }

    //   Ajout en cas d'absence, je crée le noeud
    Noeud *noeud_nouv = creer_noeud(R->nbNoeuds + 1, x, y);

    // et le rajoute à la liste des noeuds du réseau
    ajouter_noeud_reseau(R, noeud_nouv);
    // CellNoeud *cell_noeud_nouv = malloc(sizeof(CellNoeud));
    // cell_noeud_nouv->nd = noeud_nouv;
    // // Ajout en tete
    // cell_noeud_nouv->suiv = R->noeuds;
    // R->noeuds = cell_noeud_nouv;
    return noeud_nouv;
}

// Creer commodite, l'ajouter en tete des commodites du reseau
void ajouter_commodite(Reseau *R, Noeud *extrA, Noeud *extrB)
{
    CellCommodite *commodite = malloc(sizeof(CellCommodite));
    commodite->extrA = extrA;
    commodite->extrB = extrB;
    commodite->suiv = R->commodites;
    R->commodites = commodite;
}

void ajout_voisin(Noeud *noeud_cour, Noeud *noeud_voisin)
{
    // Creer nouvelle lien
    CellNoeud *nouv_lien = creer_cell_noeud(noeud_voisin);
    // Ajout en tete
    nouv_lien->suiv = noeud_cour->voisins;
    noeud_cour->voisins = nouv_lien;
}

CellNoeud *recherche_voisin(CellNoeud *liste_voisins, Noeud *n)
{
    if (!liste_voisins)
        return NULL;

    CellNoeud *cour = liste_voisins;
    while (cour && (!noeuds_egaux(cour->nd, n)))
        cour = cour->suiv;
    return cour;
}

Reseau *creer_reseau(int gamma)
{
    Reseau *res = malloc(sizeof(Reseau));
    res->nbNoeuds = 0;
    res->gamma = gamma;
    res->noeuds = NULL;
    res->commodites = NULL;
    return res;
}

Reseau *reconstitueReseauListe(Chaines *C)
{
    // Création du réseau
    Reseau *res = creer_reseau(C->gamma);

    // Parcours des chaines
    CellChaine *chaine_cour = C->chaines;
    CellPoint *point_cour = NULL;

    // Pour voisins
    Noeud *noeud_prec = NULL;
    Noeud *noeud_trouve = NULL;
    Noeud *noeud_voisin = NULL;

    // Pour commodites
    Noeud *extrA = NULL;
    Noeud *extrB = NULL;

    while (chaine_cour)
    {
        point_cour = chaine_cour->points;
        while (point_cour)
        {
            noeud_trouve = rechercheCreeNoeudListe(res, point_cour->x, point_cour->y);

            // MAJ des voisins
            // Point precedant, s'il existe
            if (noeud_prec)
            {
                // MAJ une liste des voisins du noeud retrouve
                if (recherche_voisin(noeud_trouve->voisins, noeud_prec) == NULL)
                    ajout_voisin(noeud_trouve, noeud_prec);
                // MAJ une liste des voisins d'un voisin du noeud retrouve
                // Ajouter nouveau noeud dans une liste de ces voisins
                if (recherche_voisin(noeud_prec->voisins, noeud_trouve) == NULL)
                    ajout_voisin(noeud_prec, noeud_trouve);
            }
            // Point suivant, s'il existe
            if (point_cour->suiv)
            {
                noeud_voisin = rechercheCreeNoeudListe(res, point_cour->suiv->x, point_cour->suiv->y);
                // MAJ une liste des voisins du noeud retrouve
                if (recherche_voisin(noeud_trouve->voisins, noeud_voisin) == NULL)
                    ajout_voisin(noeud_trouve, noeud_voisin);
                // MAJ une liste des voisins d'un voisin du noeud retrouve
                // Ajouter nouveau noeud dans une liste de ces voisins
                if (recherche_voisin(noeud_voisin->voisins, noeud_trouve) == NULL)
                    ajout_voisin(noeud_voisin, noeud_trouve);
            }

            // Definir le premier point comme extrA
            if (extrA == NULL)
                extrA = noeud_trouve;

            noeud_prec = noeud_trouve;
            point_cour = point_cour->suiv;
        }
        //  Le noeud correpond au dernier point
        extrB = noeud_trouve;
        ajouter_commodite(res, extrA, extrB);

        chaine_cour = chaine_cour->suiv;
        extrA = NULL;
        noeud_prec = NULL;
        noeud_voisin = NULL;
    }
    return res;
}

void liberer_reseau(Reseau **R)
{
    // Liberer noeuds
    CellNoeud *noeud_cour = (*R)->noeuds;
    CellNoeud *voisin_cour = NULL;
    CellNoeud *tmp_cell = NULL;
    while (noeud_cour)
    {
        voisin_cour = noeud_cour->nd->voisins;
        // Pour chaque noeud liberer ces voisins
        while (voisin_cour)
        {
            tmp_cell = voisin_cour->suiv;
            free(voisin_cour);
            voisin_cour = tmp_cell;
        }
        free(noeud_cour->nd);

        tmp_cell = noeud_cour->suiv;
        free(noeud_cour);
        noeud_cour = tmp_cell;
    }
    // liberer commdodites
    CellCommodite *commodite_cour = (*R)->commodites;
    CellCommodite *tmp_commodite = NULL;

    while (commodite_cour)
    {
        tmp_commodite = commodite_cour->suiv;
        free(commodite_cour);
        commodite_cour = tmp_commodite;
    }

    free(*R);
    *R = NULL;
}