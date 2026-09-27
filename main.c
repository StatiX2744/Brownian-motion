/**
 * @file main.c
 * @brief Point d'entrée du programme de simulation du mouvement brownien.
 */

#include <stdio.h>

#include "src/simulation.h"

/**
 * @brief Affiche le menu principal et récupère le choix de l'utilisateur.
 *
 * @return Le numéro de la simulation sélectionnée.
 */
static int read_simulation_choice(void)
{
    int choice;

    puts("Ecrivez votre choix de fonction parmi :");
    puts("1) Trajectoire 1D");
    puts("2) Trajectoire 2D");
    puts("3) Trajectoire 1D moyenne");
    puts("4) Trajectoire 2D moyenne");
    puts("5) Histogramme de simulation 2D");

    if (scanf("%d", &choice) != 1) {
        fprintf(stderr, "Erreur : choix invalide.\n");
        return 0;
    }

    return choice;
}

/**
 * @brief Fonction principale du programme.
 */
int main(void)
{
    const int choice = read_simulation_choice();

    /* Lance la simulation correspondant au choix de l'utilisateur. */
    return run(choice);
}
