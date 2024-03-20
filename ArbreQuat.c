#include <stdlib.h>
#include <float.h>

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
        // Centre est redéfinie après
        *a = creerArbreQuat(0, 0, n->x, n->y);
        (*a)->noeud = n;
        (*a)->coteX = parent->coteX / 2;
        (*a)->coteY = parent->coteY / 2;
        // Définir les nouvelles dimensions selon la direction
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
        if (n->x < parent->xc && n->y >= parent->yc)
        {
            (*a)->xc = parent->xc - parent->coteX / 4;
            (*a)->yc = parent->yc + parent->coteY / 4;
        }
        // Nord-est
        if (n->x >= parent->xc && n->y >= parent->yc)
        {
            (*a)->xc = parent->xc + parent->coteX / 4;
            (*a)->yc = parent->yc + parent->coteY / 4;
        }
        return;
    }
    // Feuille
    if ((*a)->noeud != NULL)
    {
        // Marquer cellule comme le noeud interne
        Noeud *tmp = (*a)->noeud;
        (*a)->noeud = NULL;

        insererNoeudArbre(tmp, a, parent);
        insererNoeudArbre(n, a, parent);
    }
    // Cellule interne
    else
    {
        // Sud-ouest
        if (n->x < (*a)->xc && n->y < (*a)->yc)
            insererNoeudArbre(n, &((*a)->so), *a);
        // Sud-est
        if (n->x >= (*a)->xc && n->y < (*a)->yc)
            insererNoeudArbre(n, &((*a)->se), *a);
        // Nord-ouest
        if (n->x < (*a)->xc && n->y >= (*a)->yc)
            insererNoeudArbre(n, &((*a)->no), *a);
        // Nord-est
        if (n->x >= (*a)->xc && n->y >= (*a)->yc)
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
        insererNoeudArbre(noeud_nouv, a, parent);
        ajouter_noeud_reseau(R, noeud_nouv);

        return noeud_nouv;
    }
    // Feuille
    if ((*a)->noeud != NULL)
    {
        // Noeud avec (x, y) est présent et trouvé
        if (noeuds_x_y_egaux((*a)->noeud, x, y))
        {
            return (*a)->noeud;
        }
        Noeud *noeud_nouv = creer_noeud(R->nbNoeuds + 1, x, y);
        insererNoeudArbre(noeud_nouv, a, parent);
        ajouter_noeud_reseau(R, noeud_nouv);
        return noeud_nouv;
    }

    // Cellule interne, recherche dans l'une des cellules
    // Sud-ouest
    if ((x < (*a)->xc) && (y < (*a)->yc))
        return rechercheCreeNoeudArbre(R, &((*a)->so), *a, x, y);
    // Sud-est
    if ((x >= (*a)->xc) && (y < (*a)->yc))
        return rechercheCreeNoeudArbre(R, &((*a)->se), *a, x, y);
    // Nord-ouest
    if ((x < (*a)->xc) && (y >= (*a)->yc))
        return rechercheCreeNoeudArbre(R, &((*a)->no), *a, x, y);
    // Nord-est
    if ((x >= (*a)->xc) && (y >= (*a)->yc))
        return rechercheCreeNoeudArbre(R, &((*a)->ne), *a, x, y);
    return NULL;
}

Reseau *reconstitueReseauArbre(Chaines *C)
{
    // Creer reseau
    Reseau *res = creer_reseau(C->gamma);

    // Creer arbre quaternaire
    double xmin = DBL_MAX, ymin = DBL_MAX, xmax = DBL_MIN, ymax = DBL_MIN;
    chaineCoordMinMax(C, &xmin, &ymin, &xmax, &ymax);
    double xc = (xmin + xmax) / 2; // centre (axe X)
    double yc = (ymin + ymax) / 2; // centre (axe Y)

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
                noeud_voisin = rechercheCreeNoeudArbre(res, &AQ, AQ, point_cour->suiv->x, point_cour->suiv->y);
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
    afficher_arbre(AQ, 0);
    liberer_arbre(AQ);
    return res;
}

void liberer_arbre(ArbreQuat *A)
{
    if (A == NULL)
        return;
    liberer_arbre(A->no);
    liberer_arbre(A->ne);
    liberer_arbre(A->se);
    liberer_arbre(A->so);

    free(A);
}

void afficher_arbre(ArbreQuat *A, int level)
{
    if (A == NULL)
    {
        printf("null\n");
        return;
    }

    if (A->noeud == NULL)
    {
        printf("Cell interne. center : %.2f, %.2f\n", A->xc, A->yc);
        printf("SE:\n");
        afficher_arbre(A->se, level + 1);
        printf("SO:\n");
        afficher_arbre(A->so, level + 1);
        printf("NE:\n");
        afficher_arbre(A->ne, level + 1);
        printf("NO:\n");
        afficher_arbre(A->no, level + 1);
    }
    else
    {
        printf("Feuille : ");
        printf("level %d,  %d %.2f %2.f\n", level, A->noeud->num, A->noeud->x, A->noeud->y);
    }
}