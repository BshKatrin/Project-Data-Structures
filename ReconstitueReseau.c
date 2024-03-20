#include <stdio.h>
#include <stdlib.h>

#include "Chaine.h"
#include "Reseau.h"
#include "Hachage.h"
#include "ArbreQuat.h"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Nom du fichier, type de structures requis. %s <fichier-lecture.cha> <fichier-ecriture.res> <type-structure>\n", argv[0]);
        printf("Types des structures possibles :\n1 : Liste chainee\n2 : Table de hachage\n3:Arbre\n");
        return 1;
    }
    // 1 : Liste, 2 : Tab de hachage, 3 : Arbre
    int type = atoi(argv[3]);

    FILE *f_lecture = fopen(argv[1], "r");
    FILE *f_ecriture = fopen(argv[2], "w");
    if (f_ecriture == NULL || f_lecture == NULL)
    {
        printf("Erreur d'ouverture des fichiers\n");
        return 2;
    }

    Chaines *C = lectureChaines(f_lecture);
    Reseau *R = NULL;

    switch (type)
    {
    case 1:
        R = reconstitueReseauListe(C);
        break;
    case 2:
        R = reconstitueReseauHachage(C, 15);
        break;
    case 3:
        R = reconstitueReseauArbre(C);
        break;
    default:
        printf("Tel choix n'est pas possible\n");
        break;
    }

    ecrireReseau(R, f_ecriture);
    afficheReseauSVG(R, "reseau");
    if (R)
        liberer_reseau(&R);
    liberer_chaines(&C);
    fclose(f_lecture);
    fclose(f_ecriture);
    return 0;
}