#include <stdio.h>
#include <stdlib.h>

#include "Chaine.h"

int main(int argv, char **argc)
{
    if (argv < 3)
    {
        printf("Nom du fichier requis. %s <nom-fichier-lecture> <nom-fichier-ecriture>\n", argc[0]);
        return 1;
    }
    FILE *f_lecture = fopen(argc[1], "r");
    FILE *f_ecriture = fopen(argc[2], "w");
    if (!f_lecture || !f_ecriture)
    {
        printf("Erreur d'ouverture des fichiers\n");
        return 2;
    }

    Chaines *chaine = lectureChaines(f_lecture);
    ecrireChaines(chaine, f_ecriture);
    fclose(f_lecture);
    fclose(f_ecriture);
    return 0;
}