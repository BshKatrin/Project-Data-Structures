#include <stdlib.h>
#include <math.h>

#include "Hachage.h"
#include "Reseau.h"

double clef(double x, double y)
{
    return y + (x + y) * (x + y + 1) / 2;
}

int hash(double clef, int tab_taille)
{
    double A = (sqrt(5) - 1) / 2;
    double part_fraction = clef * A - (int)(clef * A);
    return (int)(tab_taille * part_fraction);
}

TableHachage *creer_tab(unsigned int tailleMax)
{
    TableHachage *H = malloc(sizeof(TableHachage));
    if (!H)
    {
        printf("Erreur d'allocation de mémoire\n");
        return NULL;
    }
    H->nbElement = 0;
    H->tailleMax = tailleMax;
    H->T = malloc(sizeof(CellNoeud *) * H->tailleMax);

    for (int i = 0; i < H->tailleMax; i++)
        H->T[i] = NULL;
    return H;
}

void inserer(TableHachage *H, Noeud *nd)
{
    CellNoeud *cell = creer_cell_noeud(nd);
    int index = hash(clef(nd->x, nd->y), H->tailleMax);
    // Ajouter en tete de la liste chainee
    cell->suiv = H->T[index];
    H->T[index] = cell;
    (H->nbElement)++
}

void liberer_tab(TableHachage *H)
{
    CellNoeud *cell_cour = NULL;
    CellNoeud *tmp = NULL;
    for (int i = 0; i < H->tailleMax; i++)
    {
        cell_cour = H->T[i];
        while (cell_cour)
        {
            tmp = cell_cour->suiv;
            free(cell_cour);
            cell_cour = tmp;
        }
    }
    free(H->T);
    free(H);
}

Noeud *rechercheCreeNoeudHachage(Reseau *R, TableHachage *H, double x, double y)
{
    if (!R)
        return NULL;

    int index = hash(clef(x, y), H->tailleMax);
    CellNoeud *cell_cour = H->T[index];
    while (cell_cour && !(noeuds_x_y_egaux(cell_cour->nd, x, y)))
    {
        cell_cour = cell_cour->suiv;
    }
    // Cas 1 : point est trouve
    if (cell_cour)
        return cell_cour->nd;

    // Cas 2 : point n'est pas trouve
    Noeud *noeud_nouv = creer_noeud(R->nbNoeuds + 1, x, y);
    // printf("Creation %d %.2f %.2f\n", R->nbNoeuds + 1, x, y);
    //  Ajouter dans le reseau
    ajouter_noeud_reseau(R, noeud_nouv);
    // Ajouter dans la table de hachage
    inserer(H, noeud_nouv);
    return noeud_nouv;
}

Reseau *reconstitueReseauHachage(Chaines *C, int M)
{
    // Init de table de hachage
    TableHachage *H = creer_tab(M);

    // Création du réseau
    Reseau *res = malloc(sizeof(Reseau));
    res->nbNoeuds = 0;
    res->gamma = C->gamma;
    res->noeuds = NULL;
    res->commodites = NULL;

    // Parcours des chaines
    CellChaine *chaine_cour = C->chaines;
    CellPoint *point_cour = NULL;

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
            noeud_trouve = rechercheCreeNoeudHachage(res, H, point_cour->x, point_cour->y);

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
                noeud_voisin = rechercheCreeNoeudHachage(res, H, point_cour->suiv->x, point_cour->suiv->y);
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

        extrA = NULL;
        noeud_prec = NULL;
        noeud_voisin = NULL;

        chaine_cour = chaine_cour->suiv;
    }
    liberer_tab(H);
    // Desallouer la memoire occupee par une table de hachage

    return res;
}