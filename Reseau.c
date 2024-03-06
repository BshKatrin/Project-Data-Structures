#include "Reseau.h"
#include <stdlib.h>

Noeud* rechercheCreeNoeudListe(Reseau *R, double x, double y){
    if(!R) return NULL;
    // Vérification de la présence du noeud
    CellNoeud * courant = R->noeuds;
    while(courant){
        if(courant->nd->x == x && courant ->nd->y == y) return courant->nd;
        courant = courant->suiv;
    }

    // Ajout en cas d'absence, je crée le noeud
    Noeud *res = malloc(sizeof(Noeud));
    res->x=x;
    res->y=y;
    res->num=R->nbNoeuds+1;
    res->voisins=NULL;
    R->nbNoeuds++;

    // et le rajoute à la liste des noeuds du réseau
    CellNoeud* cell_res = malloc(sizeof(CellNoeud));
    cell_res->nd=res;
    cell_res->suiv=R->noeuds;
    R->noeuds=cell_res;
    return res;
}

CellNoeud * trouver_cellule(Reseau *r, Noeud *nd){
    if(!r) return NULL;
    CellNoeud * courant = r->noeuds;
    while(courant){
        if(courant->nd == nd) return courant;
    }
    return NULL;
}

void ajout_commodite_fin(CellCommodite *courant, CellCommodite *new){
    if(courant) new->suiv = courant;
    courant=new;
}

void ajout_voisins(Noeud *nd, Noeud * new_voisin){
    CellNoeud * copie = malloc(sizeof(CellNoeud));
    copie->nd=new_voisin;
    copie->suiv=nd->voisins;
    nd->voisins=copie;
}



Reseau* reconstitueReseauListe(Chaines *C){
    // Création du réseau
    Reseau * res = malloc(sizeof(Reseau));
    res->nbNoeuds=0;
    res->noeuds=NULL;
    res->gamma=C->gamma;
    res->commodites=NULL;

    // Ajout des points et commodités
    for(int i = 0 ; i<C->nbChaines ; i++){
        CellCommodite * new_commodite = malloc(sizeof(CellCommodite));
        CellPoint *courant = C->chaines[i].points;
        new_commodite->extrA=rechercheCreeNoeudListe(res,courant->x,courant->y);

        while(courant){
            // Je recherche ou crée mon noeud associé 
            Noeud *nd = rechercheCreeNoeudListe(res,courant->x,courant->y);
            CellNoeud * cell_n = trouver_cellule(res,nd);

            // La cellule suivante est sa voisine : je la rajoute à la liste des voisins
            if(courant->suiv){
                Noeud * nd_suiv = rechercheCreeNoeudListe(res,courant->x,courant->y);
                ajout_voisins(nd,nd_suiv);
            }

            // Si je suis dernière, je me note comme extrB de la commodité 
            else new_commodite->extrB = nd;

            courant = courant->suiv;
        } 
        // J'ajoute ma commodité à la liste des comodités existantes
        ajout_commodite(res->commodites,new_commodite);
    }
    return res;
}