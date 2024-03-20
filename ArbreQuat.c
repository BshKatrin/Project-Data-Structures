#include <stdlib.h>

#include "ArbreQuat.h"
#include "Reseau.h"

void chaineCoordMinMax(Chaines *C, double *xmin, double *ymin, double *xmax, double *ymax)
{
    if (!C)
        return;
    CellChaine *chaine = C->chaines;
    CellPoint *point = NULL;
    while (chaine)
    {
        point = chaine->points;
        while (point)
        {
            (*xmin) = (point->x < (*xmin)) ? point->x : (*xmin);
            (*ymin) = (point->y < (*ymin)) ? point->y : (*ymin);

            (*xmax) = (point->x > (*xmax)) ? point->x : (*xmax);
            (*ymax) = (point->y > (*ymax)) ? point->y : (*ymax);

            point = point->suiv;
        }
        chaine = chaine->suiv;
    }
}

ArbreQuat *creerArbreQuat(double xc, double yc, double coteX, double coteY)
{
    ArbreQuat *AQ = malloc(sizeof(ArbreQuat));
    AQ->xc = xc;
    AQ->yc = yc;
    AQ->coteX = coteX;
    AQ->coteY = coteY;
    AQ->noeud = NULL;
    AQ->no = NULL;
    AQ->ne = NULL;
    AQ->so = NULL;
    AQ->se = NULL;
    return AQ;
}

void insererNoeudArbre(Noeud *n, ArbreQuat **a, ArbreQuat *parent)
{
    // Arbre vide
    if ((*a) == NULL)
    {
        *a = creerArbreQuat(n->x, n->y, n->x, n->y);
        (*a)->noeud = n;
        (*a)->coteX = parent->coteX / 2;
        (*a)->coteY = parent->coteY / 2;
        // Tester le direction ou il faut l'inserer
        // Sud-ouest
        if (n->x < parent->xc && n->y < parent->yc)
        {
            (*a)->xc = parent->xc - parent->coteX / 4;
            (*a)->yc = parent->yc - parent->coteY / 4;
        }
        // Sud-est
        if (n->x >= parent->xc && n->y < parent->yc)
        {
            (*a)->xc = parent->xc + parent->coteX / 4;
            (*a)->yc = parent->yc - parent->coteY / 4;
        }
        // Nord-ouest
        if (n->x < parent->xc && n->y > parent->yc)
        {
            (*a)->xc = parent->xc - parent->coteX / 4;
            (*a)->yc = parent->yc + parent->coteY / 4;
        }
        // Nord-est
        if (n->x <= parent->xc && n->y < parent->yc)
        {
            (*a)->xc = parent->xc + parent->coteX / 4;
            (*a)->yc = parent->yc + parent->coteY / 4;
        }
        return;
    }
    // Feuille
    if ((*a)->noeud != NULL)
    {
        insererNoeudArbre(n, NULL, *a);
        insererNoeudArbre((*a)->noeud, NULL, *a);
    }
    // Cellule interne
    else
    {
        // Sud-ouest
        if (n->x < parent->xc && n->y < parent->yc)
            insererNoeudArbre(n, &((*a)->so), *a);
        // Sud-est
        if (n->x >= parent->xc && n->y < parent->yc)
            insererNoeudArbre(n, &((*a)->se), *a);
        // Nord-ouest
        if (n->x < parent->xc && n->y > parent->yc)
            insererNoeudArbre(n, &((*a)->no), *a);
        // Nord-est
        if (n->x <= parent->xc && n->y < parent->yc)
            insererNoeudArbre(n, &((*a)->ne), *a);
    }
}

Noeud *rechercheCreeNoeudArbre(Reseau *R, ArbreQuat **a, ArbreQuat *parent, double x, double y)
{
    if (!R)
        return NULL;
    // Arbre vide
    if (*a == NULL)
    {
        Noeud *noeud_nouv = creer_noeud(R->nbNoeuds + 1, x, y);
        ajouter_noeud_reseau(R, noeud_nouv);
        insererNoeudArbre(noeud_nouv, a, parent);
        return noeud_nouv;
    }
    // Feuille
    if ((*a)->noeud != NULL)
    {
        // Noeud avec (x, y) est présent et trouvé
        if (noeuds_x_y_egaux((*a)->noeud, x, y))
            return (*a)->noeud;
        // Sinon, le creer et l'ajouter dans l'arbre et dans le reseau
        Noeud *noeud_nouv = creer_noeud(R->nbNoeuds + 1, x, y);
        ajouter_noeud_reseau(R, noeud_nouv);
        insererNoeudArbre(noeud_nouv, a, parent);
        return noeud_nouv;
    }

    // Cellule interne, recherche dans l'une des cellules
    Noeud *cour = (*a)->noeud;
    // Sud-ouest
    if (x < cour->x && y < cour->y)
        return rechercheCreeNoeudArbre(R, &((*a)->se), *a, x, y);
    // Sud-est
    if (x >= cour->x && y < cour->y)
        return rechercheCreeNoeudArbre(R, &((*a)->se), *a, x, y);
    // Nord-ouest
    if (x < cour->x && y > cour->y)
        return rechercheCreeNoeudArbre(R, &((*a)->se), *a, x, y);
    // Nord-est
    if (x <= cour->x && y < cour->y)
        return rechercheCreeNoeudArbre(R, &((*a)->se), *a, x, y);
    return NULL;
}

Reseau *reconstitueReseauArbre(Chaines *C)
{
    // Creer reseau
    Reseau *res = creer_reseau(C->gamma);

    // Creer arbre quaternaire
    double xmin, ymin, xmax, ymax;
    chaineCoordMinMax(C, &xmin, &ymin, &xmax, &ymax);
    double xc = (xmin + xmax) / 2; // centre
    double yc = (ymin + ymax) / 2; // centre
    ArbreQuat *AQ = creerArbreQuat(xc, yc, xmax - xmin, ymax - ymin);

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
            noeud_trouve = rechercheCreeNoeudArbre(res, &AQ, AQ, point_cour->x, point_cour->y);
            // MAJ des voisins
            if (noeud_prec)
            {
                if (recherche_voisin(noeud_trouve->voisins, noeud_prec) == NULL)
                    ajout_voisin(noeud_trouve, noeud_prec);

                if (recherche_voisin(noeud_prec->voisins, noeud_trouve) == NULL)
                    ajout_voisin(noeud_prec, noeud_trouve);
            }
            if (point_cour->suiv)
            {
                noeud_voisin = rechercheCreeNoeudListe(res, point_cour->suiv->x, point_cour->suiv->y);
                if (recherche_voisin(noeud_trouve->voisins, noeud_voisin) == NULL)
                    ajout_voisin(noeud_trouve, noeud_voisin);

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
    liberer_arbre(AQ);
    return res;
}

void liberer_arbre(ArbreQuat *A)
{
    return;
}