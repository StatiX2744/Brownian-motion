/**
 * @file simulation.h
 * @brief Déclarations des fonctions de simulation du mouvement brownien.
 */

#ifndef SIMULATION_H
#define SIMULATION_H

#include <SDL2/SDL.h>

/* Valeurs utilisées lorsque l'utilisateur lance une simulation classique. */
#define SIM_NUMBER 10000
#define SIM_SIZE   1000

/* Type numérique utilisé par les calculs de simulation. */
typedef double precision;

/**
 * @brief Coordonnées d'un point dans le plan.
 */
typedef struct {
    precision x;
    precision y;
} coo;

/**
 * @brief Domaine mathématique associé à un graphique.
 */
typedef struct {
    precision x_min;
    precision x_max;
    precision y_min;
    precision y_max;
    int x_origin;
    int y_origin;
} graph;

/* ---------- Simulation en une dimension ---------- */

precision box_muller_1D(precision u1, precision u2);
precision *trajectory_1D(int sim_size);
precision *simulation_increments_1D(int sim_number, int sim_size);
void trajectory_1D_graphique(SDL_Renderer *renderer, int sim_size);
void trajectory_average_graph(SDL_Renderer *renderer, int sim_size,
                              int sim_number);
void add_trajectory_1D(precision *tab, int size);

/* ---------- Simulation en deux dimensions ---------- */

coo box_muller_2D(precision u1, precision u2);
coo *trajectory_2D(int sim_size);
coo *simulation_increments_2D(int sim_number, int sim_size);
void trajectory_2D_live(SDL_Renderer *renderer, int sim_size);
void trajectory_2D_average(SDL_Renderer *renderer, int sim_size,
                           int sim_number);
void trajectory_2D_graphique(SDL_Renderer *renderer, int sim_size);
void trajectory_2D_average_graph(SDL_Renderer *renderer, int sim_size,
                                 int sim_number);
void add_trajectory_2D(coo *tab, int size);

/* ---------- Simulation de l'atteinte d'une cible ---------- */

int nb_step(coo target, precision gap, int max_interval);
void histogram2D(int size_interval, int sim_number, int sim_size,
                 coo target, precision gap);

/* ---------- Gestion du programme ---------- */

int run(int id);

#endif /* SIMULATION_H */
