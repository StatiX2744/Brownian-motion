/**
 * @file graphing.h
 * @brief Fonctions de dessin et de conversion des coordonnées.
 */

#ifndef GRAPHING_H
#define GRAPHING_H

#include <SDL2/SDL.h>

#include "simulation.h"

/* ---------- Style général du graphique ---------- */

#define PEN_THICKNESS      3
#define POINT_THICKNESS    2
#define CROSS_THICKNESS    3
#define CROSS_SIZE         16

/* Style des graduations principales et secondaires. */
#define MAIN_STROKE_THICKNESS   6
#define MAIN_STROKE_LENGTH      30
#define SECOND_STROKE_THICKNESS 4
#define SECOND_STROKE_LENGTH    15

/* Graduations des graphiques 1D. */
#define HORIZONTAL_GAP       100.0
#define HORIZONTAL_SUBLINES  2
#define VERTICAL_GAP         20.0
#define VERTICAL_SUBLINES    1

/* Graduations des graphiques 2D. */
#define HORIZONTAL_GAP_2D       5
#define HORIZONTAL_SUBLINES_2D  1
#define VERTICAL_GAP_2D         5
#define VERTICAL_SUBLINES_2D    1

/* Marges autour des valeurs affichées. */
#define LEFT_PADDING      25.0
#define RIGHT_PADDING      3.0
#define VERTICAL_PADDING   3.0

/* Taille de police utilisée pour les graduations. */
#define NUMBERS_SIZE 30

/* Paramètres utilisés par la simulation 2D. */
#define MAX_2D_SIZE      5
#define DIMINUTION_FACTOR 15

/**
 * @brief Dessine une ligne horizontale ou verticale avec une épaisseur donnée.
 */
void draw_line(SDL_Renderer *renderer, int x1, int y1,
               int x2, int y2, int thickness);

/**
 * @brief Convertit une abscisse mathématique en abscisse écran.
 */
int toGraphCoordsX(const graph *graphique, precision x);

/**
 * @brief Convertit une ordonnée mathématique en ordonnée écran.
 */
int toGraphCoordsY(const graph *graphique, precision y);

/**
 * @brief Convertit directement un point mathématique en coordonnées SDL.
 */
SDL_Point toGraphCoords(const graph *graphique, precision x, precision y);

/**
 * @brief Dessine une graduation principale ou secondaire.
 */
void stroke_plot_lines(SDL_Renderer *renderer, const graph *graphique,
                       int position, char orientation, char main_stroke);

/**
 * @brief Dessine un point ou une croix sur le graphique.
 */
void plot(SDL_Renderer *renderer, const graph *graphique,
          precision x, precision y, short int plot_style);

/**
 * @brief Dessine une courbe à partir d'un tableau d'ordonnées.
 */
void plot_line(SDL_Renderer *renderer, const graph *graphique,
               const precision *points_y, int points_x_spread,
               int nb_points);

/**
 * @brief Crée et dessine le repère d'un graphique.
 */
graph *create_graph(SDL_Renderer *renderer,
                    precision x_min, precision x_max,
                    precision y_min, precision y_max,
                    precision graduation_interval_x, int nb_sublines_x,
                    precision graduation_interval_y, int nb_sublines_y);

/**
 * @brief Dessine une trajectoire composée de points 2D.
 */
void plot_2D(SDL_Renderer *renderer, const graph *graphic,
             const coo *tab, int size);

/**
 * @brief Dessine un histogramme.
 */
void draw_histogram(SDL_Renderer *renderer, const int *histogram, int size);

#endif /* GRAPHING_H */
