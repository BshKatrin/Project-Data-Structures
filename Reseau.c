#include "Reseau.h"
#include <stdlib.h>
#include <stdio.h>
#include "Chaine.h"
#include <math.h>
#include "SVGLib/SVGwriter.h"

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

void ajout_commodite(CellCommodite *courant, CellCommodite *new){
    new->suiv = courant;
    courant=new;
}

void ajout_voisins(Noeud *nd, Noeud * new_voisin){
    CellNoeud * copie = malloc(sizeof(CellNoeud));
    copie->nd=new_voisin;
    copie->suiv=nd->voisins;
    nd->voisins=copie;
}



Reseau* reconstitueReseauListe(Chaines *C){
    //Création du réseau
    Reseau * res = (Reseau *) malloc(sizeof(Reseau));
    res->nbNoeuds=0;
    res->gamma=C->gamma;
    res->noeuds=NULL;
    res->commodites=NULL;

    // Ajout des points et commodités
        CellCommodite * new_commodite = (CellCommodite *)malloc(sizeof(CellCommodite));
        CellChaine *ch_courante = C->chaines;
        for(int i = 0 ; i<C->nbChaines ; i++ && ch_courante->suiv ){
        // Je parcours les chaines

            CellPoint *courant = ch_courante->points;
            new_commodite->extrA=rechercheCreeNoeudListe(res,courant->x,courant->y);

            while(courant){
                // Je recherche ou crée mon noeud associé 
                Noeud *nd = rechercheCreeNoeudListe(res,courant->x,courant->y);

                // La cellule suivante est sa voisine : je la rajoute à la liste des voisins
                if(courant->suiv){
                    Noeud * nd_suiv = rechercheCreeNoeudListe(res,courant->suiv->x,courant->suiv->y);
                    ajout_voisins(nd,nd_suiv);
                }

                // Si elle est dernière, je la note comme extrB de la commodité 
                else new_commodite->extrB = nd;

                courant = courant->suiv;
            } 
            // J'ajoute ma commodité à la liste des comodités existantes
            ajout_commodite(res->commodites,new_commodite);
        }
   return res;
}

int main(){
    FILE * f = fopen("00014_burma.cha","r");
    Chaines *test = lectureChaines(f);
    fclose(f);
    Reseau *R =reconstitueReseauListe(test);
    return 0;
}