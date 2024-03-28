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

int main(int argc, char **argv)
{
    FILE *f_ecriture_liste = fopen("graphs/liste_analyse.txt", "w");
    FILE *f_ecriture_hash_AQ = fopen("graphs/liste_hash_AQ.txt", "w");

    if (f_ecriture_liste == NULL || f_ecriture_hash_AQ == NULL)
    {
        printf("Erreur lors de l'ouverture des fichiers\n");
        return 1;
    }

    srand(time(NULL));

    fprintf(f_ecriture_liste, "Nb_chaines Liste\n");
    fprintf(f_ecriture_hash_AQ, "Nb_chaines Hash Arbre\n");

    clock_t temps_init, temps_fin;
    double temps_liste = 0, temps_hash = 0, temps_arbre = 0;

    Chaines *C = NULL;
    Reseau *R = NULL;
    // Axe x : nb_chaines
    // Axy y : temps de calcul
    int nbPointsChaine = 100, xmax = 5000, ymax = 5000;

    for (int nb_chaines = 500; nb_chaines <= 5000; nb_chaines += 500)
    {
        // Liste
        printf("%d\n", nb_chaines);
        C = generationAleatoire(nb_chaines, nbPointsChaine, xmax, ymax);
        temps_init = clock();
        R = reconstitueReseauListe(C);
        temps_fin = clock();
        temps_liste = calculer_temps_cpu(temps_init, temps_fin);
        liberer_reseau(&R);
        printf("%.2f\n", temps_liste);

        // Tab de hachage
        temps_init = clock();
        // Facteur de charge ~ 80%
        R = reconstitueReseauHachage(C, (int)(nbPointsChaine * nb_chaines / 0.75));
        temps_fin = clock();
        temps_hash = calculer_temps_cpu(temps_init, temps_fin);
        liberer_reseau(&R);
        printf("%.2f\n", temps_hash);

        // Arbre quaternaire
        temps_init = clock();
        R = reconstitueReseauArbre(C);
        temps_fin = clock();
        temps_arbre = calculer_temps_cpu(temps_init, temps_fin);
        liberer_reseau(&R);
        printf("%.2f\n", temps_arbre);
        liberer_chaines(&C);

        // Ecriture dans les fichiers
        fprintf(f_ecriture_liste, "%d %.6f\n", nb_chaines, temps_liste);
        fprintf(f_ecriture_hash_AQ, "%d %.6f %.6f\n", nb_chaines, temps_hash, temps_arbre);
    }
    fclose(f_ecriture_liste);
    fclose(f_ecriture_hash_AQ);
    return 0;
}