#include <stdio.h>
#include <stdlib.h>

#include "SVGLib/SVGwriter.h"
#include "Chaine.h"

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        printf("Nom du fichier requis. %s <nom-fichier-lecture> <nom-fichier-ecriture> <nom-fichier-html>\n", argv[0]);
        return 1;
    }
    FILE *f_lecture = fopen(argv[1], "r");
    FILE *f_ecriture = fopen(argv[2], "w");

    if (!f_lecture || !f_ecriture)
    {
        printf("Erreur d'ouverture des fichiers\n");
        return 2;
    }

    Chaines *chaine = lectureChaines(f_lecture);
    ecrireChaines(chaine, f_ecriture);
    afficheChainesSVG(chaine, argv[3]);

    fclose(f_lecture);
    fclose(f_ecriture);
    return 0;
}