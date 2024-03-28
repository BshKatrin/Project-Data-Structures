#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "Chaine.h"
#include "Hachage.h"
#include "ArbreQuat.h"

double calculer_temps_cpu(clock_t temps_init, clock_t temps_fin)
{
    return ((double)(temps_fin - temps_init)) / CLOCKS_PER_SEC;
}

void tester_instance(char *nom_instance, FILE *f_ecriture)
{
    FILE *f = fopen(nom_instance, "r");
    if (!f)
    {
        printf("Erreur lors de l'ouverture d'une instance \"%s\"\n", nom_instance);
        return;
    }
    // Tester et ecrire les resultats des tests
    Chaines *C = lectureChaines(f);
    Reseau *R = NULL;

    clock_t temps_init, temps_fin;
    double temps_liste, temps_hash, temps_arbre;
    int taille_tab_hash;

    // (1) Tester listes
    temps_init = clock();
    R = reconstitueReseauListe(C);
    temps_fin = clock();
    temps_liste = calculer_temps_cpu(temps_init, temps_fin);

    liberer_reseau(&R);

    // (2) Tester table de hachage
    // Facteur de charge ~ 80%
    taille_tab_hash = comptePointsTotal(C) / 0.8;

    temps_init = clock();
    R = reconstitueReseauHachage(C, taille_tab_hash);
    temps_fin = clock();
    temps_hash = calculer_temps_cpu(temps_init, temps_fin);
    liberer_reseau(&R);

    printf("Taille hash : %d\n", taille_tab_hash);
    // (3) Tester arbre
    temps_init = clock();
    R = reconstitueReseauArbre(C);
    temps_fin = clock();
    temps_arbre = calculer_temps_cpu(temps_init, temps_fin);
    liberer_reseau(&R);

    fprintf(f_ecriture, "%s %.6f %.6f %.6f\n", nom_instance, temps_liste, temps_hash, temps_arbre);
    fclose(f);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Nom du fichier requis. %s <file1>.cha <file2>.cha ...\n", argv[0]);
        return 1;
    }

    FILE *f_ecriture = fopen("graphs/instances_analyse.txt", "w");
    fprintf(f_ecriture, "Instance Liste Hash Arbre\n");

    // Parcourir chaque instance donnée
    // Ecrire des résultats dans le fichier graphs/instance_analyse.txt
    for (int i = 1; i < argc; i++)
    {
        tester_instance(argv[i], f_ecriture);
    }

    fclose(f_ecriture);
    return 0;
}